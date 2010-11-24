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
#ifndef QTRPCSERVER_H
#define QTRPCSERVER_H

#include <QObject>
#include <QxtPimpl>
#include <QHash>
#include <ServiceFactory>
#include <QtRpcGlobal>

class QThread;
// class QHash;
class QMutex;

namespace QtRpc
{

class ServerProtocolInstanceBase;
class ServerPrivate;
class ServiceProxy;

/**
This is the main peice of the server side of QtRpc2. This object by itself does virtually nothing, instead you attach listeners and services to it which do most of the work. The only real work this class does is managing all the other classes.
	@brief Central server object for use by QtRpc2
	@author Brendan Powers <brendan@resara.com>
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT Server : public QObject
{
	QXT_DECLARE_PRIVATE(Server);
	Q_OBJECT
public:
	/**
	 * This enum represents the different threading models that are available for QtRpc2.
	 */
	enum ThreadType
	{
		SingleThread,		/**< Single Threaded means that everything on the server will run in the same thread. In almost every case this is the worst model. */
		ThreadPool,		/**< Thread Pool means that a predefined number of threads will be created at the startup of the server and each connection/service pair is load balanced across them. If this number is not set manually, it defaults to the number of processors + 1. In almost all cases this is the best model. */
		ThreadPerInstance,	/**< Thread Per Instance means that every single connection/service pair runs in it's own seperate thread. This model is very useful in certain situations, but in most cases is inferior to the ThreadPool model. */
#ifndef Q_OS_WIN32
		ProcessPerInstance	/**< This threading model actually forks off and execv's for each incoming connection. This makes centralized services that manage other services not work (like LaptopManager etc) */
#endif
	};
	Server(QObject *parent = 0, ThreadType thread = ThreadPool, int threadCount = -1, bool cleanChildren = false);
	~Server();

	QThread * requestThread();
	template<class Service>
	ServiceProxy* registerService(QString name)
	{
		if (!_serviceFactories.contains(name))
		{
			_serviceFactories[name] = new ServiceFactory<Service>();
		}

		_serviceFactories[name]->instance().setServiceName(name);
		return &_serviceFactories[name]->instance();
	}
	ServiceProxy* requestService(QString name, ServerProtocolInstanceBase*);
	QString getServiceName(ServiceProxy* service);
	ThreadType threadType();
	QStringList listServices();
	QList<Signature> listFunctions(const QString &service);
	QList<Signature> listCallbacks(const QString &service);
	QList<Signature> listEvents(const QString &service);
public slots:
	void removeService();

private:
	QHash<QString, ServiceFactoryParent*> _serviceFactories;
	/*
	QHash<QString, ServiceProxy *> _services;
	QHash<QString, ServiceFactoryParent*> _serviceFactories;
	QMutex _servicemutex;
	ThreadType _threadType;
	QList<int> _threadCount;
	QList<QThread*> _threads;
	QMutex _threadMutex;
	*/
};

}

#endif
