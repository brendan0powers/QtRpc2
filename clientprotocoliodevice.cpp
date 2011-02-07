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
#include "clientprotocoliodevice.h"
#include "clientprotocoliodevice_p.h"

#include <QDebug>
#include <QThread>
#include <QUrl>

// #define DEBUG_MESSAGES

namespace QtRpc
{

/**
 *
This is the default constructor, it doesn't do anything.
 * @param parent Optional parent to the QObject.
 */
ClientProtocolIODevice::ClientProtocolIODevice(QObject* parent)
		: ClientProtocolBase(parent)
{
	QXT_INIT_PRIVATE(ClientProtocolIODevice);
	QObject::connect(this, SIGNAL(aboutToChangeThreads(QThread*)), &qxt_d(), SLOT(moveToThread(QThread*)));
	qxt_d().totalSize = 0;
}

/**
 * The default deconstructor does not do anything, and does not delete the QIODevice, that is up to the inherited class to do so.
 */
ClientProtocolIODevice::~ClientProtocolIODevice()
{
}

void ClientProtocolIODevicePrivate::moveToThread(QThread* thread)
{
	QObject::moveToThread(thread);
}

/**
 *
This function sends a basic function call to the service. This function is 100% asyncrionous, returning when it finishes sending the message. This function should never be called directly, only by the ClientMessageBus
 * @param id Id number for the Message
 * @param func Signature object for the Message
 * @param args Arguments list for the Message
 */
void ClientProtocolIODevice::function(Message msg)
{
	qxt_d().writeMessage(msg);
}

/**
 *
This function is called by the message bus to send the return value of a callback function to the service.
 * @param id Id number matching the original callback function
 * @param ret ReturnValue from the callback function
 */
void ClientProtocolIODevice::callbackReturn(Message msg)
{
	qxt_d().writeMessage(msg);
}

/**
 *
This function preparses all protocol functions to check if they are a generic QtRpc call. The generic QtRpc function calls are welcome(), error(int,QString), stateChanged(int), and ready(QVariant). This function is private and is used internally by the ClientProtocolIODevice class.
 * @param msg
 * @return Function returns true when it is a generic command, or false when it is not.
 */
bool ClientProtocolIODevicePrivate::checkProtocolFunction(Message msg)
{
	QString name = msg.signature().name().trimmed();
	if (name == "welcome")
	{
		if (msg.arguments().count() > 0) //qtrpc2 versioning support
		{
			quint32 sversion = msg.arguments()[0].toUInt();
			if (sversion > Message::currentVersion())
				sversion = Message::currentVersion();
			qxt_p().callProtocolFunction(Signature("setProtocolVersion(quint32)"), Arguments() << sversion);
		}
		else
		{
			qxt_p().connected();
		}
		return true;
	}
	else if (name == "setProtocolVersion")
	{
		if (msg.arguments().count() > 0)
		{
			quint32 sversion = msg.arguments()[0].toUInt();
			if (sversion > Message::currentVersion())
			{
				// The server is now reading in a verion not supported by this client, there is no hope.
				qxt_p().disconnect();
				return true;
			}
			qxt_p().setVersion(sversion);
			qxt_p().connected();
			return true;
		}
		// The server is now reading in a verion not supported by this client, there is no hope.
		qxt_p().disconnect();
		return true;
	}
	else if (name == "error")
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
				if (qxt_p().connectId() != static_cast<uint>(-1))
				{
					emit qxt_p().returnReceived(Message(qxt_p().connectId(), ReturnValue(2, msg.arguments()[1].toString())));
				}
				qxt_p().protocolDisconnect();
				break;
			default:
				qCritical() << "Unknown Error Level: " << msg.arguments()[1].toString();
		}
		return true;
	}
	else if (name == "stateChanged")
	{
		switch (msg.arguments()[0].toInt())
		{
			case 0:// Connecting, we should never see this though...
				break;
			case 1:// Service
				if (qxt_p().getServiceName().isEmpty())
				{
					// no service to select, return from connect with true (services are optional now)
					emit qxt_p().returnReceived(Message(qxt_p().connectId(), ReturnValue(true)));
					break;
				}
				selectService();
				break;
			case 2:// Ready
				break;
			default: // error
				break;
		}
		return true;
	}
	else if (name == "ready")
	{
		emit qxt_p().returnReceived(Message(qxt_p().selectServiceId(), ReturnValue(msg.arguments()[0])));
		return true;
	}
	return false;
}

/**
 * This function tells the server which service to use. This function should not be called until the server has given the go ahead to do so by setting it's state to Service. This function is private and is used internally by the ClientProtocolIODevice class.
 */
void ClientProtocolIODevicePrivate::selectService()
{
	Arguments args;
	args << qxt_p().getServiceName() << qxt_p().url().userName() << qxt_p().url().password();
	qxt_p().callProtocolFunction(Signature("selectService(QString,QString,QString)"), args);
}

/**
 *
This function writes a generic Message object to the QIODevice used by the class. This function should not be called until prepareDevice() has been called.
 * @param msg Message to send to the server
 */
void ClientProtocolIODevicePrivate::writeMessage(Message msg)
{
	Q_ASSERT(thread() == QThread::currentThread());
	if (!qxt_p().isConnected())
	{
		qCritical() << "Attempting to send a message while not connected";
		emit qxt_p().returnReceived(Message(msg.id(), ReturnValue(1, "Not Connected")));
		return;
	}
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	msg.setVersion(qxt_p().version());

#ifdef DEBUG_MESSAGES
	if (msg.type() == Message::Return)
		qDebug() << "S:" << msg.returnValue();
	else
		qDebug() << "S:" << msg.signature();
#endif

	stream << msg.size() << msg;
	device->write(array);
	return;
	while (device->bytesToWrite() > 0)
	{
		if (!device->waitForBytesWritten(25))
		{
			qCritical() << "Failed to wait for bytes written";
			emit qxt_p().returnReceived(Message(msg.id(), ReturnValue(1, device->errorString())));
			return;
		}
	}
}

/**
 *
This function reads a generic Message object from the QIODevice used by the class. This function should not be called until prepareDevice() has been called. In addition, this function should not be called manually unless you know what you're doing, as it could break everything.
 * @return Message object read from the QIODevice
 */

/**
 *
This function prepares the QIODevice object for communication, connecting all nessisary signals and slots and assigning it to the QDataStream. This function must be called before any communications are done.
 * @param device QIODevice object to be used as the communication bus.
 */
void ClientProtocolIODevice::prepareDevice(QIODevice* device)
{
	qxt_d().device = device;
	qxt_d().device->setParent(this);
	connect(qxt_d().device, SIGNAL(readyRead()), &(qxt_d()), SLOT(readyRead()));
	qxt_d().stream.setDevice(qxt_d().device);
}

/**
 *
This slot is connected to the readyRead() signal on the QIODevice object. It loops through all the available data until no more data can be read. This function is private and is used internally by the ClientProtocolIODevice class.
 */
void ClientProtocolIODevicePrivate::readyRead()
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
			qxt_p().disconnect();
			return;
		}
		read += i;

		if (totalSize <= read)
		{
			totalSize = 0;
			read = 0;
			Message msg;
			msg.setVersion(qxt_p().version());
			QDataStream stream(buffer);
			stream >> msg;
#ifdef DEBUG_MESSAGES
			qDebug() << "R:" << msg.signature();
#endif
			parseMessage(msg);
		}
	}
}

void ClientProtocolIODevicePrivate::parseMessage(Message msg)
{
	switch (msg.type())
	{
		case Message::Function:
			emit qxt_p().sendCallback(msg);
			break;
		case Message::QtRpc:
			if (checkProtocolFunction(msg))
				break;
			qxt_p().protocolFunction(msg);
			break;
		case Message::Return:
			emit qxt_p().returnReceived(msg);
			break;
		case Message::Event:
			emit qxt_p().sendEvent(msg);
			break;
		case Message::Invalid:
		default:
			qCritical() << "Error: Received invalid message" << msg.type();
	}
}

void ClientProtocolIODevice::protocolFunction(Message msg)
{
	protocolFunction(msg.signature(), msg.arguments());
}

// 01 02 15 16 17 20 21 22
// 04 03 14 13 18 19 24 23
// 05 08 09 12 31 30 25 26
// 06 07 10 11 32 29 28 27
// 59 58 55 54 33 36 37 38
// 60 57 56 53 34 35 40 39
// 61 62 51 52 47 46 41 42
// 64 63 50 49 48 45 44 43

/**
 *
This function sends a protocol function to the server. Protocol messages do not have return values, so they do not have id numbers.
 * @param func Signature object for the Message
 * @param args Argument list for the Message
 */
void ClientProtocolIODevice::callProtocolFunction(Signature func, Arguments args)
{
	qxt_d().writeMessage(Message(0, Message::QtRpc, func, args));
}


}
