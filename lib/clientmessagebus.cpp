/***************************************************************************
 *  Copyright (c) 2011, Resara LLC                                         *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  Redistribution and use in source and binary forms, with or without     *
 *  modification, are permitted provided that the following conditions are *
 *  met:                                                                   *
 *      * Redistributions of source code must retain the above copyright   *
 *        notice, this list of conditions and the following disclaimer.    *
 *      * Redistributions in binary form must reproduce the above          *
 *        copyright notice, this list of conditions and the following      *
 *        disclaimer in the documentation and/or other materials           *
 *        provided with the distribution.                                  *
 *      * Neither the name of Resara LLC nor the names of its              *
 *        contributors may be used to endorse or promote products          *
 *        derived from this software without specific prior written        *
 *        permission.                                                      *
 *                                                                         *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RESARA LLC BE   *
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR    *
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF   *
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        *
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,  *
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE   *
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN *
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                          *
 *                                                                         *
 ***************************************************************************/
#include "clientmessagebus.h"
#include "clientmessagebus_p.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QFile>

#include <Message>
#include <ClientProtocolThread>
//Protocols
#include <ClientProtocolTest>
#include <ClientProtocolTcp>
#include <ClientProtocolSocket>
#include "sleeper.h"

namespace QtRpc
{

/**
 * The constructor simply sets the initial id number and makes sure that it is not running in the wrong thread.
 * @param parent Optional parent for the QObject.
 */
ClientMessageBus::ClientMessageBus(QObject *parent) : QObject(parent)
{
	QXT_INIT_PRIVATE(ClientMessageBus);
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
	{
		qFatal("Fatal Error: Do not create ClientMessageBus objects directly, use ClientProtocolThread instead.");
	}
	qxt_d().curid = 0;
	QObject::connect(this, SIGNAL(disconnected()), &qxt_d(), SLOT(disconnected()));
}

/**
 * The deconstructor doesn't do anything.
 */
ClientMessageBus::~ClientMessageBus()
{
	qxt_d().disconnected();
#ifdef USE_MESSAGEBUS_FIX
	Sleeper::usleep(10000);
	qxt_d().disconnected();
#endif
}

void ClientMessageBusPrivate::disconnected()
{
	mutex.lock();
	disconnect(this, SLOT(returnReceived(Message)));
	ReturnValue ret(1, "Disconnected from server");
	QList<uint> keys = wm.keys();
	mutex.unlock();
	foreach(uint id, keys)
	{
		returnReceived(Message(id, ret));
	}
	waiter.wakeAll();
}

/**
 * This function is called when creating a new ClientMessageBus object. It will automatically create a new ClientProtocolThread and place a functional message bus and the selected protocol object in that thread. The function then returns the fully initialized message bus.
 * @param protocol The string value of the protocol, like "tcp" or "socket"
 * @return Returns a pointer to the newly initialized ClientMessageBus.
 */
ClientMessageBus* ClientMessageBus::instance(QString protocol)
{
	if (protocol == "test")
	{
		ClientProtocolThread* thread = new ClientProtocolThread();
		return thread->init<ClientProtocolTest>();
	}
	else if (protocol == "tcp" || protocol == "tcps")
	{
		ClientProtocolThread* thread = new ClientProtocolThread();
		return thread->init<ClientProtocolTcp>();
	}
#ifndef Q_OS_WIN32
	else if (protocol == "socket")
	{
		ClientProtocolThread *thread = new ClientProtocolThread();
		return thread->init<ClientProtocolSocket>();
	}
#endif
	qCritical() << "Warning: Unsupported protocol selected, " << protocol;
	return NULL;
}

/**
 * This function is used for calling syncronous calls. It can be used for both protocol functions and for server functions. It simply makes the call and then blocks until it gets a ReturnValue, or until \a timeout seconds have gone by. This function is completely thread safe and should, in fact, never be called from the same thread that the ClientMessageBus lies on.
 * @param func Signature of the remote function
 * @param args Arguments list for the remote function
 * @param timeout Time in milliseconds before timing out.
 * @return Returns the ReturnValue from the server, or an error if something goes wrong.
 */
ReturnValue ClientMessageBus::callFunction(Signature func, Arguments args, int timeout)
{
	Q_ASSERT(false);
	qCritical() << "Old ClientMessageBus::callFunction method being used, this could cause serious problems if it's used wrong (which it probably is)";
	return callFunction(Message(0, Message::QtRpc, func, args), timeout);
}

ReturnValue ClientMessageBus::callFunction(Message msg, int timeout)
{
	QMutexLocker locker(&qxt_d().mutex);
	if (QThread::currentThread() == this->thread())
	{
		qCritical() << "Error: You cannot call functions from the same thread as the message bus, this breaks EVERYTHING.";
		return ReturnValue(1, "You cannot call functions from the same thread as the message bus, this breaks EVERYTHING.");
	}
	qxt_d().curid++;
	msg.setId(qxt_d().curid);
// 	qDebug() << "SEND:" << msg;
	qxt_d().wm[msg.id()].sync = true;
	emit sendFunction(msg); //Make the function call (across thread boundary)
	QTime timer(0, 0, 0, 0);
	timer.start();
	while ((qxt_d().waiter.wait(&qxt_d().mutex, (timeout - timer.elapsed()))) && (timer.elapsed() < timeout))  //Wait for the return data.
	{
		if (qxt_d().returnValues.contains(msg.id()))
		{
			return qxt_d().returnValues.take(msg.id()); //data found, return from the function
		}
	}
	return ReturnValue(1, "Timed out while waiting for reply on the MessageBus.");
}

/**
 * This function is used for calling asyncronous calls. It can be used for both protocol functions and for server functions. It returns immediately after the call is made, and sends the ReturnValue to \a slot on \a obj .
 * @param obj A pointer to the QObject who will be receiving the ReturnValue
 * @param slot The Signature object for the slot that will receive the ReturnValue. The slot must take a uint and a ReturnValue as it's parameters.
 * @param func Signature of the remote function
 * @param args Arguments list for the remote function
 * @return Returns the id of the function call.
 */
int ClientMessageBus::callFunction(QObject* obj, Signature slot, Signature func, Arguments args)
{
	Q_ASSERT(false);
	qCritical() << "Old ClientMessageBus::callFunction method being used, this could cause serious problems if it's used wrong (which it probably is)";
	return callFunction(obj, slot, Message(0, Message::QtRpc, func, args));
}

int ClientMessageBus::callFunction(QObject* obj, Signature slot, Message msg)
{
	QMutexLocker locker(&qxt_d().mutex);
	if (QThread::currentThread() == this->thread())
	{
		qWarning() << "You should not call functions from the same thread as the message bus.";
	}
	qxt_d().curid++;
	msg.setId(qxt_d().curid);
// 	qDebug() << "SEND:" << msg;
	qxt_d().wm[msg.id()].sync = false;
	qxt_d().wm[msg.id()].object = obj;
	qxt_d().wm[msg.id()].slot = slot.name();
	emit sendFunction(msg); //Make the function call (across thread boundary)
	return msg.id();
}

/**
 * This function parses \a ret and routes it to the correct place by \a id . For internal use only.
 * @param id The id number of the function call
 * @param ret The ReturnValue of the function call.
 */
void ClientMessageBusPrivate::returnReceived(Message msg)
{
// 	qDebug() << "RECV:" << msg;
	QMutexLocker locker(&mutex);
	if (!wm.contains(msg.id()))
	{
		qWarning() << "Wait list does not contain id," << msg.id() << "this is likely because the syncronous call timed out.";
		return;
	}
	ClientMessageBusPrivate::WaitingMessage wmessage = wm.take(msg.id());
	if (wmessage.sync) //If it's a syncronous call then simply set the data and wake the threads.
	{
		returnValues[msg.id()] = msg.returnValue();
		waiter.wakeAll();
		return;
	}
	if (wmessage.object.isNull())
	{
		return;
	}
	//If it's an asyncronous call then use a QueuedConnection invokeMethod to send the reply to the correct slot.
	if (!QMetaObject::invokeMethod(wmessage.object, qPrintable(wmessage.slot), Qt::QueuedConnection, Q_ARG(uint, msg.id()), Q_ARG(ReturnValue, msg.returnValue())))
		qCritical() << "Error: QMetaObject::invokeMethod returned false. (" << wmessage.object << ", " << wmessage.slot;
}


}

