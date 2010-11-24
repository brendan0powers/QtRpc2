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
#include "serverprotocollistenerbase.h"
#include "serverprotocollistenerbase_p.h"
#include <QDebug>
#include <QThread>
#include <QPointer>
#include <QMetaObject>
#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <errno.h>

namespace QtRpc
{

/**
 * Constructor, associates with the server.
 * @param server Initialized pointer to the Server object.
 */
ServerProtocolListenerBase::ServerProtocolListenerBase(Server *server)// : QObject(parent)
{
	QXT_INIT_PRIVATE(ServerProtocolListenerBase);
	if (QPointer<Server>(server).isNull())
	{
		qFatal("Fatal Error: ServerProtocolListener was passed a NULL Server* object.");
	}
	qxt_d().serv = server;
}

/**
 * Deconstructor
 */
ServerProtocolListenerBase::~ServerProtocolListenerBase()
{
}

Server* ServerProtocolListenerBase::server() const
{
	return qxt_d().serv;
}

/**
 * This function moves the instance object to the correct thread and invokes the init() function on the instance.
 * @param instance Pointer to the activated instance object.
 */
void ServerProtocolListenerBase::prepareInstance(ServerProtocolInstanceBase* instance, QThread* thread)
{
#ifndef Q_OS_WIN32
	if (qxt_d().serv->threadType() == Server::ProcessPerInstance)
	{
		QMetaObject::invokeMethod(instance, "deleteLater", Qt::QueuedConnection);
		QStringList args = qApp->arguments();
		int ret = fork();
		if (ret == -1)
		{
			qCritical() << "Failed to fork for incoming connection!" << strerror(errno);
			return;
		}
		if (ret == 0)
		{
			char** argv = new char*[5];
			argv[4] = NULL;

			argv[0] = new char[args.at(0).size()+1];
			memcpy(argv[0], qPrintable(args.at(0)), args.at(0).size());
			argv[0][args.at(0).size()] = NULL;

			QString argv1 = "FORK_PROCESS";
			argv[1] = new char[argv1.size()+1];
			memcpy(argv[1], qPrintable(argv1), argv1.size());
			argv[1][argv1.size()] = NULL;

			QString fd = QString("%1").arg(dup(instance->getProperty("descriptor").toInt()));
			argv[2] = new char[fd.size()+1];
			memcpy(argv[2], qPrintable(fd), fd.size());
			argv[2][fd.size()] = NULL;

			QString protocol = "tcp";
			argv[3] = new char[protocol.size()+1];
			memcpy(argv[3], qPrintable(protocol), protocol.size());
			argv[3][protocol.size()] = NULL;

			execvp(qPrintable(args.at(0)), argv);
			QFile file("/tmp/qtrpc_failure");
			file.open(QFile::WriteOnly | QFile::Truncate);
			file.write(qPrintable(QString("Failed to execv! %1 %2").arg(strerror(errno)).arg(errno)));
			file.close();
			qFatal(strerror(errno));
		}
		close(instance->getProperty("descriptor").toInt());
		return;
	}
#endif

	if (thread == 0)
		thread = qxt_d().serv->requestThread();
	instance->moveToThread(thread);
	switch (qxt_d().serv->threadType())
	{
		case Server::SingleThread:
			break;
		case Server::ThreadPool:
			if (instance == 0)
			{
				qCritical() << "A null instance was passed to prepareInstance!";
				return;
			}
			QObject::connect(instance, SIGNAL(destroyed()), qxt_d().serv, SLOT(removeService()), Qt::DirectConnection);
			break;
		case Server::ThreadPerInstance:
			if (instance == 0)
			{
				qCritical() << "A null instance was passed to prepareInstance!";
				thread->quit();
				return;
			}
			QObject::connect(instance, SIGNAL(destroyed()), thread, SLOT(quit()));
			break;
#ifndef Q_OS_WIN32
		case Server::ProcessPerInstance:
			qCritical() << "fixme: Threading model is ProcessPerInstance but it still made it to the case statement in prepareInstance, this should never happen.";
			break;
#endif
	}
	QMetaObject::invokeMethod(instance, "init", Qt::QueuedConnection);
}


}
