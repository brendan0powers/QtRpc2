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
#ifndef QXTDISCOVERABLESERVICENAME_H
#define QXTDISCOVERABLESERVICENAME_H

#include <QString>
#include <QByteArray>
#include <QAbstractSocket>
#include <QxtPimpl>
#include <QtRpcGlobal>

class QxtDiscoverableServiceNamePrivate;
class QTRPC2_EXPORT QxtDiscoverableServiceName
{
QXT_DECLARE_PRIVATE(QxtDiscoverableServiceName)
public:
    QxtDiscoverableServiceName();
    QxtDiscoverableServiceName(const QByteArray& domainName);
    QxtDiscoverableServiceName(const QString& name, const QString& serviceType, const QString& domain,
                               QAbstractSocket::SocketType socketType = QAbstractSocket::UnknownSocketType);

    QString serviceName() const;
    void setServiceName(const QString& name);

    QAbstractSocket::SocketType socketType() const;
    void setSocketType(QAbstractSocket::SocketType type);

    QString serviceType() const;
    void setServiceType(const QString& type);
    QByteArray fullServiceType() const;

    QString domain() const;
    void setDomain(const QString& domain);

    QString host() const;
    void setHost(const QString& host);

    QByteArray escapedFullDomainName() const;

    //mDNS stuff
    static int lookupHost(const QString name, QObject* receiver, const char* member);
    static void abortHostLookup(int id);
};

#endif // QXTDISCOVERABLESERVICENAME_H
