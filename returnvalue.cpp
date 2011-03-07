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
#include "returnvalue.h"
#include "returnvalue_p.h"
#include "serviceproxy_p.h"

#include <QDebug>
#include <ServiceFinder>

namespace QtRpc
{

bool qtrpc_metatypes_registered = false;
void registerMetaTypes()
{
	if (!qtrpc_metatypes_registered)
	{
		qRegisterMetaTypeStreamOperators<QtRpc::ReturnValue>("QtRpc::ReturnValue");
		qRegisterMetaType<QtRpc::ReturnValue>("QtRpc::ReturnValue");
		qRegisterMetaTypeStreamOperators<ReturnValue>("ReturnValue");
		qRegisterMetaType<ReturnValue>("ReturnValue");

		qRegisterMetaType<QVariant>("QVariant");

		qRegisterMetaType<QtRpc::AuthToken>("QtRpc::AuthToken");
		qRegisterMetaTypeStreamOperators<QtRpc::AuthToken>("QtRpc::AuthToken");
		qRegisterMetaType<QtRpc::AuthToken>("AuthToken");
		qRegisterMetaTypeStreamOperators<QtRpc::AuthToken>("AuthToken");

		qRegisterMetaTypeStreamOperators<QtRpc::Signature>("QtRpc::Signature");
		qRegisterMetaType<QtRpc::Signature>("QtRpc::Signature");
		qRegisterMetaTypeStreamOperators<Signature>("Signature");
		qRegisterMetaType<Signature>("Signature");
		qRegisterMetaTypeStreamOperators<QList<QtRpc::Signature> >("QList<QtRpc::Signature>");
		qRegisterMetaType<QList<Signature> >("QList<QtRpc::Signature>");
		qRegisterMetaTypeStreamOperators<QList<Signature> >("QList<Signature>");
		qRegisterMetaType<QList<Signature> >("QList<Signature>");

		qRegisterMetaType<QtRpc::Arguments>("QtRpc::Arguments");
		qRegisterMetaType<Arguments>("Arguments");

		qRegisterMetaType<Message>("QtRpc::Message");
		qRegisterMetaType<Message>("Message");

		qRegisterMetaType<ServiceFinder::Service>("ServiceFinder::Service");
		qtrpc_metatypes_registered = true;
	}
}

ReturnValueData::ReturnValueData() :
		type(Variant),
		number(0),
		rawServicePointer(0),
		serviceId(-1)
{
}

#define DEFAULT_CONSTRUCTOR(x) \
	ReturnValue::ReturnValue(x val) : QVariant(val) \
	{ \
		QXT_INIT_PRIVATE(ReturnValue); \
		qxt_d().data = new ReturnValueData(); \
		qxt_d().data.data()->type = ReturnValueData::Variant; \
	}

ReturnValue::ReturnValue() : QVariant()
{
	QXT_INIT_PRIVATE(ReturnValue);
	qxt_d().data = new ReturnValueData();
	qxt_d().data.data()->type = ReturnValueData::Variant;
}

/*!
    \fn QtRpc::ReturnValue::ReturnValue(int num, QString str)
 */

QtRpc::ReturnValue::ReturnValue(int num, QString str)
{
	QXT_INIT_PRIVATE(ReturnValue);
	qxt_d().data = new ReturnValueData();
	qxt_d().data.data()->type = ReturnValueData::Error;
	qxt_d().data.data()->number = num;
	qxt_d().data.data()->string = str;
}

QtRpc::ReturnValue::ReturnValue(QtRpc::ServiceProxy* srv)
{
	QXT_INIT_PRIVATE(ReturnValue);
	qxt_d().data = new ReturnValueData();
	qxt_d().data.data()->type = ReturnValueData::Service;
	qxt_d().data.data()->service = srv->qxt_d().weakPointer;
	qxt_d().data.data()->rawServicePointer = srv;
}

/*!
    \fn QtRpc::ReturnValue::ReturnValue(const ReturnValue &r)
 */
QtRpc::ReturnValue::ReturnValue(const ReturnValue& other) : QVariant(other)
{
	QXT_INIT_PRIVATE(ReturnValue);
	qxt_d().data = other.qxt_d().data;
}

ReturnValue::~ReturnValue()
{
}

/**
 * Check weather the ReturnValue contains an error
 * @return Returns true if there is an error, false if there is not.
 */
bool ReturnValue::isError() const
{
	return(qxt_d().data.constData()->type == ReturnValueData::Error);
}

/**
 * @return Returns the error number. The value if this function is undefined if there is no error. You hsould run isError() first.
 * @sa isError()
 */
uint ReturnValue::errNumber() const
{
	return(qxt_d().data.constData()->number);
}

/**
 * @return Returns the error string. The value if this function is undefined if there is no error. You hsould run isError() first.
 * @sa isError()
 */
QString ReturnValue::errString() const
{
	return(qxt_d().data.constData()->string);
}

bool ReturnValue::isService() const
{
	return(qxt_d().data.constData()->type == ReturnValueData::Service);
}

bool ReturnValue::isAsyncronous() const
{
	return(qxt_d().data.constData()->type == ReturnValueData::Asyncronous);
}

void ReturnValue::setServiceId(quint32 serviceId)
{
	qxt_d().data->type = ReturnValueData::Service;
	qxt_d().data->serviceId = serviceId;
}

quint32 ReturnValue::serviceId() const
{
	return qxt_d().data->serviceId;
}

ServiceProxy* ReturnValue::service() const
{
	return qxt_d().data->rawServicePointer;
}

ReturnValue ReturnValue::asyncronous()
{
	ReturnValue ret;
	ret.qxt_d().data->type = ReturnValueData::Asyncronous;
	return ret;
}

ReturnValue& ReturnValue::operator=(const ReturnValue& other)
{
	qxt_d().data = other.qxt_d().data;
	QVariant::operator=(other);
	return *this;
}

DEFAULT_CONSTRUCTOR(const QVariant &)
DEFAULT_CONSTRUCTOR(QDataStream &)
DEFAULT_CONSTRUCTOR(int)
DEFAULT_CONSTRUCTOR(uint)
DEFAULT_CONSTRUCTOR(qlonglong)
DEFAULT_CONSTRUCTOR(qulonglong)
DEFAULT_CONSTRUCTOR(bool)
DEFAULT_CONSTRUCTOR(double)
DEFAULT_CONSTRUCTOR(const char *)
DEFAULT_CONSTRUCTOR(const QByteArray &)
DEFAULT_CONSTRUCTOR(const QBitArray &)
DEFAULT_CONSTRUCTOR(const QString &)
DEFAULT_CONSTRUCTOR(const QLatin1String &)
DEFAULT_CONSTRUCTOR(const QStringList &)
DEFAULT_CONSTRUCTOR(const QChar &)
DEFAULT_CONSTRUCTOR(const QDate &)
DEFAULT_CONSTRUCTOR(const QTime &)
DEFAULT_CONSTRUCTOR(const QDateTime &)
DEFAULT_CONSTRUCTOR(const QList<QVariant> &)
#define tmpval const QMap<QString, QVariant> &
DEFAULT_CONSTRUCTOR(tmpval)
#undef tmpval //sorry...
DEFAULT_CONSTRUCTOR(const QSize &)
DEFAULT_CONSTRUCTOR(const QSizeF &)
DEFAULT_CONSTRUCTOR(const QPoint &)
DEFAULT_CONSTRUCTOR(const QPointF &)
DEFAULT_CONSTRUCTOR(const QLine &)
DEFAULT_CONSTRUCTOR(const QLineF &)
DEFAULT_CONSTRUCTOR(const QRect &)
DEFAULT_CONSTRUCTOR(const QRectF &)
DEFAULT_CONSTRUCTOR(const QUrl &)
DEFAULT_CONSTRUCTOR(const QLocale &)
DEFAULT_CONSTRUCTOR(const QRegExp &)
DEFAULT_CONSTRUCTOR(Qt::GlobalColor)

}

QDebug operator<<(QDebug dbg, const ReturnValue& ret)
{
	dbg.nospace() << "ReturnValue(";
	switch (ret.qxt_d().data->type)
	{
		case QtRpc::ReturnValueData::Service:
			dbg << "Service: " << ret.serviceId();
			break;
		case QtRpc::ReturnValueData::Error:
			dbg << static_cast<QtRpc::ReturnValue::Error>(ret.errNumber()) << ": " << ret.errString();
			break;
		case QtRpc::ReturnValueData::Variant:
		default:
			dbg << ret.typeName() << ", ";
			ReturnValue::handler->debugStream(dbg, ret);
			break;
	}
	dbg << ')';
	return dbg.space();
}

QDebug operator<<(QDebug dbg, QtRpc::ReturnValue::Error err)
{
	switch (err)
	{
		case QtRpc::ReturnValue::GenericWarning:
			dbg << "Generic warning";
			break;
		case QtRpc::ReturnValue::GenericError:
			dbg << "Generic error";
			break;
		case QtRpc::ReturnValue::GenericFatalError:
			dbg << "Generic fatal error";
			break;
		case QtRpc::ReturnValue::PermissionDenied:
			dbg << "Permission denied";
			break;
		case QtRpc::ReturnValue::NoExists:
			dbg << "Does not exist";
			break;
		case QtRpc::ReturnValue::EverythingIsBroken:
			dbg << "Everything is broken";
			break;
		case QtRpc::ReturnValue::WeGotHaxed:
			dbg << "0mg we ttly gotz h4xxord!";
			break;
		default:
			dbg << "Error code " << static_cast<qint32>(err);
			break;
	}
	return dbg;
}

QDataStream& operator>> (QDataStream& s, QtRpc::ReturnValue& p)
{
	quint32 type;
	s >> type;
	p.qxt_d().data->type = static_cast<QtRpc::ReturnValueData::ReturnValueType>(type);
	switch (p.qxt_d().data->type)
	{
		case QtRpc::ReturnValueData::Service:
			s >> p.qxt_d().data->serviceId;
			break;
		case QtRpc::ReturnValueData::Error:
			s >> p.qxt_d().data->number;
			s >> p.qxt_d().data->string;
			break;
		case QtRpc::ReturnValueData::Variant:
		default:
			s >> static_cast<QVariant&>(p);
			break;
	}
	return s;
}

QDataStream& operator<< (QDataStream& s, const QtRpc::ReturnValue& p)
{
	s << static_cast<quint32>(p.qxt_d().data.constData()->type);
	switch (p.qxt_d().data.constData()->type)
	{
		case QtRpc::ReturnValueData::Service:
			s << p.qxt_d().data.constData()->serviceId;
			break;
		case QtRpc::ReturnValueData::Error:
			s << p.qxt_d().data.constData()->number;
			s << p.qxt_d().data.constData()->string;
			break;
		case QtRpc::ReturnValueData::Variant:
		default:
			s << static_cast<const QVariant&>(p);
			break;
	}
	return s;
}

QDataStream& operator>> (QDataStream& s, QtRpc::ReturnValue::Error& p)
{
	int pt;
	s >> pt;
	p = (QtRpc::ReturnValue::Error)pt;
	return s;
}

QDataStream& operator<< (QDataStream& s, const QtRpc::ReturnValue::Error p)
{
	s << (int)p;
	return s;
}

