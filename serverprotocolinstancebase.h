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
#ifndef QTRPC_SERVERPROTOCOLINSTANCEBASE_H
#define QTRPC_SERVERPROTOCOLINSTANCEBASE_H

#include <QObject>
#include <QxtPimpl>
#include <Signature>
#include <QHash>
#include <Message>
#include <QtRpcGlobal>

class QThread;

namespace QtRpc
{

class AuthToken;
class ReturnValue;
class Server;
class ServiceProxy;
class ServerProtocolInstanceBasePrivate;

/**
	This is the base class for all of the server side protocol instance objects. The instance objects are in charge of all server side communication with the client. A matching client side protocol object is required in nearly all cases for proper communication.

This class is an interface class with a few implemented convenience functions and therefore cannot be used directly. This class, nor any of it's children, should be used directly, as a listener object should be the only class to create the instance objects.

An instance object does not know what thread it will run it, but it will always be created in the same thread. Once the instance object is created it is then moved to it's destination thread and init() is called inside of that thread. Beacuse of this, it is important not to initialize any sockets or anything in the constructor, but instead to do so in the init() function.

	@sa Server ServerProtocolListenerBase ClientProtocolBase
	@brief The base class for server side instance objects
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ServerProtocolInstanceBase : public QObject
{
	QXT_DECLARE_PRIVATE(ServerProtocolInstanceBase);
	Q_OBJECT
public:
	struct ReplySlot
	{
		QObject* object;
		Signature slot;
	};
	ServerProtocolInstanceBase(Server* serv, QObject *parent = 0);
	~ServerProtocolInstanceBase();
	/**
	 *        This function is used for setting arbitrary properties on protocols. The properties vary from protocol to protocol.
	 * @sa getProperty
	 * @param property The name of the property to set
	 * @param value The new value of \a property
	 */
	virtual void setProperty(QString property, QVariant value) = 0;
	/**
	 *        This function is used for getting arbitrary properties on protocols. The properties vary from protocol to protocol.
	 * @sa setProperty
	 * @param property The name of the property to get
	 * @return The value of \a property
	 */
	virtual QVariant getProperty(QString propert) = 0;
	/**
	 *        This function is called by the service object to send events.
	 * @param func The Signature of the event
	 * @param args Arguments list for the event
	 */
	virtual void sendEvent(quint32 id, Signature func, Arguments args) = 0;
	void moveToThread(QThread*);
	AuthToken defaultToken();

public slots:
	/**
	 *        This function is called to disconnect the server from the client. In this slot, the instance object should also be destroyed in most cases, as the instance object is no longer needed once disconnected.
	 */
	virtual void disconnect() = 0;
	/**
	 *        This is a very important function. This function is called on a newly created instance object in the thread that it will lie in. Instance objects are not always, in fact more often not, created in the same thread that they will execute in. Instance objects are moved to a thread and then this function is run in the new thread. This function should be used in place of the constructor for almost everything.
	 */
	virtual void init() = 0;
	/**
	 *        This function is called by the service object to send a callback function. The instance object transmits the callback function, and when a reply is received it sends the reply to \a slot on \a obj . The slot receiving the reply must take a uint and a ReturnValue as it's parameters, else it will not receive the reply and it will silently fail.
	 * @param obj The QObject* that will be receiving the reply
	 * @param slot The Signature of the slot that will be receiving the ReturnValue
	 * @param func The Signature of the callback function
	 * @param args Arguments list for the callback function
	 * @return The id number of the callback function that will be passed to \a slot
	 */
	virtual uint callCallback(QObject* obj, Signature slot, quint32 id, Signature func, Arguments args) = 0;
	quint32 serviceId(ServiceProxy* service) const;
	quint32 currentFunctionId() const;
	virtual void writeMessage(Message) = 0;

signals:
	void aboutToChangeThreads(QThread*);

protected:
	ReturnValue listServices();
	ReturnValue listFunctions(const QString &service);
	ReturnValue listCallbacks(const QString &service);
	ReturnValue listEvents(const QString &service);
	ReturnValue getServiceObject(QString, QString, QString);
	ReturnValue getServiceObject(QString service);
	ServiceProxy* service() const;
	ServiceProxy* service(quint32 id) const;
	ReturnValue callFunction(quint32 id, quint32 serviceid, Signature sig, Arguments args);
	ReturnValue callFunction(const Message &msg);
	QHash<uint, ReplySlot>& queue();
	uint nextId();
};

}

#endif
