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
#ifndef QTRPCRETURNVALUE_H
#define QTRPCRETURNVALUE_H

#include <QVariant>
#include <QxtPimpl>
#include <AutomaticMetatypeRegistry>
#include <QtRpcGlobal>

namespace QtRpc
{
class ServiceProxy;
class ReturnValue;
class ReturnValuePrivate;
}
class QDebug;

QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::ReturnValue& p);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::ReturnValue& p);

#ifndef Q_OS_WIN32
QDebug& operator<<(QDebug dbg, const QtRpc::ReturnValue& ret);
#else
QTRPC2_EXPORT QDebug& operator<<(QDebug& dbg, const QtRpc::ReturnValue& ret);
#endif

namespace QtRpc
{

void registerMetaTypes();

/**
	Return value behaves like QVariant in almost all cases. In addition it add a new constructor, and error checking functions. Here is a short example of its use.

	@code
ReturnValue a(12.0);					//A Valid ReturnValue storing a floating point value
ReturnValue b(1,"This is a test error");	//A return value containing an error

if(a.isError())
	qDebug() << "A is an error" << a.errNumber() << a.errString();
else
	qDebug() << "A = " << a;

if(b.isError())
	qDebug() << "B is an error" << b.errNumber() << b.errString();
else
	qDebug() << "B = " << a;
	@endcode

	In this example, you can see that we create two return values, and assign them values. The first ReturnValue is assigned a floating point number, while the seccond one is assigned an error value. After asigning the values, the example checks for errors and prints the results.

	@brief Used for return values of functions.
	@author Chris Vickery <chris@resara.com>
	@author Brendan Powers <brendan@resara.com>
	@sa Signature
*/
class QTRPC2_EXPORT ReturnValue : public QVariant
{
	QXT_DECLARE_PRIVATE(ReturnValue);
	friend QDataStream& ::operator>> (QDataStream& s, QtRpc::ReturnValue& p);
	friend QDataStream& ::operator<< (QDataStream& s, const QtRpc::ReturnValue& p);

#ifndef Q_OS_WIN32
        friend QDebug& ::operator<<(QDebug dbg, const QtRpc::ReturnValue& ret);
#else
        friend QDebug& ::operator<<(QDebug& dbg, const QtRpc::ReturnValue& ret);
#endif
	friend class ServerProtocolInstanceBase;
	friend class ClientProxyPrivate;
public:
	enum Error
	{
		GenericWarning = 0,
		GenericError,
		GenericFatalError,
		PermissionDenied,
		NoExists,
		EverythingIsBroken = 99,
		WeGotHaxed = 1337
	};
	ReturnValue();
	ReturnValue(const QVariant & val);
	ReturnValue(QDataStream & val);
	ReturnValue(int val);
	ReturnValue(uint val);
	ReturnValue(qlonglong val);
	ReturnValue(qulonglong val);
	ReturnValue(bool val);
	ReturnValue(double val);
	ReturnValue(const char * val);
	ReturnValue(const QByteArray & val);
	ReturnValue(const QBitArray & val);
	ReturnValue(const QString & val);
	ReturnValue(const QLatin1String & val);
	ReturnValue(const QStringList & val);
	ReturnValue(const QChar & val);
	ReturnValue(const QDate & val);
	ReturnValue(const QTime & val);
	ReturnValue(const QDateTime & val);
	ReturnValue(const QList<QVariant> & val);
	ReturnValue(const QMap<QString, QVariant> & val);
	ReturnValue(const QSize & val);
	ReturnValue(const QSizeF & val);
	ReturnValue(const QPoint & val);
	ReturnValue(const QPointF & val);
	ReturnValue(const QLine & val);
	ReturnValue(const QLineF & val);
	ReturnValue(const QRect & val);
	ReturnValue(const QRectF & val);
	ReturnValue(const QUrl & val);
	ReturnValue(const QLocale & val);
	ReturnValue(const QRegExp & val);
	ReturnValue(Qt::GlobalColor val);
	/**
	 * Creates a ReturnValue with an error
	 * @param num The number of the error, The value of the number doesn't matter.
	 * @param str The user readable string for the error.
	 */
	ReturnValue(int num, QString str);
	ReturnValue(QtRpc::ServiceProxy* srv);

	ReturnValue(const ReturnValue& other);
	ReturnValue& operator=(const ReturnValue& other);

	~ReturnValue();

	bool isError() const;
	uint errNumber() const;
	QString errString() const;

	bool isService() const;
	bool isAsyncronous() const;
	void setServiceId(quint32 serviceId);
	quint32 serviceId() const;
	ServiceProxy* service() const;

	static ReturnValue asyncronous();
};

}

using QtRpc::ReturnValue;

Q_DECLARE_METATYPE(QtRpc::ReturnValue);
Q_DECLARE_TYPEINFO(QtRpc::ReturnValue, Q_MOVABLE_TYPE);

#ifndef Q_OS_WIN32
QDebug& operator<<(QDebug dbg, QtRpc::ReturnValue::Error ret);
#else
QTRPC2_EXPORT QDebug& operator<<(QDebug& dbg, QtRpc::ReturnValue::Error ret);
#endif

QTRPC2_EXPORT QDataStream& operator>> (QDataStream& s, QtRpc::ReturnValue::Error& p);
QTRPC2_EXPORT QDataStream& operator<< (QDataStream& s, const QtRpc::ReturnValue::Error p);


#endif
