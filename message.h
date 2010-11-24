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
#ifndef QTRPCMESSAGE_H
#define QTRPCMESSAGE_H

#include <QxtPimpl>
#include <QList>
#include <QVariant>
#include <QtRpcGlobal>

namespace QtRpc
{
class Message;
class MessagePrivate;
class ReturnValue;
class Signature;
typedef QList<QVariant> Arguments;
}

QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::Message& p);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::Message& p);

namespace QtRpc
{
/**
This class is used by the ClientProtocolIODevice and the ServerProtocolInstanceIODevice to communicate. In most cases, this class will never need to be used outside of those classes. This class allows for a generic object containing basic communications over the IODevice protocol.

All functions in this class are thread safe.

	@brief Generic message object used by the QIODevice protocol
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT Message
{
        friend QTRPC2_EXPORT QDataStream& ::operator>> (QDataStream& s, QtRpc::Message& p);
        friend QTRPC2_EXPORT QDataStream& ::operator<< (QDataStream& s, const QtRpc::Message& p);
	QXT_DECLARE_PRIVATE(Message);
public:
	static quint32 currentVersion();
	/**
	These are the different types of messages.
	*/
	enum Type
	{
		Function,	/**< Function call */
		QtRpc,	/**<  QtRpc internal command */
		Event,	/**<  Emit event*/
		Return,	/**<  ReturnValue for a function call */
		Invalid	/**<  Unknown or invalid type (error) */
	};
	Message();
	Message(const Message&);
	Message(uint id, Type, Signature, Arguments);
	Message(uint id, Type, Signature, Arguments, quint32 service);
	Message(uint id, ReturnValue ret);
	Message operator=(const Message &other);

	quint32 version() const;
	void setVersion(quint32);

	unsigned int id() const;
	void setId(unsigned int);

	Type type() const;
	void setType(Type);

	Signature signature() const;
	void setSignature(const Signature&);

	Arguments arguments() const;
	void setArguments(const Arguments&);

	ReturnValue returnValue() const;
	void setReturnValue(const ReturnValue&);

	quint32 service() const;
	void setService(quint32 id);

	qint64 size() const;

	~Message();
};

}

Q_DECLARE_METATYPE(QtRpc::Message);

QTRPC2_EXPORT QDebug operator<<(QDebug, const QtRpc::Message& ret); //for debugging
QTRPC2_EXPORT QDebug operator<<(QDebug, QtRpc::Message::Type ret); //for debugging

QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::Message& p);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::Message& p);
QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::Message::Type &p);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::Message::Type p);

#endif
