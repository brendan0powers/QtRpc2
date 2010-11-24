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
#ifndef QTRPC_SERVERPROTOCOLLISTENERBASE_H
#define QTRPC_SERVERPROTOCOLLISTENERBASE_H

#include <ServerProtocolInstanceBase>
#include <QxtPimpl>
#include <QtRpcGlobal>

namespace QtRpc
{

class ServerProtocolListenerBasePrivate;
class Server;

/**
	This class is the base of the listener objects. The listener objects are used to watch for incoming connections on their given protocol, and then create an instance object of that protocol.

	Child classes of this should implement their own independant listening method, and should place individual connections into a ServerProtocolInstanceBase child object based on that protocol. To activate that instance object, you must pass it to prepareInstance(). For more details on how to make an instance object, see ServerProtocolInstanceBase.

	Listener objects are automatically associated with whatever Server is passed to the constructor. Server association cannot be changed once it is set.

	@sa Server ServerProtocolInstanceBase
	@brief The base object for the protocol listeners
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ServerProtocolListenerBase
{
	QXT_DECLARE_PRIVATE(ServerProtocolListenerBase);
public:
	ServerProtocolListenerBase(Server *parent);
	~ServerProtocolListenerBase();
protected:
	void prepareInstance(ServerProtocolInstanceBase* instance, QThread* thread = 0);
	Server* server() const;

};

}

#endif
