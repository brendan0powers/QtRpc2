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
#include "clientproxy.h"
#include "clientproxy_p.h"
#include "returnvalue_p.h"
#include "authtoken.h"

#include <QDebug>
#include <QStringList>
#include <QThread>

using namespace QtRpc;

ServiceData::ServiceData(quint32 _id, QSharedPointer<ConnectionData> _connection) :
		id(_id),
		connection(_connection),
		primary(0)
{
}

ServiceData::~ServiceData()
{
	QWriteLocker locker(&connection->mutex);
	if (connection->state != ClientProxy::Disconnected && !connection->bus.isNull())
	{
		connection->callFunction(NULL, Signature(), Signature("destroyService(quint32)"), Arguments() << id);
	}
	connection->unregisterServiceData(id);
}

void ServiceData::addProxy(ClientProxy* ptr)
{
	QWriteLocker locker(&mutex);
	// make sure we always have a primary
	if (primary.isNull())
		primary = ptr;
	list << ptr;
}

void ServiceData::removeProxy(ClientProxy* ptr)
{
	QWriteLocker locker(&mutex);
	list.removeAll(ptr);
	if (primary == ptr)
	{
		// make sure we always have a primary
		if (list.count() > 0)
			primary = list.first();
		else
			primary = 0; // No more clients!
	}
}

void ServiceData::sendEvent(Signature sig, Arguments args)
{
	QReadLocker locker(&mutex);
	foreach(ClientProxy* service, list)
	{
		service->qxt_d().receiveEvent(sig, args);
	}
}

void ServiceData::sendCallback(uint id, Signature sig, Arguments args)
{
	QWriteLocker locker(&mutex);
	if (primary.isNull())
	{
		// make sure we always have a primary
		if (list.count() > 0)
			primary = list.first();
		else
		{
			// there should never be a servicedata object with no proxies to go with it, if there is then that means that the shared pointer didn't delete this class when the last proxy went away.... problem!
			qCritical() << "ServiceData has no proxies but still exists, this is an error";
			return;
		}
	}
	primary->qxt_d().receiveCallback(id, sig, args);
}

ReturnValue ServiceData::callFunction(Signature sig, Arguments args)
{
	if (!connection || !connection->bus)
		return ReturnValue(1, "Cannot call functions while not connected");
	return connection->bus->callFunction(Message(0, Message::Function, sig, args, id));
}

ReturnValue ServiceData::callFunction(QObject* obj, Signature slot, Signature sig, Arguments args)
{
	if (!connection || !connection->bus)
		return ReturnValue(1, "Cannot call functions while not connected");
	return connection->bus->callFunction(obj, slot, Message(0, Message::Function, sig, args, id));
}

ConnectionData::~ConnectionData()
{
	// at this point the last servicedata object was deleted, so we can now make the message bus go away, which effectively (via signals and slots and shit) also deletes the qtrpc2 communications internals... clientprotocolthread does all those connections, check both the header and the cpp for details on how that works
	// Don't lock because when this is destroyed, there is only 1 reference anyway
	if (!bus.isNull())
		bus->deleteLater();
}

void ConnectionData::sendEvent(Message msg)
{
	QReadLocker locker(&mutex);
	if (serviceDataObjects.count() < 1)
		return;
	if (msg.version() == 0)
	{
		// no sharing services in version 0, so just send it to the first one we find :P
		serviceDataObjects.begin().value().data()->sendEvent(msg.signature(), msg.arguments());
		return;
	}
	// route it to the proper servicedata object and let it route it to the proper client proxies...
	if (!serviceDataObjects.contains(msg.service()))
		return;
	if (serviceDataObjects.value(msg.service()).isNull())
		return;
	serviceDataObjects.value(msg.service()).data()->sendEvent(msg.signature(), msg.arguments());
}

void ConnectionData::sendCallback(Message msg)
{
	QReadLocker locker(&mutex);
	if (serviceDataObjects.count() < 1)
		return;
	if (msg.version() == 0)
	{
		// no sharing services in version 0, so just send it to the first one we find :P
		serviceDataObjects.begin().value().data()->sendCallback(msg.id(), msg.signature(), msg.arguments());
		return;
	}
	// route it to the proper servicedata object and let it route it to the proper client proxies...
	if (!serviceDataObjects.contains(msg.service()))
		return;
	if (serviceDataObjects.value(msg.service()).isNull())
		return;
	serviceDataObjects.value(msg.service()).data()->sendCallback(msg.id(), msg.signature(), msg.arguments());
}

void ConnectionData::registerServiceData(quint32 id, QWeakPointer<ServiceData> srv)
{
	QWriteLocker locker(&mutex);
	serviceDataObjects[id] = srv;
}

void ConnectionData::unregisterServiceData(quint32 id)
{
	QWriteLocker locker(&mutex);
	serviceDataObjects.remove(id);
}

// outgoing functions...
ReturnValue ConnectionData::callFunction(Signature sig, Arguments args)
{
	if (!bus)
		return ReturnValue(1, "Cannot call functions while not connected");
	return bus->callFunction(Message(0, Message::QtRpc, sig, args));
}

ReturnValue ConnectionData::callFunction(QObject* obj, Signature slot, Signature sig, Arguments args)
{
	if (!bus)
		return ReturnValue(1, "Cannot call functions while not connected");
	return bus->callFunction(obj, slot, Message(0, Message::QtRpc, sig, args));
}

ClientProxyPrivate::ClientProxyPrivate() :
		connection(new ConnectionData()) //we should ALWAYS have a connection object, no matter what
{
}

/**
 * Constructor
 * @param parent The parent object, can be NULL
 */
ClientProxy::ClientProxy(QObject *parent)
		: ProxyBase(parent)
{
	QXT_INIT_PRIVATE(ClientProxy);
	//Register Meta Types
	registerMetaTypes();

	// connection exists because of the default constructor of ClientProxyPrivate
	QObject::connect(qxt_d().connection.data(), SIGNAL(disconnected()), &qxt_d(), SLOT(disconnectedSlot()), Qt::QueuedConnection);
	QObject::connect(qxt_d().connection.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::QueuedConnection);

	qxt_d().connection->state = Disconnected;
	qxt_d().connection->bus = NULL;
	qxt_d().initialized = false;
}

ClientProxy::ClientProxy(const ClientProxy& cp, QObject *parent) : ProxyBase(parent)
{
	QXT_INIT_PRIVATE(ClientProxy);
	//Register Meta Types
	registerMetaTypes();
	qxt_d().connection = cp.qxt_d().connection;
	qxt_d().service = cp.qxt_d().service;
	if (!qxt_d().service.isNull())
		qxt_d().service->addProxy(this);
	qxt_d().initialized = false;

	QObject::connect(qxt_d().connection.data(), SIGNAL(disconnected()), &qxt_d(), SLOT(disconnectedSlot()), Qt::QueuedConnection);
	QObject::connect(qxt_d().connection.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::QueuedConnection);
}

/**
 * Destructor, cleans up and closes any open connections
 */
ClientProxy::~ClientProxy()
{
	// We use shared pointers to remove everything. Win!
}

ClientProxy::State ClientProxy::state()
{
	return qxt_d().connection->state;
}

/**
 * Connect to the server.
 * @param url The url to connct to. See class description for examples
 * @return Returns a valid ReturnValue on success, Or on error on failure.
 */
ReturnValue QtRpc::ClientProxy::connect(QString url)
{
	return(connect(QUrl::fromEncoded(qPrintable(url))));
}

ReturnValue ClientProxy::connect(QString url, const AuthToken &defaultToken)
{
	return connect(url, NULL, "", defaultToken);
}

/**
 * Asynchronous connect function. Does the same this as connect(QString), but return the result asynchronously.
 * @param obj The object for the asynchronous return.
 * @param slot The slot to be called when connect finishes
 * @param url Returns a valid ReturnValue on success, Or on error on failure.
 * @return Returns the ID of the asynchronous call
 */
ReturnValue QtRpc::ClientProxy::connect(QObject *obj, const char *slot, QString url)
{
	return(connect(QUrl(url), obj, slot));
}


/**
 * Don't use the function directly, use connect(QString) or connect(QObject *, const char*, QString)
 */
ReturnValue QtRpc::ClientProxy::connect(QUrl url, QObject *obj, const char *slot)
{
	AuthToken token;
	if (!url.userName().isEmpty())
		token.clientData()["username"] = url.userName();
	if (!url.password().isEmpty())
		token.clientData()["password"] = url.password();
	return connect(url, obj, slot, token);
}

ReturnValue QtRpc::ClientProxy::connect(QUrl url, QObject *obj, const char *slot, const AuthToken &defaultToken)
{
	// when opening a new connection, always break off and make a new connection object....
	qxt_d().connection = QSharedPointer<ConnectionData>(new ConnectionData());
	qxt_d().connection->state = Disconnected;
	qxt_d().connection->bus = NULL;
	QObject::connect(qxt_d().connection.data(), SIGNAL(disconnected()), &qxt_d(), SLOT(disconnectedSlot()), Qt::QueuedConnection);
	QObject::connect(qxt_d().connection.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::QueuedConnection);

	// we set this connections default token to the one used at connect time
	qxt_d().connection->token = defaultToken;
	qxt_d().connection->state = Connecting;
	//Initialize the proxy object
	if (!qxt_d().initialized) init();
	//check to see if were connected, and if so disconnect
	if (!qxt_d().connection->bus.isNull()) disconnect();

	if (url.scheme() == "")
	{
		qxt_d().connection->state = Disconnected;
		return(ReturnValue(1, "You must specify a protocol in the url."));
	}
	// we put the service into the authtoken... it's used later...
	if (url.scheme() == "socket")
	{
#ifdef Q_OS_WIN32
		return ReturnValue(1, "Sockets are not supported on windows");
#else
		// sockets have weird paths
		if (url.path().contains(':'))
		{
			qxt_d().connection->token.clientData()["service"] = url.path().mid(url.path().indexOf(':') + 1);
			url.setPath(url.path().left(url.path().indexOf(':')));
		}
#endif
	}
	else
	{
		// > 1 because it includes the server:1000"/Service", so even if there's no service the length could be 1 for the /
		if (url.path().count() > 1)
			qxt_d().connection->token.clientData()["service"] = url.path().mid(1); //cut out the /
		url.setPath(QString());
	}

	//Get the message bus for the intended protocol
	qxt_d().connection->bus = ClientMessageBus::instance(url.scheme());
	if (qxt_d().connection->bus.isNull())
	{
		qxt_d().connection->state = Disconnected;
		return(ReturnValue(2, "Protocol not found."));
	}

	QObject::connect(qxt_d().connection->bus, SIGNAL(sendEvent(Message)), qxt_d().connection.data(), SLOT(sendEvent(Message)), Qt::QueuedConnection);
	QObject::connect(qxt_d().connection->bus, SIGNAL(sendCallback(Message)) , qxt_d().connection.data(), SLOT(sendCallback(Message)) , Qt::QueuedConnection);
	QObject::connect(qxt_d().connection->bus, SIGNAL(disconnected()), qxt_d().connection.data(), SIGNAL(disconnected()), Qt::QueuedConnection);
// 	QObject::connect(qxt_d().data.data(), SIGNAL(callbackReturn(uint, ReturnValue)), qxt_d().data->bus, SIGNAL(callbackReturn(uint, ReturnValue)), Qt::QueuedConnection);

	Arguments args;
	args << url;
	Signature sig("connect(QUrl)"); //set service to 0
	if (obj == NULL)
	{
		ReturnValue ret = qxt_d().connection->callFunction(sig, args);
		if (ret.isError())
		{
			qxt_d().connection->state = Disconnected;
			return ret;
		}
		qxt_d().connection->state = Connected;

		// at this point the version negotiations are completed
		ret = qxt_d().connection->callFunction(Signature("version()"), Arguments());
		if (ret.isError())
		{
			qCritical() << "Failed to get version:" << ret;
			return ret;
		}
		if (ret.toUInt() > 0)
		{
			ret = qxt_d().connection->callFunction("setDefaultToken(AuthToken)", Arguments() << QVariant::fromValue(qxt_d().connection->token));
			if (ret.isError())
			{
				qCritical() << "Failed to set default AuthToken:" << ret;
				return ret;
			}
		}

		// we filled this in earlier, now we're using it!
		if (qxt_d().connection->token.clientContains("service"))
		{
			QString service = qxt_d().connection->token.clientData()["service"].toString();
			qxt_d().connection->token.clientRemove("service");
			return selectService(service);
		}
		else
			return ret;
	}
	else
	{
		qxt_d().connection->token.clientData()["setDefaultToken"] = false;
		ReturnValue ret = qxt_d().connection->callFunction(&qxt_d(), Signature("connectCompleted(uint, ReturnValue)"), sig, args);
		if (ret.isError())
			qxt_d().connection->state = Disconnected; //failed to make an async call
		else
		{
			ClientProxyPrivate::ObjectSlot objectslot = {slot, obj, ret.toUInt()};
			qxt_d().connectObjects[ret.toUInt()] = objectslot;
		}
		return(ret);
	}
}

void QtRpc::ClientProxyPrivate::connectCompleted(uint id, ReturnValue ret)
{
	if (ret.isError())
	{
		connection->token.clientData().remove("setDefaultToken");
		connection->token.clientData().remove("service");
		connection->state = ClientProxy::Disconnected;
		ObjectSlot obj = connectObjects.take(id);
		sendReturnValue(obj, ret);
		return;
	}
	if (!connection)
	{
		connection->token.clientData().remove("setDefaultToken");
		connection->token.clientData().remove("service");
		connection->state = ClientProxy::Disconnected;
		ret = ReturnValue(1, "Connection unexpectedly dropped during connect.");
		ObjectSlot obj = connectObjects.take(id);
		sendReturnValue(obj, ret);
		return;
	}
	if (connection->token.clientContains("setDefaultToken"))
	{
		connection->token.clientData().remove("setDefaultToken");
		ret = connection->callFunction(
		          this,
		          Signature("connectCompleted(uint, ReturnValue)"),
		          Signature("setDefaultToken(AuthToken)"),
		          Arguments() << QVariant::fromValue(connection->token));
		if (ret.isError())
		{
			connection->token.clientData().remove("setDefaultToken");
			connection->token.clientData().remove("service");
			connection->state = ClientProxy::Disconnected;
			ObjectSlot obj = connectObjects.take(id);
			sendReturnValue(obj, ret);
		}
		else
			connectObjects.insert(ret.toUInt(), connectObjects.take(id));
		return;
	}
	if (connection->token.clientContains("service"))
	{
		QString service = connection->token.clientData().take("service").toString();
		ret = qxt_p().selectService(
		          this,
		          SLOT(connectCompleted(uint, ReturnValue)),
		          service);
		if (ret.isError())
		{
			connection->token.clientData().remove("setDefaultToken");
			connection->token.clientData().remove("service");
			connection->state = ClientProxy::Disconnected;
			ObjectSlot obj = connectObjects.take(id);
			sendReturnValue(obj, ret);
		}
		else
			connectObjects.insert(ret.toUInt(), connectObjects.take(id));
		return;
	}
	connection->state = ClientProxy::Connected;
	ObjectSlot obj = connectObjects.take(id);
	sendReturnValue(obj, ret);
}

void QtRpc::ClientProxyPrivate::sendReturnValue(const ObjectSlot &obj, const ReturnValue &ret)
{
	signalerMutex.lock();
	if (!obj.object || obj.slot.isEmpty())
		return;
	if (obj.slot.contains("QtRpc::ReturnValue"))
	{
		QObject::connect(&(qxt_p()), SIGNAL(asyncronousSignalerNamespace(uint, QtRpc::ReturnValue)), obj.object, qPrintable(obj.slot), Qt::QueuedConnection);
		emit qxt_p().asyncronousSignalerNamespace(obj.id, ret);
		QObject::disconnect(&(qxt_p()), SIGNAL(asyncronousSignalerNamespace(uint, QtRpc::ReturnValue)), obj.object, qPrintable(obj.slot));
	}
	else
	{
		QObject::connect(&(qxt_p()), SIGNAL(asyncronousSignaler(uint, ReturnValue)), obj.object, qPrintable(obj.slot), Qt::QueuedConnection);
		emit qxt_p().asyncronousSignaler(obj.id, ret);
		QObject::disconnect(&(qxt_p()), SIGNAL(asyncronousSignaler(uint, ReturnValue)), obj.object, qPrintable(obj.slot));
	}
	signalerMutex.unlock();
}

// overloaded
ReturnValue ClientProxy::selectService(QString service)
{
	return selectService(service, AuthToken::defaultToken());
}

ReturnValue ClientProxy::selectService(QObject *obj, const char *slot, const QString &service)
{
	return selectService(obj, slot, service, AuthToken::defaultToken());
}

// selects a service, assigns it to this client, and then returns the service
ReturnValue ClientProxy::selectService(QString service, AuthToken token)
{
	ReturnValue ret = getService(service, token);
	if (!ret.isError())
		operator=(ret);
	return ret;
}

ReturnValue ClientProxy::selectService(QObject *o, const char *slot, const QString &service, const AuthToken &token)
{
	ReturnValue ret = getService(
	                      &qxt_d(),
	                      SLOT(selectServiceCompleted(uint, ReturnValue)),
	                      service,
	                      token);
	if (ret.isError())
		return ret;
	ClientProxyPrivate::ObjectSlot obj = {slot, o, ret.toUInt()};
	qxt_d().selectServiceObjects.insert(ret.toUInt(), obj);
	return ret;
}

void ClientProxyPrivate::selectServiceCompleted(uint id, ReturnValue ret)
{
	if (!ret.isError())
		qxt_p() = (ret);
	ObjectSlot obj = selectServiceObjects.take(id);
	sendReturnValue(obj, ret);
}

// overloaded
ReturnValue ClientProxy::getService(QString service)
{
	return getService(service, AuthToken::defaultToken());
}
ReturnValue ClientProxy::getService(QObject *obj, const char *slot, const QString &service)
{
	return getService(obj, slot, service, AuthToken::defaultToken());
}

// selects a service and returns it
ReturnValue ClientProxy::getService(QObject *o, const char *slot, const QString &service, const AuthToken &token)
{
	if (!qxt_d().connection)
		return ReturnValue(1, "Not connected");
	QVariantMap serviceStatus;
	serviceStatus["step"] = 0;
	serviceStatus["token"] = QVariant::fromValue(token);
	serviceStatus["service"] = service;
	ReturnValue ret = qxt_d().connection->callFunction(
	                      &qxt_d(),
	                      Signature("getServiceCompleted(uint, ReturnValue)"),
	                      Signature("version()"),
	                      Arguments());
	if (!ret.isError())
	{
		qxt_d().getServiceStatus.insert(ret.toUInt(), serviceStatus);
		ClientProxyPrivate::ObjectSlot obj = {slot, o, ret.toUInt()};
		qxt_d().getServiceObjects.insert(ret.toUInt(), obj);
	}
	return ret;
}

void ClientProxyPrivate::getServiceCompleted(uint id, ReturnValue ret)
{
	if (ret.isError())
	{
		ObjectSlot obj = getServiceObjects.take(id);
		getServiceStatus.remove(obj.id);
		sendReturnValue(obj, ret);
		return;
	}
	if (!connection)
	{
		connection->state = ClientProxy::Disconnected;
		ret = ReturnValue(1, "Connection unexpectedly dropped during connect.");
		ObjectSlot obj = getServiceObjects.take(id);
		getServiceStatus.remove(obj.id);
		sendReturnValue(obj, ret);
		return;
	}
	uint functionId = getServiceObjects.value(id).id;
	QVariantMap serviceStatus = getServiceStatus.value(functionId);
	QString service = serviceStatus["service"].toString();
	AuthToken token = serviceStatus["token"].value<AuthToken>();
	switch (serviceStatus.value("step").toUInt())
	{
		case 0:
		{
			ret = connection->callFunction(
			          this,
			          Signature("getServiceCompleted(uint, ReturnValue)"),
			          Signature("version()"),
			          Arguments());
			break;
		}
		case 1:
		{
			// return from version
			serviceStatus["version"] = ret.toInt();
			if (serviceStatus["version"] == 0)
			{
				if (token.isDefault())
					token = connection->token;
				// old style service selecting
				ret = connection->callFunction(
				          this,
				          Signature("getServiceCompleted(uint, ReturnValue)"),
				          Signature("selectService(QString, QString, QString)"),
				          Arguments() << service << token.clientData()["username"] << token.clientData()["password"]);
			}
			else
			{
				// select service the new way
				ret = connection->callFunction(
				          this,
				          Signature("getServiceCompleted(uint, ReturnValue)"),
				          Signature("selectService(QString)"),
				          Arguments() << service);
			}
			break;
		}
		case 2:
		{
			// return from selectService
			if (serviceStatus["version"] == 0)
			{
				// old style service selecting
				ret = parseReturn(ret);
				if (!ret.isError())
				{
					QSharedPointer<ServiceData> data(new ServiceData(0, connection));
					connection->registerServiceData(0, data);
					if (!this->service.isNull())
						this->service->removeProxy(&qxt_p());
					this->service = data;
					if (!this->service.isNull())
						this->service->addProxy(&qxt_p());
				}
				if (token.isDefault())
					token = connection->token;
				token.clientData()["auth_return"] = ret;
				ObjectSlot obj = getServiceObjects.take(id);
				getServiceStatus.remove(obj.id);
				sendReturnValue(obj, ret);
			}
			else
			{
				// select service the new way
				ret = parseReturn(ret);
				if (!ret.isService())
				{
					ObjectSlot obj = getServiceObjects.take(id);
					getServiceStatus.remove(obj.id);
					sendReturnValue(obj, ReturnValue(1, "Failed to get service object from server"));
				}
				QSharedPointer<ServiceData> data = getServiceData(ret);
				if (data.isNull())
				{
					ObjectSlot obj = getServiceObjects.take(id);
					getServiceStatus.remove(obj.id);
					sendReturnValue(obj, ReturnValue(ReturnValue::GenericError, "Failed to fetch the service data from the ReturnValue!"));
				}
				serviceStatus["service_return"] = QVariant::fromValue(ret);
				ret = data->callFunction(
				          this,
				          Signature("getServiceCompleted(uint, ReturnValue)"),
				          Signature("auth(QtRpc::AuthToken)"),
				          Arguments() << QVariant::fromValue(token));
			}
			break;
		}
		case 3:
		{
			// return from auth
			ret = parseReturn(ret);
			if (ret.isError())
			{
				ObjectSlot obj = getServiceObjects.take(id);
				getServiceStatus.remove(obj.id);
				sendReturnValue(obj, ret);
			}
			if (token.isDefault())
				token = connection->token;
			token.clientData()["auth_return"] = ret;

			ObjectSlot obj = getServiceObjects.take(id);
			getServiceStatus.remove(obj.id);
			sendReturnValue(obj, serviceStatus["service_return"].value<ReturnValue>());
			break;
		}
		default:
			ObjectSlot obj = getServiceObjects.take(id);
			getServiceStatus.remove(obj.id);
			sendReturnValue(obj, ReturnValue(1, "Reached an known step in the service selecting process!"));
			return;
			break;
	}
	if (!getServiceStatus.contains(functionId)) // we're all done!
		return;

	if (ret.isError())
	{
		ObjectSlot obj = getServiceObjects.take(id);
		getServiceStatus.remove(obj.id);
		sendReturnValue(obj, ret);
	}
	else
		getServiceObjects.insert(ret.toUInt(), getServiceObjects.take(id));
	serviceStatus["step"] = serviceStatus.value("step").toUInt() + 1;
	getServiceStatus.insert(functionId, serviceStatus);
}

ReturnValue ClientProxy::getService(QString service, AuthToken token)
{
	if (!qxt_d().connection)
		return ReturnValue(1, "Not connected");
	ReturnValue ret = qxt_d().connection->callFunction(Signature("version()"), Arguments());
	if (ret.isError())
	{
		qCritical() << "Failed to get version:" << ret;
		return ret;
	}
	if (ret.toUInt() == 0)
	{
		// select service the old way
		if (token.isDefault())
			token = qxt_d().connection->token;
		ret = qxt_d().parseReturn(qxt_d().connection->callFunction(Signature("selectService(QString, QString, QString)"), Arguments() << service << token.clientData()["username"] << token.clientData()["password"]));
		if (!ret.isError())
		{
			QSharedPointer<ServiceData> data(new ServiceData(0, qxt_d().connection));
			qxt_d().connection->registerServiceData(0, data);
			if (!qxt_d().service.isNull())
				qxt_d().service->removeProxy(this);
			qxt_d().service = data;
			if (!qxt_d().service.isNull())
				qxt_d().service->addProxy(this);
		}
		qxt_d().connection->token.clientData()["auth_return"] = ret;
		return ret;
	}
	else
	{
		// select service the new way
		ret = qxt_d().parseReturn(qxt_d().connection->callFunction(Signature("selectService(QString)"), Arguments() << service));
		if (!ret.isService())
			return ReturnValue(1, "Failed to get service object from server");
		QSharedPointer<ServiceData> data = qxt_d().getServiceData(ret);
		if (data.isNull())
			return ReturnValue(ReturnValue::GenericError, "Failed to fetch the service data from the ReturnValue!");
		ReturnValue ret2 = qxt_d().parseReturn(data->callFunction(Signature("auth(QtRpc::AuthToken)"), Arguments() << QVariant::fromValue(token)));
		if (ret2.isError())
			return ret2;
		QWriteLocker locker(&qxt_d().connection->mutex);
		qxt_d().connection->token.clientData()["auth_return"] = ret2;
		return ret;
	}
}

// unlink this client from it's service....
ReturnValue ClientProxy::deselectService()
{
	if (!qxt_d().service.isNull())
	{
		qxt_d().service->removeProxy(this);
	}
	qxt_d().service = QSharedPointer<ServiceData>();
	return true;
}

/**
 * Internal function inherited from ProxyBase to handle Synchronous function calls
 */
ReturnValue ClientProxy::functionCalled(const Signature& sig, const Arguments& args, const QString&)
{
	if (qxt_d().connection->bus.isNull())
		return(ReturnValue(1, "Not Connected"));

	if (qxt_d().service.isNull())
		return(ReturnValue(1, "No service selected"));
	//make the call and return the result
	return qxt_d().parseReturn(qxt_d().service->callFunction(sig, args));
}

/**
 * Internal function inherited from ProxyBase to handle Asynchronous function calls
 */
ReturnValue QtRpc::ClientProxy::functionCalled(QObject *obj, const char *slot, const Signature& sig, const Arguments& args, const QString&)
{
	if (qxt_d().connection->bus.isNull())
		return(ReturnValue(1, "Not Connected"));


	//strip the argument list from the slot
	QString tmpslot = QString(slot).section("(", 0, 0);
	//remove the 1 from the begining of the function if SLOT() was used
	if (tmpslot.startsWith("1"))
		tmpslot = tmpslot.remove(0, 1);
	Signature s(tmpslot + "()"); //create the signature object from the slot


	//make the call and return the result
// 	ReturnValue ret = _bus->callFunction(obj, s, sig, args);

	ReturnValue ret;
	if (qxt_d().service.isNull())
		return(ReturnValue(1, "No service selected"));
	//make the call and return the result
	ret = qxt_d().service->callFunction(&qxt_d(), Signature("functionCompleted(uint, ReturnValue)"), sig, args);
	if (!ret.isError())
	{
		ClientProxyPrivate::ObjectSlot objectslot = {slot, obj};
		qxt_d().functionObjects[ret.toUInt()] = objectslot;
	}

	return(ret);
}

// async reply to function calls, it connects the signal/slot, emits, and disconnects...
void QtRpc::ClientProxyPrivate::functionCompleted(uint id, ReturnValue ret)
{
	ret = parseReturn(ret);
	ObjectSlot obj = functionObjects.take(id);
	obj.id = id;
	sendReturnValue(obj, ret);
}

/**
 * Protected function for initialzing the ProxyBase object. You never need to call this function directly
 */
void QtRpc::ClientProxy::init()
{
	QStringList funclist;
	QStringList eventlist;
	QStringList calllist;

	//Define what types we allow
	funclist << "ReturnValue" << "QtRpc::ReturnValue";
	eventlist << "Event" << "QtRpc::Event";
	calllist << "ReturnValue" << "QtRpc::ReturnValue";

	//Initialize proxy object so the varisous functions work
	ProxyBase::init(funclist, calllist, eventlist);

	qxt_d().initialized = true;
}

/**
 * Internal function called by the message bus when a callback has been called
 * @param id Id to use when returning the result
 * @param sig The signature of the callback
 * @param args The arguments for the callback
 */
void QtRpc::ClientProxyPrivate::receiveCallback(uint id, Signature sig, Arguments args)
{
	if (connection->bus.isNull()) return;

	ReturnValue ret = qxt_p().callCallback(sig, args);
	emit qxt_p().returnCallback(id, ret);
}

/**
 * Internal function called by the message bus when a event is recieved
 * @param sig The signature of the event
 * @param args The arguments for the event
 */
void QtRpc::ClientProxyPrivate::receiveEvent(Signature sig, Arguments args)
{
	if (connection->bus.isNull())
	{
		qCritical() << "Event failed because the bus was null!";
		return;
	}

	ReturnValue ret = qxt_p().emitSignal(sig, args);
	if (ret.isError())
	{
		qFatal(qPrintable(ret.errString()));
	}
}

/**
 * disconnects from server
 */
void QtRpc::ClientProxy::disconnect()
{
	{
		QReadLocker locker(&qxt_d().connection->mutex);
		if (qxt_d().connection->bus.isNull())
			return;
		if (qxt_d().connection->state == Disconnected)
			return;
	}
	QWriteLocker locker(&qxt_d().connection->mutex);
	Arguments args;
	Signature sig("disconnect()");
	qxt_d().connection->state = Disconnected;

	qxt_d().connection->callFunction(NULL, NULL, sig, args);

	qxt_d().connection->bus->deleteLater();
	qxt_d().connection->bus = 0;
}

ReturnValue ClientProxy::listServices()
{
	return qxt_d().connection->callFunction(Signature("listServices()"), Arguments());
}

ReturnValue ClientProxy::listFunctions(const QString &service)
{
	return qxt_d().connection->callFunction(Signature("listFunctions(QString)"), Arguments() << service);
}

ReturnValue ClientProxy::listCallbacks(const QString &service)
{
	return qxt_d().connection->callFunction(Signature("listCallbacks(QString)"), Arguments() << service);
}

ReturnValue ClientProxy::listEvents(const QString &service)
{
	return qxt_d().connection->callFunction(Signature("listEvents(QString)"), Arguments() << service);
}

void QtRpc::ClientProxyPrivate::disconnectedSlot()
{
	{
		QReadLocker locker(&connection->mutex);
		if (connection->bus.isNull())
			return;
		if (connection->state == ClientProxy::Disconnected)
			return;
	}
	QWriteLocker locker(&connection->mutex);
	connection->state = ClientProxy::Disconnected;

	connection->bus->deleteLater();
	connection->bus = 0;
}

// handles putting the servicedata into the returnvalue so that clients can be assigned to it
ReturnValue ClientProxyPrivate::parseReturn(ReturnValue ret)
{
	QReadLocker locker(&connection->mutex);
	ReturnValueData* rtData = const_cast<ReturnValueData*>(ret.qxt_d().data.constData());
	if (ret.isService())
	{
		quint32 id = ret.serviceId();
		if (!connection->serviceDataObjects.contains(id))
		{
			QSharedPointer<ServiceData> data(new ServiceData(id, connection));
			rtData->serviceData = data;
			connection->registerServiceData(id, data);
		}
		else
		{

			ReturnValueData* rtData = const_cast<ReturnValueData*>(ret.qxt_d().data.constData());
			rtData->serviceData = connection->serviceDataObjects.value(id);
		}
	}
	return ret;
}

QSharedPointer<ServiceData> ClientProxyPrivate::getServiceData(const ReturnValue &ret) // because he's my biffle yo
{
	return ret.qxt_d().data->serviceData;
}

bool ClientProxyPrivate::isPrimary()
{
	if (!service.isNull())
	{
		return (service->primary == &qxt_p());
	}
	return false;
}

// used to get the shared data objects out of a return value and put it into this client proxy
ClientProxy& ClientProxy::operator=(const ReturnValue & service)
{
	if (!qxt_d().initialized) init();
	if (service.isService())
	{
		if (!qxt_d().service.isNull())
			qxt_d().service->removeProxy(this);
		qxt_d().service = qxt_d().getServiceData(service);
		Q_ASSERT(!qxt_d().service.isNull());
		qxt_d().connection = qxt_d().service->connection;
		qxt_d().service->addProxy(this);
	}
	else
	{
		qWarning() << "Attempt to assign ClientProxy to non-service ReturnValue";
	}
	return *this;
}

// share both connection and service...
ClientProxy& ClientProxy::operator=(const ClientProxy & other)
{
	if (!qxt_d().initialized) init();
	qxt_d().connection = other.qxt_d().connection;
	if (!qxt_d().service.isNull())
		qxt_d().service->removeProxy(this);
	qxt_d().service = other.qxt_d().service;
	qxt_d().service->addProxy(this);
	return *this;
}

AuthToken ClientProxy::authToken() const
{
	if (!qxt_d().connection)
		return AuthToken();
	return qxt_d().connection->token;
}

AuthToken &ClientProxy::authToken()
{
	static AuthToken brokenAuthToken;
	if (!qxt_d().connection)
		return brokenAuthToken;
	return qxt_d().connection->token;
}


