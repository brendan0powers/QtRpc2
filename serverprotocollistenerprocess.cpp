/***************************************************************************
 *  Copyright (c) 2011, Resara LLC                                         *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  Redistribution and use in source and binary forms, with or without     *
 *  modification, are permitted provided that the following conditions are *
 *  met:                                                                   *
 *      * Redistributions of source code must retain the above copyright   *
 *        notice, this list of conditions and the following disclaimer.    *
 *      * Redistributions in binary form must reproduce the above          *
 *        copyright notice, this list of conditions and the following      *
 *        disclaimer in the documentation and/or other materials           *
 *        provided with the distribution.                                  *
 *      * Neither the name of Resara LLC nor the names of its              *
 *        contributors may be used to endorse or promote products          *
 *        derived from this software without specific prior written        *
 *        permission.                                                      *
 *                                                                         *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RESARA LLC BE   *
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR    *
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF   *
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        *
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,  *
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE   *
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN *
 *  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                          *
 *                                                                         *
 ***************************************************************************/
#include "serverprotocollistenerprocess.h"
#include "serverprotocollistenerprocess_p.h"
#include <QCoreApplication>
#include <ServerProtocolInstanceTcp>
#include <ServerThread>
#include <QStringList>
#include "sleeper.h"

namespace QtRpc
{

ServerProtocolListenerProcess::ServerProtocolListenerProcess(Server *parent)
		: ServerProtocolListenerBase(parent)
{
	QXT_INIT_PRIVATE(ServerProtocolListenerProcess);
	qxt_d().sslmode = SslEnabled;
	qxt_d().cert = "";
}

/**
 * Deconstructor
 */
ServerProtocolListenerProcess::~ServerProtocolListenerProcess()
{
}

bool ServerProtocolListenerProcess::isChild()
{
	QStringList args = qApp->arguments();
	if (args.size() < 4)
		return false;
	if (args.at(1) != "FORK_PROCESS")
		return false;
	return true;
}

/**
 * This function initializes the process listener. All settings such as SSL certificates must be called before this function.
 */
bool ServerProtocolListenerProcess::listen()
{
	QStringList args = qApp->arguments();
	if (args.size() < 4)
		return false;
	if (args.at(1) == "FORK_PROCESS")
	{
		int fd = args.at(2).toInt();
		QString protocol = args.at(3);
		if (protocol == "tcp")
		{
			ServerProtocolInstanceTcp* instance = new ServerProtocolInstanceTcp(server());
			instance->setProperty("descriptor", fd);
			instance->setProperty("sslmode", qxt_d().sslmode);
			instance->setProperty("certificate", qxt_d().cert);
			ServerThread *thread = new ServerThread();
			thread->start();
			instance->moveToThread(thread);
			QObject::connect(instance, SIGNAL(destroyed()), thread, SLOT(quit()), Qt::DirectConnection);
			QMetaObject::invokeMethod(instance, "init", Qt::QueuedConnection);
			while (thread->isRunning())
			{
				Sleeper::usleep(50000);
			}
			exit(0);
		}
		else
		{
			qFatal("Non-TCP process per instance connection!");
		}
	}
	return false;
}

/**
 * This function is used for setting the SSL mode.
 * @param mode The new SSL mode.
 */
void ServerProtocolListenerProcess::setSslMode(SslMode mode)
{
	qxt_d().sslmode = mode;
}

/**
 * This function is used for getting the SSL mode
 * @return Returns the current SSL mode.
 */
ServerProtocolListenerProcess::SslMode ServerProtocolListenerProcess::sslMode() const
{
	return qxt_d().sslmode;
}

/**
 * Sets the server's SSL certificate
 * @param cert The new SSL certificate.
 */
void ServerProtocolListenerProcess::setCertificate(const QString& cert)
{
	qxt_d().cert = cert;
}

/**
 * Gets the current SSL certificate.
 * @return Returns the current SSL certificate.
 */
QString ServerProtocolListenerProcess::certificate() const
{
	return qxt_d().cert;
}


}

