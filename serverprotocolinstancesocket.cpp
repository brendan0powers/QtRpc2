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
#include "serverprotocolinstancesocket.h"
#include "serverprotocolinstancesocket_p.h"

#include <Server>

#include <sys/types.h>
#include <sys/socket.h>

namespace QtRpc
{

/**
 * The constructor sets some default values in preperation for the connection
 * @param serv Initialized pointer to the active Server ocject
 * @param parentOptional parent for the QObject
 */
ServerProtocolInstanceSocket::ServerProtocolInstanceSocket(Server *serv, QObject *parent) : ServerProtocolInstanceIODevice(serv, parent)
{
	QXT_INIT_PRIVATE(ServerProtocolInstanceSocket);
	qxt_d().sd = -1;
	qxt_d().pid = 0;
	qxt_d().uid = 0;
	qxt_d().gid = 0;
}

/**
 * Deconstructor
 */
ServerProtocolInstanceSocket::~ServerProtocolInstanceSocket()
{
}

/**
 * This function is used for getting arbitrary properties on the socket protocol. Available properties are: protocol, descriptor, pid, uid, and gid.
 * @sa setProperty()
 * @param name Name of the property to get
 * @return Returns the value of \a name
 */
QVariant QtRpc::ServerProtocolInstanceSocket::getProperty(QString name)
{
	QMutexLocker locker(&qxt_d().mutex);

	if (name == "protocol")
	{
		return "socket";
	}
	else 	if (name == "descriptor")
	{
		return qxt_d().sd;
	}
	else if (name == "pid")
	{
		return qxt_d().pid;
	}
	else if (name == "uid")
	{
		return qxt_d().uid;
	}
	else if (name == "gid")
	{
		return qxt_d().gid;
	}
	else 	return(QVariant());
}

/**
 * This function is used for setting arbitrary properties on the socket protocol. Available properties are: descriptor.
 * @sa getProperty()
 * @param name Name of the property to set
 * @param val The new value of \a name
 */
void QtRpc::ServerProtocolInstanceSocket::setProperty(QString name, QVariant val)
{
	QMutexLocker locker(&qxt_d().mutex);
	if (name == "descriptor")
	{
		qxt_d().sd = val.toInt();
	}
}

/**
 * This function disconnects from the remote host and cleans up all classes related to the connection, including this one.
 */
void QtRpc::ServerProtocolInstanceSocket::disconnect()
{
	qxt_d().socket->disconnectFromServer();
	deleteLater(); //the service object is a child, so he will be cleaned in due time...
}

/**
 * This function does not do anything, as there are no protocol functions for the socket protocol
 * @param
 * @param
 */
void QtRpc::ServerProtocolInstanceSocket::protocolFunction(Signature, Arguments)
{
	//No protocol functions here because we don't need them
}

/**
 * This function initializes the socket connection and fetches the socket options, like the uid, pid, and gid of the client process. At the end of this function, the state is set to Server, initializing the service selection on the client side.
 */
void QtRpc::ServerProtocolInstanceSocket::init()
{
	QMutexLocker locker(&qxt_d().mutex);
	qxt_d().socket = new QLocalSocket();
	qxt_d().socket->setSocketDescriptor(qxt_d().sd);
	prepareDevice(qxt_d().socket);
	qxt_d().socket->flush();

	struct ucred cr;
	socklen_t cl = sizeof(cr);

	if (getsockopt(qxt_d().sd, SOL_SOCKET, SO_PEERCRED, &cr, &cl) == 0)
	{
		qxt_d().pid = cr.pid;
		qxt_d().uid = cr.uid;
		qxt_d().gid = cr.gid;
	}
	else
	{
		qWarning() << "Failed to get socket optios";
	}

	//Immediatly put things in the service state
	changeState(Service);
}

}
