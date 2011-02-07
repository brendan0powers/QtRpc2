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
#include "serverprotocolinstanceiodevice.h"
#include "serverprotocolinstanceiodevice_p.h"
#include "serverprotocolinstancebase_p.h"

#include <QDebug>
#include <QTimer>
#include <QThread>
#include <ReturnValue>
#include <ServiceProxy>
#include <authtoken.h>

// #define DEBUG_MESSAGES

namespace QtRpc
{

/**
 * The constructor sets the State to Connecting
 * @param serv Initialized pointer to the active Server ocject
 * @param parent Optional parent for the QObject
 */
ServerProtocolInstanceIODevice::ServerProtocolInstanceIODevice(Server* serv, QObject* parent) :
		ServerProtocolInstanceBase(serv, parent)
{
	QXT_INIT_PRIVATE(ServerProtocolInstanceIODevice);
	QObject::connect(this, SIGNAL(aboutToChangeThreads(QThread*)), &qxt_d(), SLOT(moveToThread(QThread*)));
	qxt_d().totalSize = 0;
	qxt_d().device = 0;
	qxt_d().state = Connecting;
}

/**
 * The deconstructor unsets the QDataStream object and deletes the QIODevice object.
 */
ServerProtocolInstanceIODevice::~ServerProtocolInstanceIODevice()
{
	qxt_d().stream.unsetDevice();
	if (qxt_d().device != 0)
		qxt_d().device->deleteLater();
}

void ServerProtocolInstanceIODevicePrivate::moveToThread(QThread* thread)
{
	QObject::moveToThread(thread);
}

QMutex* ServerProtocolInstanceIODevice::mutex() const
{
	return const_cast<QMutex*>(&qxt_d().mutex);
}

/**
 * This function is used by the service object to send callback functions to the client object. This function is completely asyncronous, returning immediately after sending the function call. When a reply is received it is sent to \a slot on \a obj . The receiving slot must take a uint and a ReturnValue as it's parameters.
 * @param obj The QObject* that will be receiving the return value
 * @param slot The Signature of the slot that will be receiving the return value
 * @param func The Signature of the callback function being called.
 * @param args Arguments list for the callback function
 * @return Returns the is number of the call. The same id number will be sent to \a slot
 */
uint ServerProtocolInstanceIODevice::callCallback(QObject* obj, Signature slot, quint32 servid, Signature func, Arguments args)
{
	qxt_d().mutex.lock();
	uint id = nextId();
	queue()[id].object = obj;
	queue()[id].slot = slot;
	qxt_d().mutex.unlock();
	qxt_d().writeMessage(Message(id, Message::Function, func, args, servid));
	return id;
}

/**
 * This function is used for calling protocol functions. Protocol functions do not have return values, and their id number is always 0.
 * @param func The Signature of the function to be called
 * @param args Arguments list for the protocol function
 */
void ServerProtocolInstanceIODevice::callProtocolFunction(Signature func, Arguments args)
{
	qxt_d().writeMessage(Message(0, Message::QtRpc, func, args));
}

/**
 * This function is used for emiting events to the client. Events do not have return values, and their id number is always 0.
 * @param func The Signature of the event to be emited
 * @param args Arguments list for the event
 */
void ServerProtocolInstanceIODevice::sendEvent(quint32 id, Signature func, Arguments args)
{
	qxt_d().writeMessage(Message(0, Message::Event, func, args, id));
}

/**
 * This function is used internally for reading from the QIODevice. It loops until there is no more new data, reading with readMessage() and parsing the resulting Message object. This function primarily routes the function calls to the correct place, doing some error checking along the way.
 */
void ServerProtocolInstanceIODevicePrivate::readyRead()
{
	while (device->bytesAvailable() != 0)
	{
		if (totalSize == 0)
		{
			if (device->bytesAvailable() < sizeof(totalSize))
				break;
			stream >> totalSize;
			read = 0;
			buffer.resize(0);
			buffer.reserve(totalSize);
		}
		QByteArray buff(device->read((totalSize - read)));
		int i = buff.size();
		buffer.append(buff);
		if (i < 0)
		{
			qCritical(qPrintable(QString("Oh snap! An error occured while reading from the network!" + device->errorString())));
			disconnect();
			return;
		}
		read += i;
		if (totalSize <= read)
		{
			totalSize = 0;
			read = 0;
			Message msg;
			msg.setVersion(version);
			{
				QDataStream stream(buffer);
				stream >> msg;
			}
#ifdef DEBUG_MESSAGES
			qDebug() << "R:" << msg.signature();
#endif
			if (!parseMessage(msg))
			{
				qWarning() << "Received a bad message, attempting to read as version 0";
				msg = Message();
				{
					QDataStream stream(buffer);
					stream >> msg;
				}
#ifdef DEBUG_MESSAGES
				qDebug() << "R:" << msg.signature();
#endif
				parseMessage(msg);
			}
		}
	}
}

bool ServerProtocolInstanceIODevicePrivate::parseMessage(Message msg)
{
	switch (msg.type())
	{
		case Message::Function:
			if (qxt_p().state() != ServerProtocolInstanceIODevice::Ready)
			{
				qxt_p().callProtocolFunction(Signature("error(int,QString)"), Arguments() << 2 << "Function calls cannot be made until the server is ready.");
				break;
			}
			{
				ReturnValue ret = qxt_p().callFunction(msg);
				if (!ret.isAsyncronous())
					writeMessage(Message(msg.id(), ret));
			}
			break;
		case Message::QtRpc:
		{
			if (checkProtocolFunction(msg))
				break;
			qxt_p().protocolFunction(msg.signature(), msg.arguments());
		}

		break;
		case Message::Return:
			if (qxt_p().queue().contains(msg.id()))
			{
				ServerProtocolInstanceBase::ReplySlot slot = qxt_p().queue().take(msg.id());
				QMetaObject::invokeMethod(slot.object, qPrintable(slot.slot.name()), Qt::DirectConnection, Q_ARG(uint, msg.id()), Q_ARG(ReturnValue, msg.returnValue()));
			}
			break;
		case Message::Invalid:
		default:
			qCritical() << "Error: Received invalid message";
			return false;
	}
	return true;
}

/**
 * This function is used internally for preimplemented protocol functions. This function is only used by readyRead(), and should never be called any other way. This function is extremely important for the QIODevice class to function properly.
 * @param msg Message object of the received protocol function
 * @return Returns true when the function is preimplemented, or false when the protocol should view it.
 */
bool ServerProtocolInstanceIODevicePrivate::checkProtocolFunction(Message msg)
{
	if (msg.signature().name() == "setProtocolVersion")
	{
		if (msg.arguments().count() > 0)
		{
			quint32 cversion = msg.arguments()[0].toUInt();
			if (cversion > Message::currentVersion())
				cversion = Message::currentVersion();
			writeMessage(Message(0, Message::QtRpc, Signature("setProtocolVersion(quint32)"), Arguments() << cversion));
			version = cversion;
			return true;
		}
		// The client is talking some other language, apparently...
		qxt_p().disconnect();
		return true;
	}
	else if (msg.signature().name() == "selectService")
	{
		if (state == ServerProtocolInstanceIODevice::Connecting)
		{
			if (version == 0)
				qxt_p().callProtocolFunction(Signature("error(int,QString)"), Arguments() << 1 << "You cannot select a service until the server enters the Service state.");
			else
				writeMessage(Message(msg.id(), ReturnValue(1, "You cannot select a service until the server enters the Service state.")));
		}
		else
		{
			if (version == 0)
			{
				ReturnValue ret = qxt_p().getServiceObject(msg.arguments()[0].toString(), msg.arguments()[1].toString(), msg.arguments()[2].toString());
				if (ret.isError())
				{
					qxt_p().callProtocolFunction(Signature("error(int,QString)"), Arguments() << 2 << ret.errString());
					qCritical() <<  "Failed to select service:" << msg.arguments()[0].toString();
				}
				else
				{
					qxt_p().callProtocolFunction(Signature("ready(QVariant)"), Arguments() << static_cast<QVariant>(ret));
					qxt_p().changeState(ServerProtocolInstanceIODevice::Ready);
				}
				return true;
			}
			Message reply;
			if (msg.arguments().count() >= 3)
				reply = Message(msg.id(), qxt_p().getServiceObject(msg.arguments()[0].toString(), msg.arguments()[1].toString(), msg.arguments()[2].toString()));
			else
				reply = Message(msg.id(), qxt_p().getServiceObject(msg.arguments()[0].toString()));
			if (reply.returnValue().isService())
			{
				qxt_p().changeState(ServerProtocolInstanceIODevice::Ready);
			}
			writeMessage(reply);
		}
		return true;
	}
	else if (msg.signature().name() == "error")
	{
		switch (msg.arguments()[0].toInt())
		{
			case 0: //Warning;
				qWarning() << "Warning: " << msg.arguments()[1].toString();
				break;
			case 1: //Error
				qCritical() << "Error: " << msg.arguments()[1].toString();
				break;
			case 2: //Fatal
				qCritical() << "Fatal Error: " << msg.arguments()[1].toString();
				disconnect();
				break;
			default:
				qCritical() << "Unknown Error Level: " << msg.arguments()[1].toString();
		}
		return true;
	}
	else if (msg.signature().name() == "listServices")
	{
		writeMessage(Message(msg.id(), qxt_p().listServices()));
	}
	else if (msg.signature().name() == "listFunctions")
	{
		if (msg.arguments().count() > 0)
			writeMessage(Message(msg.id(), qxt_p().listFunctions(msg.arguments().at(0).toString())));
		else
			writeMessage(Message(msg.id(), ReturnValue(1, "Missing parameters to function listFunctions")));
	}
	else if (msg.signature().name() == "listEvents")
	{
		if (msg.arguments().count() > 0)
			writeMessage(Message(msg.id(), qxt_p().listEvents(msg.arguments().at(0).toString())));
		else
			writeMessage(Message(msg.id(), ReturnValue(1, "Missing parameters to function listEvents")));
	}
	else if (msg.signature().name() == "listCallbacks")
	{
		if (msg.arguments().count() > 0)
			writeMessage(Message(msg.id(), qxt_p().listCallbacks(msg.arguments().at(0).toString())));
		else
			writeMessage(Message(msg.id(), ReturnValue(1, "Missing parameters to function listCallbacks")));
	}
	else if (msg.signature().name() == "setDefaultToken")
	{
		if (msg.arguments().count() > 0)
		{
			qxt_p().defaultToken().copy(msg.arguments()[0].value<AuthToken>());
			writeMessage(Message(msg.id(), true));
		}
		else
			writeMessage(Message(msg.id(), ReturnValue(1, "Missing parameters to function setDefaultToken")));
	}
	return false;
}

/**
 * This function is used internally by the QIODevice class for reading Message objects from the QDataStream. This function should never be called manually as it will likely break everything.
 * @return Returns the Message read from the device
 */

/**
 * This function is the generic message writing function. It is safe to call this function manually, though it is a much better idea to use the convenience functions provided.
 * @sa callProtocolFunction callCallback sendEvent
 * @param msg Message object to write to the network
 */
void ServerProtocolInstanceIODevicePrivate::writeMessage(Message msg)
{
	msg.setVersion(version);
#ifdef DEBUG_MESSAGES
	if (msg.type() == Message::Return)
		qDebug() << "S:" << msg.returnValue();
	else
		qDebug() << "S:" << msg.signature();
#endif
	stream << msg.size() << msg;
}

/**
 * This function must be called before any of the messaging functions may be used. This function initializes the QDataStream object, connects all neccesary signals and slots, and parents the QIODevice object.
 * @param device The device to be used by the instance object.
 */
void ServerProtocolInstanceIODevice::prepareDevice(QIODevice* device)
{
	qxt_d().device = device;
	qxt_d().device->setParent(this);
	connect(qxt_d().device, SIGNAL(disconnected()), this, SLOT(disconnect()));
	connect(qxt_d().device, SIGNAL(readyRead()), &qxt_d(), SLOT(readyRead()), Qt::DirectConnection);
	qxt_d().stream.setDevice(qxt_d().device);
#ifdef DEBUG_MESSAGES
	qDebug() << "Sending welcome message" << qxt_d().version;
#endif
	callProtocolFunction(Signature("welcome(quint32)"), Arguments() << Message::currentVersion());
}

/**
 * This function exposes the state of the instance object.
 * @sa changeState()
 * @return Returns the current state of the instance object
 */
ServerProtocolInstanceIODevice::State ServerProtocolInstanceIODevice::state()
{
	return qxt_d().state;
}

/**
 * This function is used for setting the state of the instance. State's cannot go backwards.
 * @param state The new state
 */
void ServerProtocolInstanceIODevice::changeState(State state)
{
	if (state <= qxt_d().state)
		return;
	qxt_d().state = state;
	callProtocolFunction(Signature("stateChanged(int)"), Arguments() << qxt_d().state);
}

void ServerProtocolInstanceIODevice::writeMessage(Message msg)
{
	qxt_d().writeMessage(msg);
}

}
