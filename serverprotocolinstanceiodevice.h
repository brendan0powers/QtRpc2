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
#ifndef QTRPCSERVERPROTOCOLINSTANCEIODEVICE_H
#define QTRPCSERVERPROTOCOLINSTANCEIODEVICE_H

#include <QObject>
#include <QxtPimpl>

#include <ServerProtocolInstanceBase>
#include <Message>
#include <QtRpcGlobal>

class QMutex;

namespace QtRpc
{

class ServerProtocolInstanceIODevicePrivate;

/**
	This is the parent class to all the QIODevice related instance objects. This class implements most of the messaging functions, using the Message object to communicate with ClientProtocolIODevice classes.

	This class uses a QDataStream attached to a QIODevice to do all communication. To initialize the QIODevice the child class must call prepareDevice().

	@sa ServerProtocolInstanceBase ClientProtocolIODevice
	@brief Parent class to all the QIODevice related server instances
	@author Chris Vickery <chris@resara.com>

*/

/*
child classes must implement the following functions:
public:
	virtual void disconnect();
	virtual void setProperty(QString, QVariant);
	virtual QVariant getProperty(QString);
public slots:
	virtual void init();
protected:
	virtual void protocolFunction(Signature, Arguments);
*/
class QTRPC2_EXPORT ServerProtocolInstanceIODevice : public ServerProtocolInstanceBase
{
	QXT_DECLARE_PRIVATE(ServerProtocolInstanceIODevice);
	Q_OBJECT
public:
	/**
	 * This enum represents the state of the connection.
	 */
	enum State
	{
		Connecting,	/**< A connection has been made, but it is not yet ready to be used */
		Service,		/**< Waiting for the client to ask for a Service object */
		Ready		/**< Ready and initialized */
	};

	ServerProtocolInstanceIODevice(Server* serv, QObject* parent);

	~ServerProtocolInstanceIODevice();

	virtual void sendEvent(quint32 id, Signature, Arguments);
	State state();
public slots:
	virtual uint callCallback(QObject*, Signature, quint32 id, Signature, Arguments);
protected slots:
	void callProtocolFunction(Signature, Arguments);
protected:
	QMutex* mutex() const;
	void changeState(State);
	void prepareDevice(QIODevice*);
	/**
	 * This function is called from protocol specific functions that are not preimplemented by checkProtocolFunction(). This function should be used for all communication between server and client protocol objects.
	 * @sa callProtocolFunction checkProtocolFunction
	 * @param func The Signature of the function being called
	 * @param args Arguments list for the function being called
	 */
	virtual void protocolFunction(Signature func, Arguments args) = 0;
	virtual void writeMessage(Message);

};

}

#endif
