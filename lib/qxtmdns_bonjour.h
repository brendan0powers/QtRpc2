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
#ifndef QXTMDNS_H
#define QXTMDNS_H

#include <QObject>
#include <QHostInfo>
#include <QtRpcGlobal>
// This is because bonjour thinks that if you're on windows, you're using MSVC >.>
#if defined(Q_CC_GNU) && defined(Q_OS_WIN32)
#undef _WIN32
#endif
#include <dns_sd.h>
#if defined(Q_CC_GNU) && defined(Q_OS_WIN32)
#define _WIN32
#endif
#include <QSocketNotifier>

class QTRPC2_EXPORT QxtMDNS : public QObject
{
	Q_OBJECT
public:
	QxtMDNS(int id = -1, QObject* parent = 0);

	void doLookup(QString name, QObject * receiver, const char * member);
	void cancelLookup();

	static void DNSSD_API DNSServiceQueryRecordCallback(DNSServiceRef DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, uint16_t rrtype, uint16_t rrclass, uint16_t rdlen, const void *rdata, uint32_t ttl, void *context);

	QHostInfo info;
	QList<QHostAddress> addresses;
	QString name;
	QObject* receiver;
        QString member;
	DNSServiceRef ref;
	QSocketNotifier* notifier;
	int id;
public Q_SLOTS:
	void socketData();


};

#endif //#ifndef QXTMDNS_H
