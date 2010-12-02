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
#include <QCoreApplication>
#include <QVariant>
#include <AuthToken>
#include <Server>
#include <ServerProtocolListenerTcp>
#include <ServerProtocolListenerSocket>
#include <ServerProtocolListenerProcess>
#include <ServicePublisher>
#include "testserver.h"
#include "callerservice.h"
#include "testsyncroserver.h"


using namespace QtRpc;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	Server srv(&app,Server::SingleThread);
	ServiceProxy* service = srv.registerService<TestServer>("TestService");
	srv.registerService<CallerService>("CallerService");
	srv.registerService<CallerService>("StringService");
	
	AuthToken auth;
	QVariant var = QVariant::fromValue(auth);
	auth = AuthToken(var.value<AuthToken>());
	qDebug() << auth.clientData();
	
	ServerProtocolListenerProcess proclist(&srv);
	if(!proclist.listen())
		qDebug() << "Not a fork";
	
	ServerProtocolListenerTcp tcp(&srv);
	tcp.setSslMode(ServerProtocolListenerTcp::SslEnabled);
	tcp.setCertificate("/etc/ssl/certs/stunnel.pem");
	qDebug() << tcp.listen(QHostAddress::Any,18777);
	
	ServerProtocolListenerSocket socket(&srv);
	socket.listen("/tmp/qtrpc-socket", ServerProtocolListenerSocket::Everyone);
	return app.exec();
}
