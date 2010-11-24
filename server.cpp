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
#include "server.h"
#include "server_p.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>
#include <QCoreApplication>
#include <QPointer>
#include <QStringList>
#ifndef Q_OS_WIN32
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#endif
#include <ServerThread>
#include <ServiceProxy>

namespace QtRpc
{

#ifndef Q_OS_WIN32
static void handler(int , siginfo_t *, void *)
{
	waitpid(-1, NULL, 0);
}
#endif

/**
 * The default constructor initializes the threading model, \a thread . Threading model cannot be changed after this point.
 * @param parent Qt parent object, defaults to no parent
 * @param thread The threading model to use. Defaults to ThreadPool.
 * @param threadCount This parameter is only used with the ThreadPool threading model. This is the number of threads to be used in the pool. Defaults to the number of proccessors + 1.
 */
Server::Server(QObject *parent, ThreadType thread, int threadCount, bool cleanChildren) :
		QObject(parent)
{
	QXT_INIT_PRIVATE(Server);
	qxt_d().threadType = thread;
	if (threadCount < 1)
		threadCount = QThread::idealThreadCount() + 1;
	if (threadCount < 1)
		threadCount = 10;
	if (qxt_d().threadType == ThreadPool)
	{
		for (int i = 0; i < threadCount; i++)
		{
			qxt_d().threadCount << 0;
			qxt_d().threads << new ServerThread(this);
			qxt_d().threads[i]->start();
		}
	}
	if (cleanChildren)
        {
#ifndef Q_OS_WIN32
		struct sigaction sa;
		sa.sa_flags = SA_SIGINFO;
		sigemptyset(&sa.sa_mask);
		sa.sa_sigaction = handler;
		if (sigaction(SIGCHLD, &sa, NULL) == -1)
			qCritical() << "Failed to register signal handler!" << strerror(errno);
#endif
	}
}

/**
 * Deconstructor
 */
Server::~Server()
{
}

/**
 * This function is used internally for telling instances/services which thread to run in. This function should never be called directly...
 * @sa removeService
 * @return A pointer to the QThread that should be used.
 */
QThread * Server::requestThread()
{
	QMutexLocker locker(&qxt_d().threadMutex);
	switch (qxt_d().threadType)
	{
		case SingleThread:
			return thread();
			break;
		case ThreadPool:
		{
			int thread = 0;
			int lowest = qxt_d().threadCount[0];
			for (int i = 0;i < qxt_d().threads.count(); i++)
			{
				if (qxt_d().threadCount[i] == 0)
				{
					qxt_d().threadCount[i]++;
					return qxt_d().threads[i];
				}
				if (lowest > qxt_d().threadCount[i])
					thread = i;
			}
			qxt_d().threadCount[thread]++;
			return qxt_d().threads[thread];
		}
		break;
#ifndef Q_OS_WIN32
		case ProcessPerInstance:
			qCritical() << "fixme: ProcessPerInstance threading asking for a thread, this should never happen as ProcessPerInstance handles it's threading on it's own.";
			break;
#endif
		case ThreadPerInstance:
			ServerThread *thread = new ServerThread(this);
			thread->start();
			return(thread);
			break;
	}
	return thread();
}

/**
 * Get the threading model.
 * @return Returns the threading model used by this server.
 */
Server::ThreadType Server::threadType()
{
	return qxt_d().threadType;
}

/**
 * This function is called to lower the count on a given thread when using threadpool. This function should never be called directly as it for internal use only.
 * @sa requestThread
 */
void Server::removeService()
{
	QMutexLocker locker(&qxt_d().threadMutex);
	qxt_d().threadCount[qxt_d().threads.indexOf(QThread::currentThread())]--;
}

/**
 * This function is used internally to select a service. This function should be run in the thread that the service object should reside in. Do not call this function directly.
 * @sa registerService
 * @param name The name of the service to select
 * @return Returns an initialized pointer to the new service object.
 */
ServiceProxy* Server::requestService(QString name, ServerProtocolInstanceBase* protocol)
{
	QMutexLocker locker(&qxt_d().servicemutex);
	if (_serviceFactories.contains(name))
	{
		ServiceProxy* service = _serviceFactories[name]->newInstance();
		service->initProxy(this, protocol, _serviceFactories[name]->instance().getRawData());
		service->setServiceName(_serviceFactories[name]->instance().serviceName());
		return service;
	}
	return NULL;
}

QStringList Server::listServices()
{
	QMutexLocker locker(&qxt_d().servicemutex);
	return _serviceFactories.keys();
}

QList<Signature> Server::listFunctions(const QString &service)
{
	QMutexLocker locker(&qxt_d().servicemutex);
	if (!_serviceFactories.contains(service))
	{
		qDebug() << "Failed to find service:" << service;
		return QList<Signature>();
	}
	return _serviceFactories[service]->instance().listFunctions();
}

QList<Signature> Server::listCallbacks(const QString &service)
{
	QMutexLocker locker(&qxt_d().servicemutex);
	if (!_serviceFactories.contains(service))
		return QList<Signature>();
	return _serviceFactories[service]->instance().listCallbacks();
}

QList<Signature> Server::listEvents(const QString &service)
{
	QMutexLocker locker(&qxt_d().servicemutex);
	if (!_serviceFactories.contains(service))
		return QList<Signature>();
	return _serviceFactories[service]->instance().listEvents();
}


}

