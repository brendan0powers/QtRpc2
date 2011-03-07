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
#include "serverprotocolinstancetcp.h"
#include "serverprotocolinstancetcp_p.h"
#include <QtCore/qconfig.h>
#ifdef QT_NO_OPENSSL
#define QSslSocket QTcpSocket
#endif
#include <QTcpSocket>
#include <QSslSocket>
#include <QDateTime>
#include <QFile>
#include <QMutexLocker>

namespace QtRpc
{

/**
 * The contructor initializes several variables and connects some signals and slots needed for timeouts to work properly.
 * @param serv Initialized pointer to the active Server ocject
 * @param parentOptional parent for the QObject
 */
ServerProtocolInstanceTcp::ServerProtocolInstanceTcp(Server* serv, QObject* parent): ServerProtocolInstanceIODevice(serv, parent)
{
	QXT_INIT_PRIVATE(ServerProtocolInstanceTcp);
	qxt_d().sd = -1;
	qxt_d().sslmode = ServerProtocolListenerTcp::SslEnabled;
	qxt_d().cert = "";
	qxt_d().timeoutEnabled = false;
	qxt_d().timeout = 20;
	connect(&qxt_d().timer, SIGNAL(timeout()), &qxt_d(), SLOT(ping()));
	qxt_d().lastPing = QDateTime::currentDateTime();
}

/**
 * deconstructor
 */
ServerProtocolInstanceTcp::~ServerProtocolInstanceTcp()
{
}

/**
 * This function is used internally to make timeouts work. Timeouts are enabled and disabled using setProperty.
 * @sa setProperty
 */
void ServerProtocolInstanceTcpPrivate::ping()
{
	if (lastPing.secsTo(QDateTime::currentDateTime()) > timeout)
	{
		qCritical() << "Remove host has timed out.";
		disconnect();
		return;
	}
	qxt_p().callProtocolFunction(Signature("ping()"), Arguments());
}

/**
 * This function disconnects from the remote host and cleans up all classes related to the connection, including this one.
 */
void ServerProtocolInstanceTcp::disconnect()
{
	QTcpSocket* socket = qxt_d().socket;
	qxt_d().socket = NULL;
	if(socket != NULL)
	{
		socket->disconnectFromHost();
	}
	deleteLater(); //the service object is a child, so he will be cleaned in due time...
}

/**
 * This function is used for setting arbitrary properties in the Tcp instance object. Available properties are: descriptor, sslmode, certificate, timeoutEnabled, and timeout.
 * @sa getProperty()
 * @param prop Name of the property to set
 * @param val The new value of \a prop
 */
void ServerProtocolInstanceTcp::setProperty(QString prop, QVariant val)
{
	QMutexLocker locker(mutex());
	if (prop == "descriptor")
		qxt_d().sd = val.toInt();
	else if (prop == "sslmode")
		qxt_d().sslmode = (ServerProtocolListenerTcp::SslMode)val.toInt();
	else if (prop == "certificate")
		qxt_d().cert = val.toString();
	else if (prop == "timeoutEnabled")
	{
		if (qxt_d().timeoutEnabled != val.toBool())
		{
			if (val.toBool())
			{
				callProtocolFunction(Signature("enableTimeout(int)"), Arguments() << qxt_d().timeout);
				qxt_d().lastPing = QDateTime::currentDateTime();
				qxt_d().timer.start((qxt_d().timeout / 4));
			}
			else
				callProtocolFunction(Signature("disableTimeout()"), Arguments());
		}
		qxt_d().timeoutEnabled = val.toBool();
	}
	else if (prop == "timeout")
	{
		qxt_d().timeout = val.toInt();
		if (qxt_d().timeoutEnabled)
		{
			callProtocolFunction(Signature("enableTimeout(int)"), Arguments() << qxt_d().timeout);
			qxt_d().lastPing = QDateTime::currentDateTime();
			qxt_d().timer.start((qxt_d().timeout / 4));
		}
		else
			callProtocolFunction(Signature("disableTimeout()"), Arguments());
	}
}

/**
 * This function is used for getting the value of arbitrary properties on the Tcp protocol. Available properties are: descriptor, sslmode, protocol, timeoutEnabled, timeout, peerAddress, port, peerPort.
 * @sa setProperty()
 * @param prop Name of the property to get
 * @return Returns the value of \a prop
 */
QVariant ServerProtocolInstanceTcp::getProperty(QString prop)
{
	QMutexLocker locker(mutex());
	if (prop == "descriptor")
		return qxt_d().sd;
	else if (prop == "sslmode")
		return qxt_d().sslmode;
	else if (prop == "certificate")
		return qxt_d().cert;
	else if (prop == "protocol")
		return "tcp";
	else if (prop == "timeoutEnabled")
		return qxt_d().timeoutEnabled;
	else if (prop == "timeout")
		return qxt_d().timeout;
	if (!qxt_d().socket.isNull())
	{
		if (prop == "peerAddress")
			return qxt_d().socket->peerAddress().toString();
		else if (prop == "port")
			return qxt_d().socket->localPort();
		else if (prop == "peerPort")
			return qxt_d().socket->peerPort();
	}
	return QVariant();
}

/**
 * This function is used to initialize the socket on the correct thread. This function should never be called directly, it is only called by the ServerProtocolListenerBase. This function connects the proper signals and slots to get the tco socket working correctly.
 */
void ServerProtocolInstanceTcp::init()
{
	QMutexLocker locker(mutex());
	qxt_d().socket = new QSslSocket();
	connect(qxt_d().socket, SIGNAL(disconnected()), this, SLOT(disconnect()), Qt::QueuedConnection);
	connect(qxt_d().socket, SIGNAL(destroyed()), this, SLOT(disconnect()), Qt::QueuedConnection);
	connect(qxt_d().socket, SIGNAL(sslErrors(QList<QSslError>)), &qxt_d(), SLOT(sslErrors(QList<QSslError>)));
	qxt_d().socket->setSocketDescriptor(qxt_d().sd);
	prepareDevice(qxt_d().socket);
}

/**
 * This function implements the Tcp specific protocol functions. The Tcp protocol functions are: setSsl and ping. This function is used internally be the ServerProtocolInstanceIODevice should never be called directly.
 * @param func The Signature of the protocol function being called.
 * @param args Arguments list for the protocol function
 */
void ServerProtocolInstanceTcp::protocolFunction(Signature func, Arguments args)
{
	if (func.name() == "setSsl")
	{
		if (qxt_d().timeoutEnabled)
		{
			callProtocolFunction(Signature("enableTimeout(int)"), Arguments() << qxt_d().timeout);
			qxt_d().lastPing = QDateTime::currentDateTime();
			qxt_d().timer.start((qxt_d().timeout / 4));
		}
		if (args[0].toBool())
		{
#ifdef QT_NO_OPENSSL
                    callProtocolFunction(Signature("error(int,QString)"), Arguments() << 2 << "Ssl is not supported on this server.");
                    disconnect();
#else
			if (qxt_d().sslmode == ServerProtocolListenerTcp::SslDisabled)
			{
				callProtocolFunction(Signature("error(int,QString)"), Arguments() << 2 << "Ssl is disabled on the server.");
				disconnect();
				return;
			}
			else if (qxt_d().cert.isEmpty() || !QFile::exists(qxt_d().cert))
			{
				qCritical() << "SSL was requested but is not properly configured.";
				callProtocolFunction(Signature("error(int,QString)"), Arguments() << 2 << "Ssl is enabled but has not been configured on this server.");
				disconnect();
				return;
			}
			else
			{
				qxt_d().socket->setLocalCertificate(qxt_d().cert);
				qxt_d().socket->setPrivateKey(qxt_d().cert);
				callProtocolFunction(Signature("enableSsl()"), Arguments());
				connect(qxt_d().socket, SIGNAL(encrypted()), &qxt_d(), SLOT(encrypted()));
				qxt_d().socket->startServerEncryption();
			}
#endif
		}
		else
		{
			if (qxt_d().sslmode == ServerProtocolListenerTcp::SslForced)
			{
				callProtocolFunction(Signature("error(int,QString)"), Arguments() << 2 << "Ssl is forced on the server.");
				disconnect();
				return;
			}
			else
			{
				changeState(Service);
			}
		}
	}
	else if (func.name() == "ping")
	{
		qxt_d().lastPing = QDateTime::currentDateTime();
	}
}

/**
 * This slot is connected to the encrypted signal on the QSslSocket device. This is used to prevent communication from continueing until Ssl is fully initialized on the socket when it is used. This function should never, under any circumstance, be called directly.
 */
void ServerProtocolInstanceTcpPrivate::encrypted()
{
	qxt_p().changeState(ServerProtocolInstanceTcp::Service);
}

#ifndef QT_NO_OPENSSL
/**
 * This slot is called whenever a Ssl error occurs on the socket. It echos the error message and then ignores it.
 * @param errors A list of ssl error messages.
 */
void ServerProtocolInstanceTcpPrivate::sslErrors(QList<QSslError> errors)
{
	foreach(QSslError error, errors)
	{
		qCritical() << "Ssl Error: " << error.errorString();
	}
	socket->ignoreSslErrors();
}
#endif
}
