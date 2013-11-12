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
#ifndef QXTSERVICEBROWSER_P_H
#define QXTSERVICEBROWSER_P_H

#include "qxtservicebrowser.h"

// This is because bonjour thinks that if you're on windows, you're using MSVC >.>
#if defined(Q_CC_GNU) && defined(Q_OS_WIN32)
#undef _WIN32
#endif
#include <dns_sd.h>
#if defined(Q_CC_GNU) && defined(Q_OS_WIN32)
#define _WIN32
#endif
#include <QStringList>

class QSocketNotifier;
class QxtServiceBrowserPrivate : public QObject, public QxtPrivate<QxtServiceBrowser>
{
Q_OBJECT
public:
    QXT_DECLARE_PUBLIC(QxtServiceBrowser)
    QxtServiceBrowserPrivate()
        : QObject(),
        service(0),
        iface(0),
        notifier(0)
    {
    }

    DNSServiceRef service;

    QString domain;
    QAbstractSocket::SocketType socketType;
    QString serviceType;
    QStringList serviceSubTypes;
    int iface;

    QSocketNotifier* notifier;

    static void DNSSD_API browseServiceCallback(DNSServiceRef service, DNSServiceFlags flags, quint32 iface, DNSServiceErrorType errCode,
                                                const char* serviceName, const char* regtype, const char* replyDomain, void* context);

public Q_SLOTS:
    void socketData();
};

#endif // QXTSERVICEBROWSER_P_H
