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
#include "clientprotocolbase.h"
#include "clientprotocolbase_p.h"
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

namespace QtRpc
{

/**
 * Sets up some internally used values.
 * @param parent Optional parent for the QObject
 */
ClientProtocolBase::ClientProtocolBase(QObject *parent) : QObject(parent)
{
	QXT_INIT_PRIVATE(ClientProtocolBase);
	qxt_d().connectId = -1;
}

/**
 * The deconstructor emits disconnected() just before going away.
 */
ClientProtocolBase::~ClientProtocolBase()
{
	emit disconnected();
}

QUrl ClientProtocolBase::url() const
{
	return qxt_d().url;
}

void ClientProtocolBase::setUrl(const QUrl& url)
{
	qxt_d().url = url;
}

uint ClientProtocolBase::connectId() const
{
	return qxt_d().connectId;
}

void ClientProtocolBase::setConnectId(uint connectId)
{
	qxt_d().connectId = connectId;
}

uint ClientProtocolBase::selectServiceId() const
{
	return qxt_d().selectServiceId;
}

void ClientProtocolBase::setSelectServiceId(uint selectServiceId)
{
	qxt_d().selectServiceId = selectServiceId;
}

/**
 * This function is used internally to route functions from the ClientMessageBus to the correct place.
 * @param id The id number of the function call
 * @param func The Signature of the function call
 * @param args Arguments list for the function call
 */
void ClientProtocolBase::sendFunction(Message msg)
{
	if (msg.type() == Message::QtRpc)  //if it's a protocol message
	{
		if (msg.signature().name() == QString("%1version").arg(QTRPC_PROTOCOL_PREFIX))
		{
			emit returnReceived(Message(msg.id(), version()));
			return;
		}
		if (msg.signature().name() == QString("%1getProperty").arg(QTRPC_PROTOCOL_PREFIX))
		{
			emit returnReceived(Message(msg.id(), getProperty(msg.arguments()[0].toString())));
			return;
		}
		else if (msg.signature().name() == QString("%1setProperty").arg(QTRPC_PROTOCOL_PREFIX))
		{
			emit returnReceived(Message(msg.id(), setProperty(msg.arguments()[0].toString(), msg.arguments()[1])));
			return;
		}
		else if (msg.signature().name() == QString("%1connect").arg(QTRPC_PROTOCOL_PREFIX))
		{
			qxt_d().url = msg.arguments()[0].toUrl();
			qxt_d().connectId = msg.id();
			protocolConnect();
			return;
		}
		else if (msg.signature().name() == QString("%1disconnect").arg(QTRPC_PROTOCOL_PREFIX))
		{
			protocolDisconnect();
			return;
		}
		else if (msg.signature().name() == QString("%1selectService").arg(QTRPC_PROTOCOL_PREFIX))
		{
			qxt_d().selectServiceId = msg.id();
		}
	}
	function(msg);
}

/**
 * This is the default implementation of the getServiceName function, it simply strips of leading slashes from the path() value of the _url. This function should be reimplemented in the case of a special url syntax.
 * @return Returns the proper name of the Service to use.
 */
QString ClientProtocolBase::getServiceName()
{
	return qxt_d().url.path().replace("/", "");
}

void ClientProtocolBase::moveToThread(QThread* thread)
{
	emit aboutToChangeThreads(thread);
	QObject::moveToThread(thread);
}

quint32 ClientProtocolBase::version()
{
	return qxt_d().version;
}

void ClientProtocolBase::setVersion(quint32 version)
{
	qxt_d().version = version;
}



}
