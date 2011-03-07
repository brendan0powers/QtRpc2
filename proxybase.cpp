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
#include "proxybase.h"
#include "proxybase_p.h"

#include <QMetaObject>
#include <QMetaMethod>

#include <QObject>
#include <QMap>
#include <QDebug>
#include <QBitArray>
#include <QBitmap>
#include <QBrush>
#include <QByteArray>
#include <QChar>
#include <QColor>
#include <QCursor>
#include <QDate>
#include <QDateTime>
#include <QFont>
#include <QIcon>
#include <QImage>
#include <QKeySequence>
#include <QLine>
#include <QLineF>
#include <QVariantList>
#include <QLocale>
#include <QVariantMap>
#include <QMatrix>
#include <QTransform>
#include <QPalette>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QPolygon>
#include <QRect>
#include <QRectF>
#include <QRegExp>
#include <QRegion>
#include <QSize>
#include <QSizeF>
#include <QSizePolicy>
#include <QString>
#include <QStringList>
#include <QTextFormat>
#include <QTextLength>
#include <QTime>
#include <QUrl>
#include <QThread>
#include <Message>
#include <authtoken.h>

#define CONVERT_VOID(_name)  if(name == #_name) return(*((_name *)data));

namespace QtRpc
{

ProxyBase::ProxyBase(QObject *parent) : QObject(parent)
{
	registerMetaTypes();
}

ProxyBase::~ProxyBase()
{
}

static const uint qt_meta_data_QtRpc__ProxyBase[] =
{

// content:
	1,       // revision
	0,       // classname
	0,    0, // classinfo
	0,    0, // methods
	0,    0, // properties
	0,    0, // enums/sets

	0        // eod
};

static const char qt_meta_stringdata_QtRpc__ProxyBase[] =
{
	"QtRpc::ProxyBase\0"
};

const QMetaObject QtRpc::ProxyBase::staticMetaObject =
{
	{ &QObject::staticMetaObject, qt_meta_stringdata_QtRpc__ProxyBase,
		qt_meta_data_QtRpc__ProxyBase, 0 }
};

const QMetaObject *QtRpc::ProxyBase::metaObject() const
{
	return &staticMetaObject;
}

void *QtRpc::ProxyBase::qt_metacast(const char *_clname)
{
	if (!_clname) return 0;
	if (!strcmp(_clname, qt_meta_stringdata_QtRpc__ProxyBase))
		return static_cast<void*>(const_cast< ProxyBase*>(this));
	return QObject::qt_metacast(_clname);
}

int QtRpc::ProxyBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
	if (_id >= 1000)
	{
		QMutexLocker locker(&qxt_d().mutex);
		// Make sure this isn't being run from the wrong thread....
		Q_ASSERT(thread() == QThread::currentThread());
		if (thread() != QThread::currentThread())
		{
			qCritical() << "You cannot call functions from other threads in QtRpc2";
		}
		//make sure the function exists
		if (qxt_d().functions.keys().contains(_id))
		{
			Arguments args;
			Signature sig = qxt_d().functions[_id];

			//check to see if the function is an async function
			if (qxt_d().asyncfunctions.contains(_id))
			{
				//create argument list
				for (int i = 0; i < sig.numArgs(); i++)
				{
					args.append(convertQVariant(sig.arg(i), _a[i+3]));
				}
// 				_a[0] = new ReturnValue(functionCalled(*(QObject **)_a[1], *(char **)_a[2], sig, args, qxt_d().functiontypes[_id]));
				*static_cast<ReturnValue*>(_a[0]) = functionCalled(*(QObject **)_a[1], *(char **)_a[2], sig, args, qxt_d().functiontypes[_id]);
				return(-1);
			}
			else
			{
				//create argument list
				for (int i = 0; i < sig.numArgs(); i++)
				{
					args.append(convertQVariant(sig.arg(i), _a[i+1]));
				}
				*static_cast<ReturnValue*>(_a[0]) = this->functionCalled(sig, args, qxt_d().functiontypes[_id]);
// 				_a[0] = new ReturnValue(functionCalled(sig, args, qxt_d().functiontypes[_id]));
// 				memcpy(_a[0], ret, sizeof(ReturnValue));
				return -1;
			}
		}
	}

	_id = QObject::qt_metacall(_c, _id, _a);

	return _id;
}



/*!
	Initialized the ProxyBase Object. This needs to be run before any other functions are called.

	See the class description for an example of how to use this function

	@param funclist A list of strings to be used when searching for functions calls
	@param callbacklist A list of strings to used when searching for callbacks
	@param eventlist A list of strings to used when searching for events
 */
void ProxyBase::init(QStringList functionlist, QStringList callbacklist, QStringList eventlist)
{
	QMutexLocker locker(&qxt_d().mutex);
	//Initialize method counters
	int numfunctions = 1000;

	//clear old lists in case init() is run twice
	qxt_d().signalHash.clear();
	qxt_d().functions.clear();
	qxt_d().callbacks.clear();
	qxt_d().asyncfunctions.clear();

	//Get the meta object so we can see signals and slots
	const QMetaObject *meta = metaObject();
	//Get the number of methods
	int methods = meta->methodCount();

	//loop through all the methods
	for (int i = 0; i < methods; i++)
	{
		QMetaMethod method = meta->method(i);
		QString type = method.typeName();
		Signature sig(method.signature());

		// This checks to see if it's a cloned signal, and continues if it is...
		// Undocumented private API ftl!!!
		if (method.attributes() && 2)
			continue;

		switch (method.methodType())
		{
			case QMetaMethod::Signal:
				if (functionlist.contains(type))
				{
					//If a function begins with QObject*, char*, then its an asyn function
					if (sig.numArgs() >= 2 && sig.arg(0) == "QObject*" && (sig.arg(1) == "const char*" || sig.arg(1) == "char*"))
					{
						qxt_d().asyncfunctions << numfunctions; //add this function id to the list of async functions

						QVector<QString> args = sig.args(); //get the list of arguments
						//remove the first two arguments
						args.erase(args.begin());
						args.erase(args.begin());
						sig.setArgs(args); //update the argument list with the new list
					}
					Q_ASSERT(sig.validate());
					//connect the signal to a local event
					meta->connect(this, i, this, numfunctions, Qt::DirectConnection);
					qxt_d().functions[numfunctions] = sig;
					qxt_d().functiontypes[numfunctions] = method.typeName();
					numfunctions++;
				}
				else if (eventlist.contains(type))
				{
					Q_ASSERT(sig.validate());
					qxt_d().signalHash[i] = sig;
				}
				//if its not one of the two, we don't care about it
				break;

			case QMetaMethod::Slot:
				if (callbacklist.contains(type))
				{
					Q_ASSERT(sig.validate());
					qxt_d().callbacks[i] = sig;
				}
				break;

			default:
				//We don't really care about other types of methods
				break;
		}
	}
}


/**
 * Converts a void * pointer into a QVariant. For internal use only.
 *
 * @param name The name of the datatype to convert to
 * @param data The source data
 * @return A QVariant of type 'name', or a null QVariant of the name was not valid.
 */
QVariant ProxyBase::convertQVariant(QString name, void *data)
{
	if (name == "QVariant")
		return *static_cast<QVariant*>(data);
	return QVariant(QMetaType::type(qPrintable(name)), data);
}

/**
 * Emits an event.
 * @param sig The signature of the event.
 * @param args The arguments for the event. The arguments must match the signature
 * @return The return value is usually a null QVariant, or an error
 * @sa callCallback()
 */
ReturnValue ProxyBase::emitSignal(Signature sig, Arguments args)
{
	//find the matching signature in callback list
	int index = qxt_d().signalHash.key(sig, -1);

	//didn't find one
	if (index == -1)
	{
		return(ReturnValue(1, "Signal not found: " + sig.toString()));
	}

	return callMetacall(sig, args);
}

/**
 *  Calls a callback function
 * @param sig The signature of the callback function
 * @param args The arguments for the callback function. The arguments must match the signature.
 * @return Return the return value of the callback. Or an error.
 * @sa emitSignal()
 */
ReturnValue ProxyBase::callCallback(Signature sig, Arguments args)
{
	//find the matching signature in callback list
	int index = qxt_d().callbacks.key(sig, -1);

	//didn't find one
	if (index == -1)
	{
		qCritical() << "Failed to find callback:" << sig << qxt_d().callbacks;
		return(ReturnValue(1, "Callback not found"));
	}

	return callMetacall(sig, args);
}

/**
 * This function does all the dirty qt stuff to call any given signal or slot manually. This function is for internal use only
 * @sa callCallback emitSignal
 * @param sig Signature of the function to be called
 * @param args Arguments list for the function
 * @return Returns the ReturnValue from the function, or an error.
 */
ReturnValue ProxyBase::callMetacall(Signature sig, Arguments args)
{
	// Test to make sure the argument list matches the signature... this is important else the void*'s will contain the wrong types (breaking everything)
	QString test = sig.test(args);
	if (!test.isEmpty())
	{
		return(ReturnValue(2, test));
	}

	// Create the return value
	ReturnValue ret;

	// Create an array of void pointers and place the QVariants into it... I don't know why this works, but it does.
	void *param[] = {(void *)&ret, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

	for (int i = 0; i < sig.numArgs(); i++)
	{
		if (sig.arg(i) != "QVariant" && args[i].type() == QVariant::UserType)
		{
			param[i+1] = args[i].data();
		}
		else
		{
			param[i+1] = static_cast<void*>(&args[i]);
		}
	}

	// Check the make sure the signature is a valid string...
	if (sig.name().isEmpty())
		return(ReturnValue(3, "Failed to call " + sig.toString() + ": Sig is not valid (" + sig.name() + ")"));

	// Get the ID number of the function, if it fails normalize the string and try again...
	int id = metaObject()->indexOfMethod(qPrintable(sig.toString()));
	if (id < 0)
		id = metaObject()->indexOfMethod(QMetaObject::normalizedSignature(qPrintable(sig.toString())).constData()); //try again

	if (id < 0) // failed to find the id number of the function
		return(ReturnValue(4, "Failed to call " + sig.toString() + ": Could not find the index of the slot (id=" + QString("%1)").arg(id)));

	// the return from qt_metacall SHOULD be negative if the slot was found...
	int retid = qt_metacall(QMetaObject::InvokeMetaMethod, id, param);
	if (retid > 0) // return error
		return(ReturnValue(5, "Failed to call " + sig.toString() + ": Failed to find it in metacall (" + QString("id=%1 retid= %2 ret=%3)").arg(id).arg(retid).arg(ret.toString())));

	// return the return value. It's data was filled via the *param[] object...
	return(ret);
}

QList<Signature> ProxyBase::listFunctions()
{
	QMutexLocker locker(&qxt_d().mutex);
	return qxt_d().functions.values();
}

QList<Signature> ProxyBase::listCallbacks()
{
	QMutexLocker locker(&qxt_d().mutex);
	return qxt_d().callbacks.values();
}

QList<Signature> ProxyBase::listEvents()
{
	QMutexLocker locker(&qxt_d().mutex);
	return qxt_d().signalHash.values();
}


}
