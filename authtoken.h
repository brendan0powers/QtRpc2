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
#ifndef QTRPCAUTHTOKEN_H
#define QTRPCAUTHTOKEN_H

#include <QMetaType>
#include <QxtPimpl>
#include <QVariant>
#include <QtRpcGlobal>

namespace QtRpc
{
class AuthTokenPrivate;
class AuthToken;
}
class QDebug;
class QString;

QTRPC2_EXPORT QDebug& operator<<(QDebug& dbg, const QtRpc::AuthToken& auth); //for debugging

QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::AuthToken& auth);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::AuthToken& auth);

namespace QtRpc
{
/**
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT AuthToken
{
	QXT_DECLARE_PRIVATE(AuthToken);
        friend QTRPC2_EXPORT QDebug& ::operator<<(QDebug& dbg, const QtRpc::AuthToken& ret);
        friend QTRPC2_EXPORT QDataStream& ::operator>> (QDataStream& s, QtRpc::AuthToken& auth);
        friend QTRPC2_EXPORT QDataStream& ::operator<< (QDataStream& s, const QtRpc::AuthToken& auth);
public:
	static const AuthToken defaultToken();
	bool isDefault() const;

	AuthToken();
	AuthToken(const QString &username, const QString &password);
	AuthToken(const AuthToken &other);
	~AuthToken();

	QVariant clientValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
	QVariant serverValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
	void clientInsert(const QString &key, const QVariant &value);
	void serverInsert(const QString &key, const QVariant &value);
	void clientRemove(const QString &key);
	void serverRemove(const QString &key);
	bool clientContains(const QString &key) const;
	bool serverContains(const QString &key) const;
	QVariantMap& clientData();
	QVariantMap& serverData();
	const QVariantMap& clientConstData() const;
	const QVariantMap& serverConstData() const;

	void detach();
	void copy(const AuthToken &other);

	QVariant& operator[](const QString &str);
	AuthToken& operator=(const AuthToken &other);
};

}

Q_DECLARE_METATYPE(QtRpc::AuthToken);

#endif

