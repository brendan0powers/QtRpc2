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
#include "servicefinder.h"
#include "servicefinder_p.h"
#include <QNetworkInterface>
#include <QMetaType>
#include <ReturnValue>

namespace QtRpc
{

ServiceFinder::ServiceFinder(QObject *parent)
		: QObject(parent)
{
	QXT_INIT_PRIVATE(ServiceFinder);
	qxt_d().currentId = 0;
	qxt_d().browser = new QxtServiceBrowser("QtRpc_" , this);
}

ServiceFinder::ServiceFinder(QString service, QObject *parent)
		: QObject(parent)
{
	QXT_INIT_PRIVATE(ServiceFinder);
	qxt_d().currentId = 0;
	qxt_d().service = service;
	qxt_d().browser = new QxtServiceBrowser("QtRpc_" + qxt_d().service, this);
}

void ServiceFinder::setService(QString service)
{
	qxt_d().service = service;
	qxt_d().browser->deleteLater();
	qxt_d().browser = new QxtServiceBrowser("QtRpc_" + qxt_d().service, this);
}

QString ServiceFinder::service()
{
	return qxt_d().service;
}

void ServiceFinder::scan()
{
	if (qxt_d().browser->isBrowsing())
	{
		qxt_d().serviceQueue.clear();
		foreach(QxtDiscoverableService* ds, qxt_d().resolverQueue.keys())
		{
			ds->deleteLater();
		}
		qxt_d().resolverQueue.clear();
		foreach(int id, qxt_d().resolveQueue.keys())
		{
			QxtDiscoverableServiceName::abortHostLookup(id);
		}
		qxt_d().resolveQueue.clear();
	}
	connect(qxt_d().browser, SIGNAL(browsingFailed(int)), &(qxt_d()), SLOT(getError(int)));
	connect(qxt_d().browser, SIGNAL(serviceAdded(const QString&, const QString&)), &(qxt_d()), SLOT(serviceAdded(const QString&, const QString&)));
	connect(qxt_d().browser, SIGNAL(serviceRemoved(const QString&, const QString&)), &(qxt_d()), SLOT(serviceRemoved(const QString&, const QString&)));
	qxt_d().browser->browse();
}

ServiceFinder::~ServiceFinder()
{
}

void ServiceFinderPrivate::getError(int)
{
	ServiceFinder::Service srv;
	currentId++;
	serviceQueue[currentId].setServiceType(service);
	srv.setEvent(ServiceFinder::Error);
	qxt_p().serviceEvent(srv);
}

void ServiceFinderPrivate::serviceAdded(const QString& serviceName, const QString& domain)
{
	currentId++;

	QxtDiscoverableService* resolver = new QxtDiscoverableService(browser->serviceType(), serviceName, this);
	resolverQueue[resolver] = currentId;
	connect(resolver, SIGNAL(resolved(const QByteArray&)), this, SLOT(resolved(const QByteArray&)));
	connect(resolver, SIGNAL(resolveError(int)), this, SLOT(resolveError(int)));
	resolver->resolve();

	serviceQueue.insert(currentId, ServiceFinder::Service());
	serviceQueue[currentId].setServiceType(service);
	serviceQueue[currentId].setServiceName(serviceName);
	serviceQueue[currentId].setDomain(domain);

	serviceQueue[currentId].setEvent(ServiceFinder::ServiceAdded);
}

void ServiceFinderPrivate::serviceRemoved(const QString& serviceName, const QString& domain)
{
	currentId++;

	QxtDiscoverableService* resolver = new QxtDiscoverableService(service, serviceName, this);
	resolverQueue[resolver] = currentId;
	connect(resolver, SIGNAL(resolved(const QByteArray&)), this, SLOT(resolved(const QByteArray&)));
	connect(resolver, SIGNAL(resolveError(int)), this, SLOT(resolveError(int)));
	resolver->resolve();

	serviceQueue[currentId].setServiceType(service);
	serviceQueue[currentId].setServiceName(serviceName);
	serviceQueue[currentId].setDomain(domain);

	serviceQueue[currentId].setEvent(ServiceFinder::ServiceRemoved);
}

void ServiceFinderPrivate::resolved(const QByteArray& domainName)
{
	Q_UNUSED(domainName);
	if (sender() == NULL)
	{
		// error
		return;
	}
	QxtDiscoverableService* resolver = qobject_cast<QxtDiscoverableService*>(sender());
	if (resolver == NULL)
	{
		//error
		return;
	}

	if (!resolverQueue.contains(resolver))
	{
		qCritical() << "resolverQueue";
		return;
	}
	int oldid = resolverQueue.take(resolver);
	if (!serviceQueue.contains(oldid))
	{
		qCritical() << "serviceQueue";
		return;
	}

	int id = QxtDiscoverableServiceName::lookupHost(resolver->host(), this, SLOT(getAddress(QHostInfo)));
	resolveQueue[id] = serviceQueue.value(oldid);
	resolveQueue[id].setPort(resolver->port());
	resolver->deleteLater();
}

void ServiceFinderPrivate::resolveError(int code)
{
	Q_UNUSED(code);
	if (sender() == NULL)
	{
		qCritical() << "Do not access the private implementation of ServiceFinder directly.";
		return;
	}
	QxtDiscoverableService* resolver = qobject_cast<QxtDiscoverableService*>(sender());
	if (resolver == NULL)
	{
		qCritical() << "Do not access the private implementation of ServiceFinder directly.";
		return;
	}

	if (!resolverQueue.contains(resolver))
	{
		qCritical() << "resolverQueue does not contain the resolver, the behavior is undefined.";
		return;
	}
	int oldid = resolverQueue.take(resolver);
	if (!serviceQueue.contains(oldid))
	{
		qCritical() << "serviceQueue does not contain the service id referenced by the resolverQueue, the behavior is undefined.";
		return;
	}

	int id = QxtDiscoverableServiceName::lookupHost(resolver->host(), this, SLOT(getAddress(QHostInfo)));
	resolveQueue[id] = serviceQueue.value(oldid);
	resolver->deleteLater();
}

void ServiceFinderPrivate::getAddress(QHostInfo info)
{
	QxtDiscoverableServiceName::abortHostLookup(info.lookupId());
	int i = info.lookupId();
	QList<QHostAddress> allAddr;
	QList<QHostAddress> validAddr;
	foreach(QHostAddress addr, info.addresses())
	{
		allAddr << addr;
		if (addr.toIPv4Address() == 0)
			continue;
		foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
		{
			foreach(QNetworkAddressEntry entry, interface.addressEntries())
			{
				if (entry.ip().toIPv4Address() == 0)
					continue;
				if ((entry.ip().toIPv4Address() & entry.netmask().toIPv4Address()) == (addr.toIPv4Address() & entry.netmask().toIPv4Address()))
				{
					validAddr << addr;
					break;
				}
			}
		}
	}
	resolveQueue[i].setAllAddresses(allAddr);
	resolveQueue[i].setValidAddresses(validAddr);
	if (validAddr.count() > 0)
		resolveQueue[i].setAddress(validAddr.first());
	emit qxt_p().serviceEvent(resolveQueue.take(i));
}

ServiceFinder::Service::Service()
{
	QXT_INIT_PRIVATE(ServiceFinder::Service);
	registerMetaTypes();
	qxt_d().port = 0;
}

ServiceFinder::Service::Service(const Service& other)
{
	QXT_INIT_PRIVATE(ServiceFinder::Service);
	operator=(other);
}

ServiceFinder::Service& ServiceFinder::Service::operator=(const Service & other)
{

	qxt_d().serviceType = other.qxt_d().serviceType;
	qxt_d().serviceName = other.qxt_d().serviceName;
	qxt_d().domain = other.qxt_d().domain;
	qxt_d().allAddresses = other.qxt_d().allAddresses;
	qxt_d().validAddresses = other.qxt_d().validAddresses;
	qxt_d().address = other.qxt_d().address;
	qxt_d().event = other.qxt_d().event;
	qxt_d().port = other.qxt_d().port;

	return *this;
}

ServiceFinder::Service::~Service()
{
}

void ServiceFinder::Service::setServiceType(QString serviceType)
{
	qxt_d().serviceType = serviceType;
}

QString ServiceFinder::Service::serviceType()
{
	return qxt_d().serviceType;
}

void ServiceFinder::Service::setServiceName(QString serviceName)
{
	qxt_d().serviceName = serviceName;
}

QString ServiceFinder::Service::serviceName()
{
	return qxt_d().serviceName;
}

void ServiceFinder::Service::setDomain(QString domain)
{
	qxt_d().domain = domain;
}

QString ServiceFinder::Service::domain()
{
	return qxt_d().domain;
}

void ServiceFinder::Service::setAllAddresses(QList<QHostAddress> allAddresses)
{
	qxt_d().allAddresses = allAddresses;
}

QList<QHostAddress> ServiceFinder::Service::allAddresses()
{
	return qxt_d().allAddresses;
}

void ServiceFinder::Service::setValidAddresses(QList<QHostAddress> validAddresses)
{
	qxt_d().validAddresses = validAddresses;
}

QList<QHostAddress> ServiceFinder::Service::validAddresses()
{
	return qxt_d().validAddresses;
}

void ServiceFinder::Service::setAddress(QHostAddress address)
{
	qxt_d().address = address;
}

QHostAddress ServiceFinder::Service::address()
{
	return qxt_d().address;
}

void ServiceFinder::Service::setEvent(ServiceFinder::Event event)
{
	qxt_d().event = event;
}

ServiceFinder::Event ServiceFinder::Service::event()
{
	return qxt_d().event;
}

void ServiceFinder::Service::setPort(quint16 port)
{
	qxt_d().port = port;
}

quint16 ServiceFinder::Service::port()
{
	return qxt_d().port;
}


}
