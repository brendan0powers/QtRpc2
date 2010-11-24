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
#ifndef CLIENTPROXY_P_H
#define CLIENTPROXY_P_H

#include <QxtPimpl>
#include <ClientMessageBus>
#include <QMutex>
#include <QReadWriteLock>
#include <QHash>
#include "clientproxy.h"
#include <qtrpcprivate.h>

#include <QSharedPointer>
#include <AuthToken>

/**
	@author Chris Vickery <chris@resara.com>
 */

/*
The new ClientProxy system works via sharing 2 important classes...

The first of which is the ConnectionData, which contains the pointer to the message bus
and handles all of the connection related stuff. It also has a series of weak pointers to all
the service data objects, but itself doesn't actually care about the current status of service
connectedness... This class cares about the connection state, the auth token, and handling
the calling of protocol functions...

The second is the ServiceData, which is basically exactly what it sounds like. This class has
a strong pointer to the ConnectionData it uses, which guarentees that when both are scheduled
to be cleaned up at the same time, the connectiondata is always cleaned up second (because the
last reference to it will be removed when the service data object is removed)... It routes all the
incoming messages to the proper client proxies and handles all that shiznit...
*/

namespace QtRpc
{
class ServiceData;

class ConnectionData : public QObject, public QSharedData
{
	Q_OBJECT
public:
	~ConnectionData();
	// All service data objects must register themselves with the connection
	void registerServiceData(quint32 id, QWeakPointer<ServiceData> srv);
	// and then must unregister themselves when they're going away
	void unregisterServiceData(quint32 id);

	// Function calling
	ReturnValue callFunction(Signature sig, Arguments args); //out
	ReturnValue callFunction(QObject* obj, Signature slot, Signature sig, Arguments args); //out

	QReadWriteLock mutex;
	ClientProxy::State state;
	QPointer<ClientMessageBus> bus;
	QHash<quint32, QWeakPointer<ServiceData> > serviceDataObjects;
	AuthToken token;
public slots:
	void sendEvent(Message msg); //in
	void sendCallback(Message msg); //in

signals:
	void disconnected(); //in
};

class ServiceData : public QObject, public QSharedData
{
	Q_OBJECT
public:
	// Has to be constructed with the ID of the service object on the server side it goes with, and a connection object to use
	ServiceData(quint32 _id, QSharedPointer<ConnectionData> _connection);
	~ServiceData();
	// Whenever a new client starts sharing this service object it must register itself to work properly
	void addProxy(ClientProxy* ptr);
	// and then unregister when it's done
	void removeProxy(ClientProxy* ptr);
	
	// function calling
	ReturnValue callFunction(Signature sig, Arguments args); //out
	ReturnValue callFunction(QObject* obj, Signature slot, Signature sig, Arguments args); //out

	// service ID from the server side
	quint32 id;
	QReadWriteLock mutex;
	//This is a shared pointer and not a weak pointer so that connection always gets cleaned up last, because we need to send functions in the destructor... When the last client proxy goes away, the last servicedata will also go away (or have already gone) so connection *will* get cleaned up
	QSharedPointer<ConnectionData> connection;
	
	// the "primary" cleint proxy is the one we call callbacks on, but events go to all client proxies
	QPointer<ClientProxy> primary;
	QList<ClientProxy*> list;
public slots:
	void sendEvent(Signature, Arguments); //in
	void sendCallback(uint, Signature, Arguments); //in

signals:
	void callbackReturn(uint, ReturnValue); //out
};

class ClientProxyPrivate : public QObject, public QxtPrivate<ClientProxy>
{
	Q_OBJECT
public:
	ClientProxyPrivate();

	struct ObjectSlot
	{
		QString slot;
		QObject* object;
	};

	QSharedPointer<ConnectionData> connection;
	QSharedPointer<ServiceData> service;
	// asyncronous calls get their return places put here, the key is the id of the function call
	QHash<uint, ObjectSlot> connectObjects;
	QHash<uint, ObjectSlot> functionObjects;
	bool initialized;
	// we have the mutex all returning asyncronous calls, because they're emitted using a signal...
	QMutex signalerMutex;
	QMutex signalerMutexFunction;
	// weather it's the primary client proxy for the service it's connected to currently
	bool isPrimary();
	// all return values need to go through this to get proper values and stuff set, specifically so that we put the service object in there...
	ReturnValue parseReturn(ReturnValue ret);
	QSharedPointer<ServiceData> getServiceData(const ReturnValue &ret);

public slots:
	void connectCompleted(uint id, ReturnValue ret); //in
	void functionCompleted(uint id, ReturnValue ret); //in
	void receiveCallback(uint id, Signature sig, Arguments args); //in
	void receiveEvent(Signature sig, Arguments args); //in
	void disconnectedSlot(); //stupid name, i know...

};
}
#endif
