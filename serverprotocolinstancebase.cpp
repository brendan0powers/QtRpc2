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
#include "serverprotocolinstancebase.h"
#include "serverprotocolinstancebase_p.h"

#include <Server>
#include <ServiceProxy>
#include <QDebug>
#include <ReturnValue>
#include <QThread>
#include <Message>
#include <QStringList>
#include "returnvalue_p.h"
#include "serviceproxy_p.h"
#include "authtoken.h"

namespace QtRpc
{

/**
 * The deconstructor sets the server variable and initializes the id numbering system
 * @param serv An initialized pointer to the active Server object
 * @param parent Optional parent for the QObject
 */
ServerProtocolInstanceBase::ServerProtocolInstanceBase(Server* serv, QObject *parent) : QObject(parent)
{
	QXT_INIT_PRIVATE(ServerProtocolInstanceBase);
	qxt_d().serv = serv;
	qxt_d().servicename = "";
	qxt_d().curid = 0;
	qxt_d().curServiceId = 0;
	qxt_d().currentFunctionId = 0;
}

/**
 * Deconstructor
 */
ServerProtocolInstanceBase::~ServerProtocolInstanceBase()
{
	foreach(QSharedPointer<ServiceProxy> srv, qxt_d().services.values())
	{
		if (srv.isNull())
			continue;
		srv->qxt_d().instance = 0;
	}
}

ServiceProxy* ServerProtocolInstanceBase::service() const
{
	if (qxt_d().services.count() > 0)
		return qxt_d().services.constBegin().value().data();
	return 0;
}

ServiceProxy* ServerProtocolInstanceBase::service(quint32 id) const
{
	if (id == 0)
		return service();
	if (qxt_d().services.contains(id))
		return qxt_d().services.value(id).data();
	return 0;
}

QHash<uint, ServerProtocolInstanceBase::ReplySlot>& ServerProtocolInstanceBase::queue()
{
	return qxt_d().queue;
}

uint ServerProtocolInstanceBase::nextId()
{
	return ++qxt_d().curid;
}

/**
 * This function is called by child instance objects to select the proper service to be used. It returns the reply from ServiceProxy::auth() or an error message if something goes wrong.
 * @param name The name of the service to activate
 * @param username The username for authenticating with the service
 * @param pass The password to be used to authenticate with the service.
 * @return Returns the value of ServiceProxy::auth() or an error.
 */
ReturnValue ServerProtocolInstanceBase::getServiceObject(QString name, QString username, QString pass)
{
	qxt_d().servicename = name;
	QSharedPointer<ServiceProxy> srv(qxt_d().serv->requestService(name, this));
	if (srv == 0)
		return ReturnValue(1, "Received service template object was null (" + name + ")");
	srv->qxt_d().weakPointer = srv;
	qxt_d().curServiceId++;
	qxt_d().services[qxt_d().curServiceId] = srv;
	srv->setParent(0);
	ReturnValue ret = srv->auth(AuthToken(username, pass));
	if (!ret.isError())
		ret.setServiceId(qxt_d().curServiceId);
	return ret;
}

ReturnValue ServerProtocolInstanceBase::getServiceObject(QString name)
{
	qxt_d().servicename = name;
	QSharedPointer<ServiceProxy> srv(qxt_d().serv->requestService(name, this));
	if (srv == 0)
		return ReturnValue(1, "Received service template object was null (" + name + ")");
	srv->qxt_d().weakPointer = srv;
	qxt_d().curServiceId++;
	qxt_d().services[qxt_d().curServiceId] = srv;
	srv->setParent(0);
	qxt_d().needsAuth.append(qxt_d().curServiceId);
	ReturnValue ret;
	ret.setServiceId(qxt_d().curServiceId);
	return ret;
}

void ServerProtocolInstanceBase::moveToThread(QThread* thread)
{
	emit aboutToChangeThreads(thread);
	QObject::moveToThread(thread);
}

ReturnValue ServerProtocolInstanceBase::callFunction(quint32 id, quint32 serviceId, Signature sig, Arguments args)
{
	ServiceProxy* srv = service(serviceId);
	if (srv == 0)
		return ReturnValue(1, "The service object does not exist");
	if (sig.name() != "auth" && qxt_d().needsAuth.contains(serviceId))
		return ReturnValue(1, "You must authenticate before selecting a service!");
	if (sig == "auth(QString, QString)")
	{
		if (args.count() < 2)
			return ReturnValue(1, "Not enough paramters for that signature");
		sig = "auth(AuthToken)";
		args = QVariantList() << QVariant::fromValue(AuthToken(args[0].toString(), args[1].toString()));
	}
	for (int i = 0; i < sig.numArgs(); ++i)
	{
		if (sig.arg(i) == "AuthToken" || sig.arg(i) == "QtRpc::AuthToken")
		{
			if (args.at(i).value<AuthToken>().isDefault())
				args[i] = QVariant::fromValue(defaultToken());
		}
	}
	qxt_d().currentFunctionId = id;
	ReturnValue ret = srv->callFunction(sig, args);
	if (sig.name() == "auth" && !ret.isError())
	{
		qxt_d().needsAuth.removeAll(serviceId);
		srv->qxt_d().token = args[0].value<AuthToken>();
	}

	qxt_d().currentFunctionId = 0;
	ReturnValueData* rvData = const_cast<ReturnValueData*>(ret.qxt_d().data.constData());
	switch (rvData->type)
	{
		case ReturnValueData::Service:
		{
			QSharedPointer<ServiceProxy> srv;
			if (rvData->service.isNull())
			{
				if (rvData->rawServicePointer->qxt_d().weakPointer.isNull())
					srv = QSharedPointer<ServiceProxy>(rvData->rawServicePointer, &QObject::deleteLater);
				else
					srv = rvData->rawServicePointer->qxt_d().weakPointer.toStrongRef();
				rvData->rawServicePointer->qxt_d().weakPointer = srv;
				rvData->service = srv;
			}
			else
				srv = rvData->service;

			rvData->rawServicePointer->initProxy(qxt_d().serv, this, QHash<QString, void *>());
			quint32 id = qxt_d().services.key(srv, static_cast<quint32>(-1));
			if (id == static_cast<quint32>(-1))
			{
				id = ++qxt_d().curServiceId;
				qxt_d().services.insert(qxt_d().curServiceId, srv);
			}
			rvData->serviceId = id;
			break;
		}
		case ReturnValueData::Variant:
		case ReturnValueData::Error:
		case ReturnValueData::Asyncronous:
			break;
	}
	return ret;
}

AuthToken ServerProtocolInstanceBase::defaultToken()
{
	return qxt_d().defaultToken;
}

quint32 ServerProtocolInstanceBase::serviceId(ServiceProxy* service) const
{
	return qxt_d().services.key(service->qxt_d().weakPointer, 0);
}

ReturnValue ServerProtocolInstanceBase::callFunction(const Message &msg)
{
	return callFunction(msg.id(), msg.service(), msg.signature(), msg.arguments());
}

quint32 ServerProtocolInstanceBase::currentFunctionId() const
{
	return qxt_d().currentFunctionId;
}

ReturnValue ServerProtocolInstanceBase::listServices()
{
	return qxt_d().serv->listServices();
}

ReturnValue ServerProtocolInstanceBase::listFunctions(const QString &service)
{
	return QVariant::fromValue(qxt_d().serv->listFunctions(service));
}

ReturnValue ServerProtocolInstanceBase::listCallbacks(const QString &service)
{
	return QVariant::fromValue(qxt_d().serv->listCallbacks(service));
}

ReturnValue ServerProtocolInstanceBase::listEvents(const QString &service)
{
	return QVariant::fromValue(qxt_d().serv->listEvents(service));
}


}
