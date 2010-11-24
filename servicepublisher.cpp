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
#include "servicepublisher.h"
#include "servicepublisher_p.h"
#include <QMutexLocker>

namespace QtRpc
{

ServicePublisher::ServicePublisher(ServiceProxy* serv)
		: QObject(serv)
{
	QXT_INIT_PRIVATE(ServicePublisher);
	qxt_d().service = serv;
	qxt_d().zeroconf = new QxtDiscoverableService("QtRpc_" + qxt_d().service->serviceName(), this);
	connect(qxt_d().zeroconf, SIGNAL(registrationError(int)), this, SIGNAL(registrationError(int)));

}

ServicePublisher::ServicePublisher(ServiceProxy* serv, QObject *parent)
		: QObject(parent)
{
	QXT_INIT_PRIVATE(ServicePublisher);
	qxt_d().service = serv;
	qxt_d().zeroconf = new QxtDiscoverableService("QtRpc_" + qxt_d().service->serviceName(), this);
	connect(qxt_d().zeroconf, SIGNAL(registrationError(int)), this, SIGNAL(registrationError(int)));
}

ServicePublisher::~ServicePublisher()
{
}

void ServicePublisher::setFriendlyName(QString name)
{
	QMutexLocker locker(&qxt_d().mutex);
	qxt_d().friendlyName = name;
}

QString ServicePublisher::friendlyName()
{
	QMutexLocker locker(&qxt_d().mutex);
	return qxt_d().friendlyName;
}

void ServicePublisher::setPort(int port)
{
	QMutexLocker locker(&qxt_d().mutex);
	qxt_d().port = port;
}

int ServicePublisher::port()
{
	QMutexLocker locker(&qxt_d().mutex);
	return qxt_d().port;
}

void ServicePublisher::publish()
{
	qxt_d().zeroconf->releaseService();
	qxt_d().zeroconf->setServiceType("QtRpc_" + qxt_d().service->serviceName());
	qxt_d().zeroconf->setServiceName(qxt_d().friendlyName);
	qxt_d().zeroconf->setPort(qxt_d().port);
	qxt_d().zeroconf->registerService();
}

void ServicePublisher::unpublish()
{
	qxt_d().zeroconf->releaseService();
}

}
