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
#ifndef QTRPCCLIENTPROTOCOLBASE_H
#define QTRPCCLIENTPROTOCOLBASE_H

#include <QObject>
#include <Signature>
#include <ReturnValue>
#include <Message>
#include <QxtPimpl>
#include <qtrpcprivate.h>

class QUrl;

#define QTRPC_PROTOCOL_PREFIX ""

namespace QtRpc
{

class ClientProtocolBasePrivate;

/**
This is the base class for all of the client side protocol objects used by QtRpc. This class is primarily an interface class for the ClientMessageBus and the ClientProtocolThread but it does have a few implemented functions.

This class should never be used directly, it should always be used with a ClientProtocolThread and a ClientMessageBus. The ClientMessageBus cleans up this class automatically.

	@brief This is the base class for the client side protocol objects.
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ClientProtocolBase : public QObject
{
	QXT_DECLARE_PRIVATE(ClientProtocolBase);
	Q_OBJECT
public:
	ClientProtocolBase(QObject *parent = 0);
	~ClientProtocolBase();
	QUrl url() const;
	void setUrl(const QUrl&);
	uint connectId() const;
	void setConnectId(uint);
	uint selectServiceId() const;
	void setSelectServiceId(uint selectServiceId);
public slots:
// 	void sendFunction(uint, Signature, Arguments);
	void sendFunction(Message msg);
	/**
	 *       This slot is called by the ClientMessageBus when a ReturnValue for a callback is ready. The ReturnValue should be relayed to the server.
	 * @param id The id number of the callback function.
	 * @param ret The ReturnValue of the callback function.
	 */
// 	virtual void callbackReturn(uint id, ReturnValue ret) = 0;
	virtual void callbackReturn(Message msg) = 0;
	void moveToThread(QThread*);
signals:
	/**
	 * This signal is used to communicate to the ClientMessageBus that a callback function is being called.
	 * @param id The id number of the callback function.
	 * @param func The Signature of the callback function.
	 * @param args Arguments list for the callback function.
	 */
// 	void sendCallback(uint id, Signature func, Arguments args);
	void sendCallback(Message msg);
	/**
	 * This signal is used to communicate to the ClientMessageBus that a ReturnValue to a function call has been received
	 * @param id The id number of the function call.
	 * @param ret The ReturnValue of the function call.
	 */
// 	void returnReceived(uint id, ReturnValue ret);
	void returnReceived(Message msg);
	/**
	 *        This signal is used to communicate to the ClientMessageBus that an event has been emited
	 * @param func The Signature of the event.
	 * @param args Arguments list for the event.
	 */
// 	void sendEvent(Signature func, Arguments args);
	void sendEvent(Message msg);
	/**
	 *        This signal is used to communicate to the ClientMessageBus that the protocol has been disconnected.
	 */
	void disconnected();
	void aboutToChangeThreads(QThread*);
protected:
	virtual QString getServiceName();
	/**
	 * This function is used for sending functions to the server. It is called internally.
	 * @param id The id number of the function call.
	 * @param func The Signature of the function call.
	 * @param args Arguments list for the function call.
	 */
// 	virtual void function(uint id, Signature func, Arguments args) = 0;
	virtual void function(Message msg) = 0;
	/**
	 *        This function is used for setting arbitrary properties on different protocols.
	 * @param property This is the name of the property to be changed
	 * @param value The new value of \a property
	 * @return Returns a ReturnValue containing information and/or errors related to the property change.
	 */
	virtual ReturnValue setProperty(QString property, QVariant value) = 0;
	/**
	 *        This function is used for getting the current value of \a property on the protocols.
	 * @param property The name of the property to get the data from
	 * @return The value of \a property or an error.
	 */
	virtual ReturnValue getProperty(QString property) = 0;
	/**
	 *        This function connects to the remote host. This function is called internally.
	 */
	virtual void protocolConnect() = 0;
	/**
	 *        This function disconnected from the remote host, and returns  information about the disconnect.
	 * @return Returns a ReturnValue containing information and/or errors related to the disconnect.
	 */
	virtual ReturnValue protocolDisconnect() = 0;
	quint32 version();
	void setVersion(quint32 version);
};

}

#endif
