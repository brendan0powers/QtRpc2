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
#ifndef QTRPCSERVERPROTOCOLLISTENERSOCKET_H
#define QTRPCSERVERPROTOCOLLISTENERSOCKET_H

#include <QLocalServer>
#include <ServerProtocolListenerBase>
#include <QxtPimpl>
#include <QtRpcGlobal>

namespace QtRpc
{

class ServerProtocolListenerSocketPrivate;
class ReturnValue;
class Server;

/**
This is the socket implementation of the protocol listener. The only significant function in this class is the "listen" function.

	@sa ServerProtocolInstanceSocket ServerProtocolListenerBase
	@brief Socket based protocol listener.
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ServerProtocolListenerSocket : public QLocalServer, public ServerProtocolListenerBase
{
	QXT_DECLARE_PRIVATE(ServerProtocolListenerSocket);
	Q_OBJECT
public:
	/**
	 * This enum represents the permissions settings to be used on the socket.
	 */
	enum SocketPermission
	{
		Owner,	/**< Owner only */
		Group,	/**< Group only */
		Everyone	/**< Everyone */
	};

	ServerProtocolListenerSocket(Server *parent);
	ServerProtocolListenerSocket(Server *serv, QObject* parent);

	~ServerProtocolListenerSocket();
	ReturnValue listen(QString filename, SocketPermission perm = Owner);

protected:
	void incomingConnection(quintptr socketDescriptor);

};

}

#endif
