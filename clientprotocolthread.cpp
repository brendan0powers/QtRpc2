/***************************************************************************
 *  Copyright (c) 2010, Resara LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Resara LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL RESARA LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/
#include "clientprotocolthread.h"
#include "clientprotocolthread_p.h"

//Protocols
#include <ClientProtocolTest>
#include <Message>
#include <QDebug>
#include "clientmessagebus_p.h"
#include "sleeper.h"

namespace QtRpc
{

/**
 * Constructor
 * @param parent Optional parent for the QObject
 */
ClientProtocolThread::ClientProtocolThread(QObject *parent) : QThread(parent)
{
	QXT_INIT_PRIVATE(ClientProtocolThread);
}

/**
 * The deconstructor waits for the thread to finish excecution.
 */
ClientProtocolThread::~ClientProtocolThread()
{
	quit();
	wait();
}

/**
 * This function is run inside of the newly created thread. This function creates the ClientMessageBus and connects some important signals and slots for cleanup. Using QWaitCondition, this function then syncs up with the init function which moves the protocol object to the correct thread. This function then starts the event loop.
 */
void ClientProtocolThread::run()
{
	_mutex.lock();
	_bus = new ClientMessageBus();
	connect(_bus, SIGNAL(destroyed()), this, SLOT(quit()), Qt::QueuedConnection);
// 	connect(_bus, SIGNAL(destroyed()), this, SLOT(deleteLater()), Qt::QueuedConnection);
	qxt_d().waiter.wakeAll();
        qxt_d().waiter.wait(&_mutex);
        _protocol->setParent(_bus);
	_mutex.unlock();
	exec();
// 	Sleeper::usleep(1);
	deleteLater();
}

ClientMessageBus* ClientProtocolThread::initialize()
{
        start();
        qxt_d().waiter.wait(&_mutex);
        _protocol->moveToThread(_bus->thread());
		//Connect all the signals and slots that make the communication work....

		//disconnect signal
	connect(_protocol, SIGNAL(disconnected()), _bus, SIGNAL(disconnected()), Qt::DirectConnection);

		//server calls callback on client
	connect(_protocol, SIGNAL(sendCallback(Message)), _bus, SIGNAL(sendCallback(Message)), Qt::DirectConnection);

		//server sends return to client
	connect(_protocol, SIGNAL(returnReceived(Message)), &_bus->qxt_d(), SLOT(returnReceived(Message)), Qt::DirectConnection);

		//server sends event to client
	connect(_protocol, SIGNAL(sendEvent(Message)), _bus, SIGNAL(sendEvent(Message)), Qt::DirectConnection);

		//client sends return to server
	connect(_bus, SIGNAL(callbackReturn(Message)), _protocol, SLOT(callbackReturn(Message)), Qt::DirectConnection);

		//client calls function on server (thread boundary)
	connect(_bus, SIGNAL(sendFunction(Message)), _protocol, SLOT(sendFunction(Message)), Qt::QueuedConnection);

	qxt_d().waiter.wakeAll();
	_mutex.unlock();
	_initMutex.unlock();
	return _bus;
}

}

