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
#ifndef QTRPCSIGNATURE_H
#define QTRPCSIGNATURE_H

#include <QxtPimpl>
#include <QVariant>
#include <QVector>
#include <QtRpcGlobal>

class QString;
class QStringList;

namespace QtRpc
{
	class Signature;
	class SignaturePrivate;
}

QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::Signature& p);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::Signature& p);

namespace QtRpc
{

/**
	This class is used to create and test function signatures. It ensures that all function signatures are in the same format. In its simplest form, you create a signature by passing a string to its constructor, and retrieve a string representation of the signature by using  toString()

	There are also the name(), numArgs(), and arg() function to retrieve a certain part of the signature, and the setName(), setArg(), and setArgs() functions to modify parts of the signature.

	The test(), and validate() functions are used to make sure the arguments are valid.

	@brief Holds information about a function, and allowes testing of argument lists.
	@author Brendan Powers <brendan@resara.com>
	@author Chris Vickery <chris@resara.com>
	@sa ReturnValue
 */
class QTRPC2_EXPORT Signature
{
	QXT_DECLARE_PRIVATE(Signature);
public:
	Signature();
	Signature(const Signature& other);
	Signature(QString sig);
	Signature(const char* sig);

	~Signature();
	bool parse(const QString& sig);
	QString name() const;
	void setName(const QString& n);
	QString toString() const;
	int numArgs() const;
	void setNumArgs(int num);
	QString arg(int num) const;
	bool setArg(int num, QString value);
	QString test(const QVariantList& list) const;
	QString test(QVariant arg1 = QVariant(), QVariant arg2 = QVariant(), QVariant arg3 = QVariant(), QVariant arg4 = QVariant(), QVariant arg5 = QVariant(), QVariant arg6 = QVariant(), QVariant arg7 = QVariant(), QVariant arg8 = QVariant(), QVariant arg9 = QVariant(), QVariant arg10 = QVariant()) const;
	bool operator==(const Signature &sig) const;
	QVector<QString> args() const;
	void setArgs(const QVector<QString>&);
	bool validate() const;
	QtRpc::Signature& operator=(const Signature& other);

private:
	friend QTRPC2_EXPORT QDataStream& ::operator>> (QDataStream& s, Signature& p);
	friend QTRPC2_EXPORT QDataStream& ::operator<< (QDataStream& s, const Signature& p);
};

typedef QList<QVariant> Arguments;

}

QTRPC2_EXPORT QDebug operator<<(QDebug, const QtRpc::Signature& sig);

Q_DECLARE_METATYPE(QtRpc::Signature);
Q_DECLARE_METATYPE(QList<QtRpc::Signature>);


#endif
