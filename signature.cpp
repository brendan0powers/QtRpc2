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
#include "signature.h"
#include "signature_p.h"

#include <QRegExp>
#include <QDebug>
QtRpc::Signature::Signature()
{
	QXT_INIT_PRIVATE(Signature);
	qxt_d().data = new SignatureData();
}

QtRpc::Signature::Signature(const QtRpc::Signature& other)
{
	QXT_INIT_PRIVATE(Signature);
	qxt_d().data = other.qxt_d().data;
}

/**
 * Creates a signature with the string \a sig.
 * @param sig The string representing the signature being created.
 */
QtRpc::Signature::Signature(QString sig)
{
	QXT_INIT_PRIVATE(Signature);
	qxt_d().data = new SignatureData();
	parse(sig);
}
QtRpc::Signature::Signature(const char* sig)
{
	QXT_INIT_PRIVATE(Signature);
	qxt_d().data = new SignatureData();
	if (sig != 0 && sig[0] == '1')
	{
		parse(QString(sig).mid(1));
	}
	else
		parse(sig);
}

QtRpc::Signature& QtRpc::Signature::operator=(const QtRpc::Signature & other)
{
	qxt_d().data = other.qxt_d().data;
	return *this;
}


QtRpc::Signature::~Signature()
{
}



/**
 * Parses a string, and sets the Signatures name and arguments acordingly
 * @param sig The string represing the signature to be parsed
 * @return returns true if the string was valid, false if otherwise
 */
bool QtRpc::Signature::parse(const QString& sig)
{
	qxt_d().data->args.clear(); //clear existing arguments list

// 	QRegExp exp("^[^)]+[(].*[)]$");
// 	if (!exp.exactMatch(sig)) return(false);
	if (sig.count('(') != 1)
		return false;
	if (sig.count(')') != 1)
		return false;

	//get the name
	int index = sig.indexOf('(');
	int index2 = sig.indexOf(')');
	if (index < 1 || index2 != (sig.count() - 1)) return(false);
	qxt_d().data->name = sig.left(index);

	//get the args
	QString args = sig.mid(index + 1, index2 - index - 1);

	//If there are no arguments, just return
	if (args == "")
		return(true);

	//split args and add them to the list
	QStringList tmp = args.split(",");
	foreach(QString arg, tmp)
	{
		qxt_d().data->args.append(arg.trimmed());
	}

	return(true);
}


/**
 * @return Returns the name of the signatue
 */
QString QtRpc::Signature::name() const
{
	return(qxt_d().data->name);
}

/**
 * Sets the name of the signature
 * @param n The name of the signature.
 */
void QtRpc::Signature::setName(const QString& n)
{
	qxt_d().data->name = n;
}

/**
 * @return Returns a string representing the signature.
 */
QString QtRpc::Signature::toString() const
{
	return(QString("%1(%2)").arg(qxt_d().data->name).arg(QStringList(qxt_d().data->args.toList()).join(",")));
}

/**
 * @return Returns the number of arguments
 */
int QtRpc::Signature::numArgs() const
{
	return(qxt_d().data->args.size());
}

/**
 * Sets the number of arguments
 * @param num The number of arguments
 */
void QtRpc::Signature::setNumArgs(int num)
{
	qxt_d().data->args.resize(num);
}

/**
 * Return arg number /a num from the signature
 * @param num The argument number to return
 * @return Returns the variable type of the argument
 */
QString QtRpc::Signature::arg(int num) const
{
	if (num < qxt_d().data->args.size()) return(qxt_d().data->args[num]);
	else return(QString());
}

/**
 * Sets the argument at number \a num to \a value.
 * @param num The number of the argument to set
 * @param value The value of the argument
 * @return Returns true if the argument exists, false if not
 */
bool QtRpc::Signature::setArg(int num, QString value)
{
	if (num < qxt_d().data->args.size())
	{
		qxt_d().data->args[num] = value;
		return(true);
	}
	return(false);
}

/**
 * Tests the given list of QVariants to see if they match the value type of the arguments
 * @param list The list of argument to test
 * @return Returns a null string if successful, an error string if not.
 */
QString QtRpc::Signature::test(const QVariantList& list) const
{
	if (qxt_d().data->args.size() != list.size()) return(QString("the number of arguments is %1, it should be %2").arg(list.size()).arg(qxt_d().data->args.size()));
	int i = 0;
	foreach(QVariant v, list)
	{
		if (QMetaType::type(v.typeName()) != QMetaType::type(qPrintable(qxt_d().data->args[i])))
		{
			if (qxt_d().data->args[i] != "QVariant")
			{
				return(QString("argument %1 is %2, it should be %3 (%4 %5)").arg(i).arg(v.typeName()).arg(qxt_d().data->args[i]).arg(QMetaType::type(v.typeName())).arg(QMetaType::type(qPrintable(qxt_d().data->args[i]))));
			}
		}
		i++;
	}

	return(QString()); //on success return a null string
}


/**
 * @overload QString test(QVariantList)
 */
QString QtRpc::Signature::test(QVariant arg1, QVariant arg2, QVariant arg3, QVariant arg4, QVariant arg5, QVariant arg6, QVariant arg7, QVariant arg8, QVariant arg9, QVariant arg10) const
{
	QVariantList list;
	int num = qxt_d().data->args.size();
	if (num >= 1) list << arg1;
	if (num >= 2) list << arg2;
	if (num >= 3) list << arg3;
	if (num >= 4) list << arg4;
	if (num >= 5) list << arg5;
	if (num >= 6) list << arg6;
	if (num >= 7) list << arg7;
	if (num >= 8) list << arg8;
	if (num >= 9) list << arg9;
	if (num >= 10) list << arg10;
	return(test(list));
}



/**
 * Tests to see if two signatures are equal
 */
bool QtRpc::Signature::operator==(const Signature &sig) const
{
	if (qxt_d().data->args.count() != sig.qxt_d().data->args.count())
		return(false);
	if (qxt_d().data->name != sig.qxt_d().data->name)
		return(false);
	if (qxt_d().data->args != sig.qxt_d().data->args)
		return(false);

	return(true);
}


/**
 * @return Returns a QVector of arguments in the function
 */
QVector<QString> QtRpc::Signature::args() const
{
	return(qxt_d().data->args);
}

QDataStream& operator>> (QDataStream& s, QtRpc::Signature& p)
{
	/*
	QString name;
	int numArgs;
	s >> name >> numArgs;
	p.setNumArgs(numArgs);
	for(int i = 0; i < numArgs; ++i)
	{
	QString arg;
	s >> arg;
	p.setArg(i, arg);
	}
	*/
	s >> p.qxt_d().data->name >> p.qxt_d().data->args;
	return s;
}

QDataStream& operator<< (QDataStream& s, const QtRpc::Signature& p)
{
	s << p.qxt_d().data->name << p.qxt_d().data->args;
	return s;
}

/**
 * Sets the argument list
 * @param args The arguments to be set
 */
void QtRpc::Signature::setArgs(const QVector<QString>& args)
{
	qxt_d().data->args = args;
}


/**
 * Makes sure that all the arguments are types suported by QVariant
 * @return Returns true if the arguments are valid, false if they are not.
 */
bool QtRpc::Signature::validate() const
{
	foreach(QString arg, qxt_d().data->args)
	{
		if (!QMetaType::isRegistered(QMetaType::type(qPrintable(arg))))
		{
			qCritical() << "The data type" << arg << "is not supported by QtRpc:" << toString() << qxt_d().data->args.count();
			return(false);
		}
	}

	return(true);
}

QDebug operator<<(QDebug dbg, const QtRpc::Signature& sig)
{
	dbg.nospace() << "Signature(" << sig.toString() << ")";
	return dbg.space();
}


