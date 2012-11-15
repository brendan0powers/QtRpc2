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
#include "clientprotocoltcp.h"
#include "clientprotocoltcp_p.h"
#include <QUrl>
#include <QtCore/qconfig.h>
#ifdef QT_NO_OPENSSL
#define QSslSocket QTcpSocket
#endif
namespace QtRpc
{

/**
	 * The constructor initializes the socket and the timeout timer and connects all the needed signals and slots. The constructor also calls ClientProtocolIODevice::prepareDevice() to prepare the socket for use.
 * @param parent Optional parent for the QObject
 */
ClientProtocolTcp::ClientProtocolTcp(QObject *parent) : ClientProtocolIODevice(parent)
{
	QXT_INIT_PRIVATE(ClientProtocolTcp);
	qxt_d().timer.setInterval(10000);
	connect(&qxt_d().timer, SIGNAL(timeout()), &qxt_d(), SLOT(ping()));
#ifndef QT_NO_OPENSSL
	connect(&qxt_d().socket, SIGNAL(sslErrors(QList<QSslError>)), &qxt_d().socket, SLOT(ignoreSslErrors()));
#endif
	connect(&qxt_d().socket, SIGNAL(disconnected()), &qxt_d().timer, SLOT(stop()));
	connect(&qxt_d().socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
	qxt_d().lastPing = QDateTime::currentDateTime();
	prepareDevice(&qxt_d().socket);
}

/**
 * Deconstructor
 */
ClientProtocolTcp::~ClientProtocolTcp()
{
}

/**
 * This function is used internally to handle pinging the server and detecting timeouts. Timeouts are enabled and disabled server side.
 */
void ClientProtocolTcpPrivate::ping()
{
	if (lastPing.secsTo(QDateTime::currentDateTime()) > timeout)
	{
		qCritical() << "Remove host has timed out.";
		qxt_p().protocolDisconnect();
		return;
	}
	qxt_p().callProtocolFunction(Signature("ping()"), Arguments());
}

/**
 * The TCP implementation of the setProperty function. This function does not do anything as there are no client side TCP properties.
 * @sa getProperty
 * @return This function will always return true.
 */
ReturnValue ClientProtocolTcp::setProperty(QString, QVariant)
{
	return true;
}

/**
 * The TCP implentation of the getProperty function. This function does not do anything as there are no client side TCP properties.
 * @sa setProperty
 * @return This function will always return true.
 */
ReturnValue ClientProtocolTcp::getProperty(QString)
{
	return true;
}

/**
 * This function is called by it's parent classes to initialize the connection with the server. It connects to the specified URL, reporting an error if the connection fails. The ClientProtocolIODevice handles most of the rest of the connecting process.
 */
void ClientProtocolTcp::protocolConnect()
{
	qxt_d().lastPing = QDateTime::currentDateTime();
	qxt_d().socket.connectToHost(url().host(), url().port());
	if (!qxt_d().socket.waitForConnected())
	{
		emit returnReceived(Message(connectId(), ReturnValue(1, QString("Failed to connect: %1").arg(qxt_d().socket.errorString()))));
	}
}

/**
 * This function disconnects from the server, emiting disconnected.
 * @return This function always returns true.
 */
ReturnValue ClientProtocolTcp::protocolDisconnect()
{
	if (qxt_d().socket.state() != QAbstractSocket::UnconnectedState)
	{
		qxt_d().socket.disconnectFromHost();
		emit disconnected();
	}
	return true;
}

bool ClientProtocolTcp::isConnected()
{
	return (qxt_d().socket.state() != QAbstractSocket::UnconnectedState);
}

/**
 * This function is for receiving protocol specific functions. This function is used internally by the ClientProtocolIODevice class.

The TCP specific protocol functions are ping(), enableTimeout(int), disableTimeout(), and enableSsl()

 * @param func The Signature of the protocol function
 * @param args Arguments list for the protocol function
 */
void ClientProtocolTcp::protocolFunction(Signature func, Arguments args)
{
	if (func.name() == "ping")
	{
		qxt_d().lastPing = QDateTime::currentDateTime();
	}
	else if (func.name() == "enableTimeout")
	{
		qxt_d().timeout = args[0].toInt();
		qxt_d().timer.start((qxt_d().timeout / 4));
	}
	else if (func.name() == "disableTimeout")
	{
		qxt_d().timer.stop();
	}
	else if (func.name() == "enableSsl")
	{
            disconnect();
#ifndef QT_NO_OPENSSL
		qxt_d().socket.startClientEncryption();
#endif
	}
}

/**
 * This function is reimplemented from the ClientProtocolIODevice class, and is called between connection and service selecting. It tells the server if it wants to enable SSL or not.
 */
void ClientProtocolTcp::connected()
{
	qxt_d().lastPing = QDateTime::currentDateTime();
	if (url().scheme() == "tcp")
	{
		callProtocolFunction(Signature("setSsl(bool)"), Arguments() << false);
	}
	else if (url().scheme() == "tcps")
	{
		callProtocolFunction(Signature("setSsl(bool)"), Arguments() << true);
// 		qxt_d().socket.startClientEncryption();
// 		qxt_d().socket.waitForEncrypted();
	}
	else
	{
		qCritical(qPrintable(QString("Error: Tcp does not support protocol: %1").arg(url().scheme())));
		emit returnReceived(Message(connectId(), ReturnValue(1, QString("Error: Tcp does not support protocol: %1").arg(url().scheme()))));
	}
}

}
