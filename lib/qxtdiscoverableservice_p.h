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
#ifndef QXTDISCOVERABLESERVICE_P_H
#define QXTDISCOVERABLESERVICE_P_H

#include "qxtdiscoverableservice.h"

// This is because bonjour thinks that if you're on windows, you're using MSVC >.>
#if defined(Q_CC_GNU) && defined(Q_OS_WIN32)
#undef _WIN32
#endif
#include <dns_sd.h>
#if defined(Q_CC_GNU) && defined(Q_OS_WIN32)
#define _WIN32
#endif
        typedef struct _DNSServiceRef_t
            {
            qint32 sockfd;  // connected socket between client and daemon
            uint32_t op;          // request_op_t or reply_op_t
            } _DNSServiceRef_t;
class QSocketNotifier;

class QxtDiscoverableServicePrivate : public QObject, public QxtPrivate<QxtDiscoverableService>
{
Q_OBJECT
public:
    QXT_DECLARE_PUBLIC(QxtDiscoverableService);
    QxtDiscoverableServicePrivate()
    {
        port = 0;
        iface = 0;
        notifier = 0;
        state = QxtDiscoverableService::Unknown;
    }

    QxtDiscoverableService::State state;

    DNSServiceRef service;
    QStringList serviceSubTypes;
    quint16 port;
    int iface;
    QString txtRecord;

    QSocketNotifier* notifier;

    static void DNSSD_API registerServiceCallback(DNSServiceRef service, DNSServiceFlags flags, DNSServiceErrorType errCode,
                                                  const char* name, const char* regtype, const char* domain, void* context);
#if defined(Q_OS_WIN32) && !defined(Q_CC_GNU)
    static void DNSSD_API resolveServiceCallback
            (
            DNSServiceRef                       service,
            DNSServiceFlags                     flags,
            uint32_t                            iface,
            DNSServiceErrorType                 errCode,
            const char                          *fullname,
            const char                          *host,
            uint16_t                            port,
            uint16_t                            txtLen,
            const unsigned char                 *txt,
            void                                *context
            );
#else
    static void resolveServiceCallback
            (
            DNSServiceRef                       service,
            DNSServiceFlags                     flags,
            uint32_t                            iface,
            DNSServiceErrorType                 errCode,
            const char                          *fullname,
            const char                          *host,
            uint16_t                            port,
            uint16_t                            txtLen,
            const unsigned char                 *txt,
            void                                *context
            );
#endif

public Q_SLOTS:
    void socketData();
};

template <typename T>
void qxt_zeroconf_parse_subtypes(T* t, const QByteArray& regtype) {
    QList<QByteArray> subtypes = regtype.split(',');
    QList<QByteArray> rt = subtypes[0].split('.');
    int ct = subtypes.count();
    t->serviceSubTypes.clear();
    if(ct > 1) {
        for(int i = 1; i < ct; i++) {
            if(subtypes.at(i)[0] == '_') {
                t->serviceSubTypes.append(subtypes[i].mid(1));
            } else {
                t->serviceSubTypes.append(subtypes[i]);
            }
        }
    }
}

#endif // QXTDISCOVERABLESERVICE_P_H
