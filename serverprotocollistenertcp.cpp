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
#include "serverprotocollistenertcp.h"
#include "serverprotocollistenertcp_p.h"

#include <ServerProtocolInstanceTcp>
#include <Server>

namespace QtRpc
{

/**
 * Constructor
 * @param parent Valid pointer to active Server object. The Server will also be used as the QObject parent.
 */
ServerProtocolListenerTcp::ServerProtocolListenerTcp(Server *parent) : QTcpServer(parent), ServerProtocolListenerBase(parent)
{
	QXT_INIT_PRIVATE(ServerProtocolListenerTcp);
	qxt_d().sslmode = SslEnabled;
	qxt_d().cert = "";
}

/**
 * Overloaded constructor
 * @param serv Valid pointer to the active Server object.
 * @param parent Parent for the QObject
 */
ServerProtocolListenerTcp::ServerProtocolListenerTcp(Server *serv, QObject* parent) : QTcpServer(parent), ServerProtocolListenerBase(serv)
{
	QXT_INIT_PRIVATE(ServerProtocolListenerTcp);
	qxt_d().sslmode = SslEnabled;
	qxt_d().cert = "";
}

/**
 * Deconstructor
 */
ServerProtocolListenerTcp::~ServerProtocolListenerTcp()
{
}

/**
 * This function is used internally for incoming connections. This function should never be called directly.
 * @param desc Socket Descriptor of the incoming connection.
 */
void ServerProtocolListenerTcp::incomingConnection(int desc)
{
	QReadLocker locker(&qxt_d().modemutex);
	QReadLocker locker2(&qxt_d().certmutex);
	ServerProtocolInstanceTcp* instance = new ServerProtocolInstanceTcp(server());
	instance->setProperty("descriptor", desc);
	qxt_d().modemutex.lockForRead();
	instance->setProperty("sslmode", qxt_d().sslmode);
	qxt_d().modemutex.unlock();
	qxt_d().certmutex.lockForRead();
	instance->setProperty("certificate", qxt_d().cert);
	qxt_d().certmutex.unlock();
	prepareInstance(instance);
}

/**
 * This function is used for setting the SSL mode.
 * @param mode The new SSL mode.
 */
void ServerProtocolListenerTcp::setSslMode(SslMode mode)
{
	QWriteLocker locker(&qxt_d().modemutex);
	qxt_d().sslmode = mode;
}

/**
 * This function is used for getting the SSL mode
 * @return Returns the current SSL mode.
 */
ServerProtocolListenerTcp::SslMode ServerProtocolListenerTcp::sslMode() const
{
	QReadLocker locker(const_cast<QReadWriteLock*>(&qxt_d().modemutex));
	return qxt_d().sslmode;
}

/**
 * Sets the server's SSL certificate
 * @param cert The new SSL certificate.
 */
void ServerProtocolListenerTcp::setCertificate(const QString& cert)
{
	QWriteLocker locker(&qxt_d().certmutex);
	qxt_d().cert = cert;
}

/**
 * Gets the current SSL certificate.
 * @return Returns the current SSL certificate.
 */
QString ServerProtocolListenerTcp::certificate() const
{
	QReadLocker locker(const_cast<QReadWriteLock*>(&qxt_d().certmutex));
	return qxt_d().cert;
}


}
