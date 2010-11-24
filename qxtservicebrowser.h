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
#ifndef QXTSERVICEBROWSER_H
#define QXTSERVICEBROWSER_H

#include <QObject>
#include <QAbstractSocket>
#include <QxtPimpl>
#include <QtRpcGlobal>
#include "qxtdiscoverableservicename.h"

class QxtServiceBrowserPrivate;
class QTRPC2_EXPORT QxtServiceBrowser : public QObject, public QxtDiscoverableServiceName
{
Q_OBJECT
QXT_DECLARE_PRIVATE(QxtServiceBrowser);
public:
    QxtServiceBrowser(const QString& serviceType, QObject* parent = 0);
    QxtServiceBrowser(const QString& serviceType, QAbstractSocket::SocketType socketType, QObject* parent = 0);
    virtual ~QxtServiceBrowser();

    bool isBrowsing() const;

public slots:
    void browse(/* int iface */);
    void stopBrowsing();

public:
    QStringList serviceSubTypes() const;
    void setServiceSubTypes(const QStringList& subtypes);
    void addServiceSubType(const QString& subtype);
    void removeServiceSubType(const QString& subtype);
    bool hasServiceSubType(const QString& subtype);

Q_SIGNALS:
    void browsingFailed(int errorCode);
    void serviceAdded(const QString& serviceName, const QString& domain);
    void serviceRemoved(const QString& serviceName, const QString& domain);
};

#endif // QXTSERVICEBROWSER_H
