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
#include "authtoken.h"
#include "authtoken_p.h"
#include <QDebug>
#include <ReturnValue>

using namespace QtRpc;

const AuthToken AuthToken::defaultToken()
{
	AuthToken token;
	token.qxt_d().data->defaultToken = true;
	return token;
}

bool AuthToken::isDefault() const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->defaultToken;
}

AuthTokenData::AuthTokenData(const AuthTokenData &other) :
		clientData(other.clientData),
		serverData(other.serverData)
{
}

AuthTokenPrivate::AuthTokenPrivate() :
		data(QSharedPointer<AuthTokenData>(new AuthTokenData()))
{
}

QReadWriteLock* AuthTokenPrivate::mutex() const
{
	return &data->mutex;
}

AuthToken::AuthToken()
{
	QXT_INIT_PRIVATE(AuthToken);
}

AuthToken::AuthToken(const QString &username, const QString &password)
{
	QXT_INIT_PRIVATE(AuthToken);
	registerMetaTypes();
	qxt_d().data->clientData["username"] = username;
	qxt_d().data->clientData["password"] = password;
}

AuthToken::AuthToken(const AuthToken &other)
{
	QXT_INIT_PRIVATE(AuthToken);
	qxt_d().data = other.qxt_d().data;
}

AuthToken& AuthToken::operator=(const AuthToken & other)
{
	qxt_d().data = other.qxt_d().data;
	return *this;
}

AuthToken::~AuthToken()
{
}

QVariant AuthToken::clientValue(const QString &key, const QVariant &defaultValue) const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->clientData.value(key, defaultValue);
}

QVariant AuthToken::serverValue(const QString &key, const QVariant &defaultValue) const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->serverData.value(key, defaultValue);
}

void AuthToken::clientInsert(const QString &key, const QVariant &value)
{
	QWriteLocker locker(qxt_d().mutex());
	qxt_d().data->clientData.insert(key, value);
}

void AuthToken::serverInsert(const QString &key, const QVariant &value)
{
	QWriteLocker locker(qxt_d().mutex());
	qxt_d().data->serverData.insert(key, value);
}

void AuthToken::clientRemove(const QString &key)
{
	QWriteLocker locker(qxt_d().mutex());
	qxt_d().data->clientData.remove(key);
}

void AuthToken::serverRemove(const QString &key)
{
	QWriteLocker locker(qxt_d().mutex());
	qxt_d().data->serverData.remove(key);
}

bool AuthToken::clientContains(const QString &key) const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->clientData.contains(key);
}

bool AuthToken::serverContains(const QString &key) const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->serverData.contains(key);
}

QVariantMap& AuthToken::clientData()
{
	QWriteLocker locker(qxt_d().mutex());
	return qxt_d().data->clientData;
}

QVariantMap& AuthToken::serverData()
{
	QWriteLocker locker(qxt_d().mutex());
	return qxt_d().data->serverData;
}

const QVariantMap& AuthToken::clientConstData() const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->clientData;
}

const QVariantMap& AuthToken::serverConstData() const
{
	QReadLocker locker(qxt_d().mutex());
	return qxt_d().data->serverData;
}

void AuthToken::detach()
{
	QWriteLocker locker(qxt_d().mutex());
	qxt_d().data = QSharedPointer<AuthTokenData>(new AuthTokenData(*qxt_d().data));
}

void AuthToken::copy(const AuthToken &other)
{
	QWriteLocker locker(qxt_d().mutex());
	QReadLocker otherlocker(other.qxt_d().mutex());
	qxt_d().data->serverData = other.qxt_d().data->serverData;
	qxt_d().data->clientData = other.qxt_d().data->clientData;
}

QVariant& AuthToken::operator[](const QString &str)
{
	QWriteLocker locker(qxt_d().mutex());
	return qxt_d().data->serverData[str];
}

QDebug& operator<<(QDebug &dbg, const AuthToken& auth)
{
	QReadLocker locker(auth.qxt_d().mutex());
	if (auth.qxt_d().data->defaultToken)
		dbg.nospace() << "AuthToken(Default Token)";
	else
		dbg.nospace() << "AuthToken(ClientData (" << auth.qxt_d().data->clientData << ") ServerData (" << auth.qxt_d().data->serverData << "))";
	return dbg.space();
}

QDataStream& operator>> (QDataStream& s, AuthToken& auth)
{
	s >> auth.qxt_d().data->defaultToken >> auth.qxt_d().data->clientData >> auth.qxt_d().data->serverData;
	return s;
}

QDataStream& operator<< (QDataStream& s, const AuthToken& auth)
{
	s << auth.qxt_d().data->defaultToken << auth.qxt_d().data->clientData << auth.qxt_d().data->serverData;
	return s;
}

