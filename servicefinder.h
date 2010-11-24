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
#ifndef QTRPCSERVICEFINDER_H
#define QTRPCSERVICEFINDER_H

#include <QObject>
#include <QxtPimpl>
#include <QtRpcGlobal>

class QHostAddress;

namespace QtRpc
{

class ServiceFinderPrivate;
class ServicePrivate;

/**
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ServiceFinder : public QObject
{
	Q_OBJECT
public:
	enum Event
	{
		ServiceAdded,
		ServiceRemoved,
// 		AllForNow, // <-- not exposed
		Error
	};

        class QTRPC2_EXPORT Service
	{
		public:
			Service();
			Service(const Service& other);
			Service& operator=(const Service& other);
			~Service();

		void setServiceType(QString serviceType);
		QString serviceType();
		void setServiceName(QString serviceName);
		QString serviceName();
		void setDomain(QString domain);
		QString domain();
		void setAllAddresses(QList<QHostAddress> allAddresses);
		QList<QHostAddress> allAddresses();
		void setValidAddresses(QList<QHostAddress> validAddresses);
		QList<QHostAddress> validAddresses();
		void setAddress(QHostAddress address);
		QHostAddress address();
		void setEvent(ServiceFinder::Event event);
		ServiceFinder::Event event();
		void setPort(quint16 port);
		quint16 port();

	private:
		QXT_DECLARE_PRIVATE(Service);
	};

	ServiceFinder(QObject *parent = 0);
	ServiceFinder(QString service, QObject *parent = 0);
	void setService(QString service);
	QString service();
	~ServiceFinder();

public slots:
	void scan();

signals:
	void serviceEvent(ServiceFinder::Service);

private:
	QXT_DECLARE_PRIVATE(ServiceFinder);

};

}

#endif
