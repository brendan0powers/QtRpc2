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
#ifndef QTRPCCLIENTMESSAGEBUS_H
#define QTRPCCLIENTMESSAGEBUS_H

#include <QObject>
#include <QxtPimpl>
#include <Signature>
#include <ReturnValue>
#include <Message>
#include <qtrpcprivate.h>

#define USE_MESSAGEBUS_FIX

namespace QtRpc
{
class Message;
class ClientMessageBusPrivate;

typedef QList<QVariant> Arguments;

/**
	This class is primary used for routing functions between the ClientProxy and the ClientProtocolBase. It also makes syncronous calls on the client side work, and it creates the protocol objects with it's static member, instance(). This ClientMessageBus requires a seperate thread to work correctly, and should never be created manually, but instead through the instance() function.

	This class is, in nearly all cases, used explusively internally by the ClientProxy for communication. If needed, though I can't think of an instance where it would be, it works fine being used manually so long as you connect to the sendCallback() and sendEvent() signals. Also, when used manually, make sure to keep the id numbers in line.

	When the bus is destroyed, likely by the Qt deleteLater method, it cleans up the thread and the protocol object also.

	@brief Used for routing functions between the ClientProxy and the ClientProtocolBase
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ClientMessageBus : public QObject
{
	Q_OBJECT
	QXT_DECLARE_PRIVATE(ClientMessageBus);
	friend class ClientProtocolThread;
public:
	ClientMessageBus(QObject *parent = 0);
	~ClientMessageBus();
	ReturnValue callFunction(Signature, Arguments args = Arguments(), int timeout = 60000);
	static ClientMessageBus* instance(QString);
// 	void deleteLater();
public slots:
	int callFunction(QObject*, Signature, Signature, Arguments args = Arguments());
	int callFunction(QObject* obj, Signature slot, Message msg);
	ReturnValue callFunction(Message msg, int timeout = 60000);
// 	void returnReceived(uint, ReturnValue);
signals:
	/**
	 *        This signal is emited when the protocol becomes disconnected.
	 */
	void disconnected();
	/**
	 *        This signal is used to communicate to the protocol object that a function is being called.
	 * @param id The id number of the call
	 * @param func Signature of the remote function
	 * @param args Arguments list for the remote function
	 */
// 	void sendFunction(uint id, Signature func, Arguments args);
	void sendFunction(Message msg);
	/**
	 *        This signal is used to communicate to the client object that a callback function is being called.
	 * @param id The id number of the call
	 * @param func Signature of the callback function
	 * @param args Arguments list for the callback function
	 */
// 	void sendCallback(uint id, Signature func, Arguments args);
	void sendCallback(Message msg);
	/**
	 *        This signal is used to communicate the ReturnValue of a callback function to the protocol object.
	 * @param id The id number of the call
	 * @param ret The ReturnValue of the callback function.
	 */
// 	void callbackReturn(uint id, ReturnValue ret);
	void callbackReturn(Message msg);
	/**
	 *        This signal is used to communicate to the client object that an event has been emited
	 * @param func The Signature of the event.
	 * @param args Arguments list for the event.
	 */
// 	void sendEvent(Signature func, Arguments args);
	void sendEvent(Message msg);
};

}

#endif
