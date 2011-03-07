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
#ifndef QTRPCSERVICEFINDER_P_H
#define QTRPCSERVICEFINDER_P_H

#include <QxtPimpl>
#include <servicefinder.h>
#include "qxtservicebrowser.h"
#include "qxtdiscoverableservice.h"
#include <QHostInfo>
#include <qtrpcprivate.h>

namespace QtRpc
{

/**
	@author Chris Vickery <chris@resara.com>
*/
class ServiceFinderPrivate : public QObject, public QxtPrivate<ServiceFinder>
{
	Q_OBJECT
public:
	QXT_DECLARE_PUBLIC(ServiceFinder);
	QxtServiceBrowser* browser;
	QString service;
	QHash<int, ServiceFinder::Service> serviceQueue;
	QHash<int, ServiceFinder::Service> resolveQueue;
	QHash<QxtDiscoverableService*, int> resolverQueue;
	int currentId;

public slots:
	void getError(int);
	void serviceAdded(const QString& host, const QString& domain);
	void serviceRemoved(const QString& host, const QString& domain);
	void getAddress(QHostInfo);
	void resolved(const QByteArray& domainName);
	void resolveError(int code);
};

class ServicePrivate : public QxtPrivate<ServiceFinder::Service>
{
public:
	QString serviceType;
	QString serviceName;
	QString domain;
	QList<QHostAddress> allAddresses;
	QList<QHostAddress> validAddresses;
	QHostAddress address;
	ServiceFinder::Event event;
	quint16 port;
};

}

#endif
