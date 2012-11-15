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
#include "serverprotocollistenersocket.h"
#include "serverprotocollistenersocket_p.h"

#include <ServerProtocolInstanceSocket>
#include <Server>
#include <ReturnValue>

#include <QFile>

namespace QtRpc
{

/**
 * Constructor
 * @param parent Valid pointer to active Server object. The Server will also be used as the QObject parent.
 */
ServerProtocolListenerSocket::ServerProtocolListenerSocket(Server *parent) : QLocalServer(parent), ServerProtocolListenerBase(parent)
{
	QXT_INIT_PRIVATE(ServerProtocolListenerSocket);
}

/**
 * Overloaded constructor
 * @param serv Valid pointer to the active Server object.
 * @param parent Parent for the QObject
 */
QtRpc::ServerProtocolListenerSocket::ServerProtocolListenerSocket(Server *serv, QObject* parent) : QLocalServer(parent), ServerProtocolListenerBase(serv)
{
	QXT_INIT_PRIVATE(ServerProtocolListenerSocket);
}

/**
 * Deconstructor
 */
ServerProtocolListenerSocket::~ServerProtocolListenerSocket()
{
	close();
}

/**
 * This function is used internally to handle incoming connections. This function should never, under any circumstance, be called directly.
 * @param socketDescriptor The descriptor of the newly opened socket
 */
void QtRpc::ServerProtocolListenerSocket::incomingConnection(quintptr socketDescriptor)
{
	ServerProtocolInstanceSocket *instance = new ServerProtocolInstanceSocket(server());
	instance->setProperty("descriptor", socketDescriptor);
	prepareInstance(instance);
}

/**
 * This function is used for activating the socket listener.
 * @param filename Full path to a file to be used as the socket.
 * @param perm Permissions to be used on the socket.
 * @return Returns true on success, otherwise returns an error.
 */
ReturnValue QtRpc::ServerProtocolListenerSocket::listen(QString filename, SocketPermission perm)
{
	//Remove old socket if it exists
	if (QFile::exists(filename))
	{
		if (!QFile::remove(filename))
		{
			qWarning() << "Unable to remove old socket file" << filename;
		}
	}

	//listen on the socket
	if (!QLocalServer::listen(filename))
	{
		//an error occured
		return(ReturnValue(1, errorString()));
	}

	//set up permissions
	switch (perm)
	{
		case Owner:
			QFile::setPermissions(filename, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
			break;
		case Group:
			QFile::setPermissions(filename, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner
			                      | QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup);
			break;
		case Everyone:
			QFile::setPermissions(filename, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner
			                      | QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup
			                      | QFile::ReadOther | QFile::WriteOther | QFile::ExeOther);
			break;
	}
	return ReturnValue(true);
}

}
