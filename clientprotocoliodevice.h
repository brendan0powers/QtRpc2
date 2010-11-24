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
#ifndef QTRPCCLIENTPROTOCOLIODEVICE_H
#define QTRPCCLIENTPROTOCOLIODEVICE_H

#include <ClientProtocolBase>
#include <Signature>
#include <ReturnValue>
#include <QtRpcGlobal>

namespace QtRpc
{

class Message;
class ClientProtocolIODevicePrivate;

/**
This class handles all of the basic protocol actions like sending and receiving messages. It cannot be used directly, and is instead inherited and implemented for specific uses like Tcp or local sockets. This class requires a functional QIODevice to send and receive it's messages on, which is initialized by calling prepareDevice(QIODevice*).

	@sa ClientProtocolTcp ClientProtocolSocket ServerProtocolInstanceIODevice
	@brief This is the base class for all QIODevice based classes.
	@author Chris Vickery <chris@resara.com>
*/
/*
Inherited classes must implement the following virtual functions to be functional:

protected slots:
	virtual void connected();
protected:
	virtual ReturnValue setProperty(QString, QVariant);
	virtual ReturnValue getProperty(QString);
	virtual ReturnValue protocolConnect(Arguments);
	virtual ReturnValue protocolDisconnect();
	virtual void protocolFunction(Signature, Arguments);
*/
class QTRPC2_EXPORT ClientProtocolIODevice : public ClientProtocolBase
{
	QXT_DECLARE_PRIVATE(ClientProtocolIODevice);
	Q_OBJECT
public:
	ClientProtocolIODevice(QObject* parent = 0);
	~ClientProtocolIODevice();
public slots:
// 	virtual void callbackReturn(uint , ReturnValue);
	virtual void callbackReturn(Message msg);

protected slots:
	void callProtocolFunction(Signature, Arguments);

	/** \fn virtual void connected()
	 * This function is called when the connection reaches the connected state. It should be used for initializing things before Service mode is activated.
	 */
	virtual void connected() = 0;

protected:
	/** \fn virtual void protocolFunction(Signature func, Arguments args)
	* This function is for protocol specific functions. Protocol functions don't have return values, so if further communication is needed you must use callProtocolFunction() to talk to the server.
	 * @param func The Signature of the function being called
	 * @param args Arguments list of arguments passed by the function
	 */
	virtual void protocolFunction(Signature func, Arguments args) = 0;
// 	virtual void function(uint , Signature , Arguments);
	void protocolFunction(Message msg);
	virtual void function(Message msg);
	virtual bool isConnected() = 0;
	void prepareDevice(QIODevice*);

};

}

#endif
