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
#ifndef QTRPCSERVERPROTOCOLLISTENERTCP_H
#define QTRPCSERVERPROTOCOLLISTENERTCP_H

#include <QTcpServer>
#include <ServerProtocolListenerBase>
#include <QxtPimpl>
#include <QtRpcGlobal>

namespace QtRpc
{

class ServerProtocolListenerTcpPrivate;
class Server;

/**
This is the TCP implementation of the server protocol listener.

	This listener is activated once the listen() function is called, which is a Qt function. Listen takes two parameters, a HostAddress and a port. The host address should be a QHostAddress representing what to listen on, QHostAddress::Any will listen on all available interfaces. The port parameter is simply an integer port number. Example:

	@code
	Server srv();

	ServerProtocolListenerTcp tcp(&srv);
	tcp.listen(QHostAddress::Any,18777);
	@endcode

	You can also arbitrarily enable and disable SSL on incoming connections, using either Enabled, Disabled, or Forced mode. If you choose Enabled or Forced, you must also specify a certificate file to be used, else SSL will not work correctly. If you enable or disable SSL after connections are already made, those connections will not be affected by the change, only future connections will be affected. Example:

	@code

	Server srv();

	ServerProtocolListenerTcp tcp(&srv);
	tcp.setSslMode(ServerProtocolListenerTcp::SslEnabled);
	tcp.setCertificate("/etc/ssl/certs/stunnel.pem");
	tcp.listen(QHostAddress::Any,18777);
	@endcode

	@sa ServerProtocolInstanceTcp ServerProtocolListenerBase
	@brief TCP implementation of the protocol listener.
	@author Chris Vickery <chris@resara.com>
 */
class QTRPC2_EXPORT ServerProtocolListenerTcp : public QTcpServer, public ServerProtocolListenerBase
{
	QXT_DECLARE_PRIVATE(ServerProtocolListenerTcp);
	Q_OBJECT
public:
	enum SslMode
        {
		SslEnabled,
		SslDisabled,
                SslForced
	};
	ServerProtocolListenerTcp(Server *parent);
	ServerProtocolListenerTcp(Server *serv, QObject* parent);
	~ServerProtocolListenerTcp();
	void setSslMode(SslMode);
	SslMode sslMode() const;
	void setCertificate(const QString&);
	QString certificate() const;
protected:
	virtual void incomingConnection(qintptr);



};

}

#endif
