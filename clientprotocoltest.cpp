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
#include "clientprotocoltest.h"
#include "clientprotocoltest_p.h"

namespace QtRpc
{

ClientProtocolTest::ClientProtocolTest(QObject *parent) : ClientProtocolBase(parent)
{
	QXT_INIT_PRIVATE(ClientProtocolTest);
}

ClientProtocolTest::~ClientProtocolTest()
{
}

void ClientProtocolTest::function(Message msg)
{
	if (msg.signature().name() == "add")
	{
		emit returnReceived(Message(msg.id(), ReturnValue("a9v757y46u2358n35b7ij")));
	}
	else
	{
		emit returnReceived(Message(msg.id(), ReturnValue("This is a return value for an RPC call")));
	}
}

ReturnValue ClientProtocolTest::setProperty(QString, QVariant)
{
	return true;
}

ReturnValue ClientProtocolTest::getProperty(QString name)
{
	if(name=="protocolType")
		return "test";
	return ReturnValue(10,QString("Unknown property: %1").arg(name));
}

void ClientProtocolTest::protocolConnect()
{
}

ReturnValue ClientProtocolTest::protocolDisconnect()
{
	return true;
}

bool ClientProtocolTest::isConnected()
{
	return true;
}

}
