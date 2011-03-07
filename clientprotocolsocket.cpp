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
#include "clientprotocolsocket.h"
#include "clientprotocolsocket_p.h"
#include <QUrl>

namespace QtRpc
{

/**
	 * The constructor calls the ClientProtocolIODevice::prepareDevice() function to initialize the socket for use.
 * @param parent Optional parent for the QObject
 */
ClientProtocolSocket::ClientProtocolSocket(QObject *parent)
		: ClientProtocolIODevice(parent)
{
	QXT_INIT_PRIVATE(ClientProtocolSocket);
	prepareDevice(&qxt_d().socket);
	connect(&qxt_d().socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

/**
 * Deconstructor
 */
ClientProtocolSocket::~ClientProtocolSocket()
{
}

/**
 * The Socket implementation of the setProperty function. This function does not do anything as there are no client side Socket properties.
 * @sa getProperty
 * @return This function will always return true.
 */
ReturnValue ClientProtocolSocket::setProperty(QString, QVariant)
{
	return true;
}

/**
 * The Socket implentation of the getProperty function. This function does not do anything as there are no client side Socket properties.
 * @sa setProperty
 * @return This function will always return true.
 */
ReturnValue ClientProtocolSocket::getProperty(QString)
{
	return true;
}

/**
 * This function initializes the connection with the server, returning an error on failure.
 * @todo This function should be fixed so that the "|" no longer needs to be used in the URLs...
 */
void ClientProtocolSocket::protocolConnect()
{
	int index = url().path().indexOf(':');
	if (index == -1)
		qxt_d().socket.connectToServer(url().path());
	else
		qxt_d().socket.connectToServer(url().path().left(index));
	if (!qxt_d().socket.waitForConnected())
	{
		emit returnReceived(Message(connectId(), ReturnValue(1, "Failed to connect to socket " + qxt_d().socket.errorString())));
	}
}

/**
 * This function disconnects the socket from the server, emiting disconnected().
 * @return This function always returns true
 */
ReturnValue ClientProtocolSocket::protocolDisconnect()
{
	if (qxt_d().socket.state() != QLocalSocket::UnconnectedState)
	{
		qxt_d().socket.disconnectFromServer();
		emit disconnected();
	}
	return true;
}

bool ClientProtocolSocket::isConnected()
{
	return (qxt_d().socket.state() != QLocalSocket::UnconnectedState);
}

/**
 * This function handles protocol specific functions, through Socket has no protocol specific functions so it doesn't actually do anything.
 */
void ClientProtocolSocket::protocolFunction(Signature, Arguments)
{
}

/**
 * This function is called when the server's state is Connecting. The Socket protocol has nothing in this step, so this function does nothing. The state immediately goes to Service.
 */
void ClientProtocolSocket::connected()
{
}

/**
 * Retrieves the service name from the url. socket:///tmp/socket:servicename, where servicename is the name of the service
 * @return Return the name of the service
 */
QString QtRpc::ClientProtocolSocket::getServiceName()
{
	QString path = url().path();
	int index = path.indexOf(':');
	if (index == -1) return QString();
	return(path.right(path.size() - (index + 1)));
}

}
