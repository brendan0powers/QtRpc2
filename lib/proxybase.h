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
#ifndef QTRPCPROXYBASE_H
#define QTRPCPROXYBASE_H

#include <QObject>
#include <QxtPimpl>
#include <QList>
#include <QVariant>
#include <ReturnValue> //useless without it, so we including it in the header for sanity...
#include <Signature> //Kind of goes hand in hand, also...
#include <QtRpcGlobal>

#ifdef signals
#undef signals
#endif
#define signals public
#define QTRPC_ASYNC QObject *obj, const char *slot


namespace QtRpc
{

class ProxyBasePrivate;
class ReturnValue;
class Signature;
typedef QList<QVariant> Arguments;

/**
	This class is a base class that allows calling of functions, events and callbacks of child classes via QTs moc code parser

	Classes inheriting this class must implement functionCalled(Signature,Arguments) or functionCalled(QObject*, char *, Signature, Arguments). The child class also may want to user the other protected functions to emit signals, and use callbacks.

	To initialize this class before use, you must run the init() function after the class is created, and before calling any functions on that class.

	Here is en example class that inherits proxybase

	@code
typedef ReturnValue Event;		//typedef defining Event, so the parser can find event signals
typedef ReturnValue CallbackValue;	//typedef defining CallbackValue, so the parser can find callback slots

class TestProxy : public ProxyBase
{
public:
	TestProxy(QObject *parent) : ProxyBase(parent) {} //constructor
	void init();	//this will run the init() function on the base class with the proper arguments

signals:
	ReturnValue testFunction(int num, QString string);	//Function
	ReturnValue testFunction(QObject *obj, const char *slot, int num, QString string); //Asynchronous Function
	Event testEvent(QString text) //example Event

protected slots:
	CallbackValue testCallback(QString text) //example callback
	{
		return(text);
	}
};
	@endcode

	As you can see, there are three method types; Functions, Events, and Callbacks. Functions are public signals that can be run from outside the class, these functions then have there arguments packed into an Argument list, and then the functionCalled() function is run. Events are standard QT signals, and they can be emited using the emitSignal() function. Callbacks are slots that can be run with the callCallback() function.

	Also, one thing that is important to note is asynchronous functions. If a function starts with QObject *, const char *, as arguments, its considered an asyncronous function. It has the same function signature as a synchronous function. So, for example the asyncronous function testFunction(Object *obj, const char *slot, int num, QString test), has a signature of testFunction(int,QString)

	Asynchronous functions will return immediatly with some identifying informatino about the call(usually an integer ID). And then later, when the function finishes, the slot is called. The slots should be in the form slotname(int id, ReturnValue)

	If you look at the top of the source code example, you'l see 2 typedef statments. These redefine ReturnValue as Event, and Callback. The typedefs are used by the parser to determine if a method is a function,event, or callback. Its important to note that all functions should return a ReturnValue. The typedefs are just for the parsers benefit.

	The types of each method are defined in the init() function call. Lets look at the init() function in the TestProxy class from above
	@code
void TestProxy::init()
{
	//Create some QStringLists that we will fill
	QStringList functions;
	QStringList events;
	QStringList callbacks;

	//notice that we fill the QStringLists with the values we used above
	functions << "ReturnValue";
	events << "Event";
	callback << "CallbackValue";

	//We run ProxyBases init() function to set up the parser
	ProxyBase::init(functions,events,callbacks);
}
	@endcode

	Once the init() function is run ,the parser then knows what functions do what actions. Another thing to note is that if you don't run the init() function before using the class, the behaviour is undefined.

	@brief Provides interface for using Qts MOC, and Meta Objects
	@author Brendan Powers <brendan@resara.com>
	@author Chris Vickery <chris@resara.com>
*/
class QTRPC2_EXPORT ProxyBase : public QObject
{
	QXT_DECLARE_PRIVATE(ProxyBase);
public:
	Q_OBJECT_CHECK;
        static const QMetaObject staticMetaObject;
	virtual const QMetaObject *metaObject() const;
	virtual void *qt_metacast(const char *);
	QT_TR_FUNCTIONS;
	virtual int qt_metacall(QMetaObject::Call, int, void **);

	ProxyBase(QObject *parent = 0);

	~ProxyBase();

	void init(QStringList functionlist, QStringList callbacklist, QStringList eventlist);
	QList<Signature> listFunctions();
	QList<Signature> listCallbacks();
	QList<Signature> listEvents();

protected:

	ReturnValue callMetacall(Signature , Arguments);
	QVariant convertQVariant(QString name, void *data);
	ReturnValue emitSignal(Signature sig, Arguments args);
	ReturnValue callCallback(Signature sig, Arguments args);
	/**
	 * Called when a function is run. You must implement this function in your child class.
	 * @param sig The signature of the function that was called.
	 * @param args The list of arguments used when calling the function.
	 * @param type The name of the return value specified by the function.
	 * @return Simply return() from your implementation to pass the return value to the calling function.
	 */
	virtual ReturnValue functionCalled(const Signature& sig, const Arguments& args, const QString& type) = 0;
	/**
	 * Called when an asynchronous function is called. You must implement this function in your child class.
	 * @param obj The object where the slot resides
	 * @param slot The slot that must be calles using QMetaObjec::metaCall
	 * @param sig The signature of the function that was called.
	 * @param args The list of arguments used when calling the function.
	 * @param type The name of the return value specified by the function.
	 * @return Return the ID of the asynchronous function, or an error if something went wrong.
	 */
	virtual ReturnValue functionCalled(QObject *obj, const char *slot, const Signature& sig, const Arguments& args, const QString& type) = 0;

};

}

#endif
