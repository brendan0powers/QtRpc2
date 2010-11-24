/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtWeb module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/
#include "qxtmdns_bonjour.h"
#include <QDebug>
#include <QtEndian>
#include <Signature>
#include <QRegExp>

QxtMDNS::QxtMDNS(int id, QObject * parent)
		: QObject(parent),
                ref(0),
          info(id)
{
}

void QxtMDNS::doLookup(QString n, QObject * r, const char * m)
{
	name = n;
	receiver = r;
        member = QtRpc::Signature(m).name().remove(QRegExp("^1"));
	DNSServiceErrorType err = DNSServiceQueryRecord(
	                              &ref,
	                              0,
	                              0,
                                      qPrintable(name),
                                      kDNSServiceType_A,
	                              kDNSServiceClass_IN,
	                              QxtMDNS::DNSServiceQueryRecordCallback,
	                              this
	                          );
	if (err != kDNSServiceErr_NoError)
	{
                qWarning() << "QxtMDNS failed to initialize the Bonjour request" << err;
		QHostInfo info(info.lookupId());
		info.setErrorString("Failed to initialize the Bonjour request.");
                QMetaObject::invokeMethod(receiver, qPrintable(member), Q_ARG(QHostInfo, info));
	}
	else
        {
		notifier = new QSocketNotifier(DNSServiceRefSockFD(ref), QSocketNotifier::Read, this);
		QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(socketData()));
	}
}

void QxtMDNS::DNSServiceQueryRecordCallback(DNSServiceRef DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, uint16_t rrtype, uint16_t rrclass, uint16_t rdlen, const void *rdata, uint32_t ttl, void *context)
{
	QxtMDNS* md = static_cast<QxtMDNS*>(context);
	QHostInfo info(md->info.lookupId());
	uint32_t ip = qFromBigEndian(*static_cast<const uint32_t*>(rdata));
	info.setAddresses(QList<QHostAddress>() << QHostAddress(ip));
        qDebug() << QHostAddress(ip);
        QMetaObject::invokeMethod(md->receiver, qPrintable(md->member), Q_ARG(QHostInfo, info));
}

void QxtMDNS::socketData()
{
	DNSServiceProcessResult(ref);
}

void QxtMDNS::cancelLookup()
{
}
