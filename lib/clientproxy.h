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
#ifndef QTRPCCLIENTPROXY_H
#define QTRPCCLIENTPROXY_H

#include <ProxyBase>
#include <QxtPimpl>
#include <ReturnValue>
#include <QUrl>
#include <QPointer>
#include <QExplicitlySharedDataPointer>
#include <QtRpcGlobal>


#define QTRPC_CLIENTPROXY(name) \
	public: \
	inline name& operator=(const ReturnValue &ret) \
	{ \
		QtRpc::ClientProxy::operator=(ret); \
		return *this; \
	} \
	name(const ReturnValue& ret)\
	{ \
		QtRpc::ClientProxy::operator=(ret); \
	} \
	private:

#define QTRPC_CLIENTPROXY_PIMPL(name) \
	public: \
	name& operator=(const ReturnValue &ret); \
	name(const ReturnValue& ret); \
	private:

#define QTRPC_CLIENTPROXY_PIMPL_IMPLEMENT(name) \
	name& name::operator=(const ReturnValue &ret) \
	{ \
		QtRpc::ClientProxy::operator=(ret); \
		return *this; \
	} \
	name::name(const ReturnValue& ret) \
	{ \
		QXT_INIT_PRIVATE(name); \
		QtRpc::ClientProxy::operator=(ret); \
	}

#define QTRPC_RETURNVALUE_OPERATOR_CLIENT(s) QTRPC_CLIENTPROXY(s)

namespace QtRpc
{

typedef ReturnValue CallbackValue;
typedef ReturnValue Event;

class AuthToken;
class ClientMessageBus;
class ClientProxyPrivate;
class ServiceData;

/**
	The ClientProxy object is the object used by the client to access services published by a server. The entire client side api is provided through this object.

	Here is an exmple ClientProxy object
@code
class TestObject : public ClientProxy
{
	Q_OBJECT
	public:
	TestObject(QObject *parent = 0);
	~TestObject();

	signals:
	ReturnValue test();
	ReturnValue echo(QString text);
	ReturnValue add(int a, int b);							//Synchronous add function
	ReturnValue add(QObject *obj, char *name, int a, int b);		//Asynchronous add function
	Event testEvent(QString text);

	public slots:
	ReturnValue testCallback(int num, QString text)
	{
		//Callback are run from the server, the client is expected to do something with the callback, and return a result
		return("Callback Returned");
	}
};
@endcode

	Ass you can see from the example, we have a ClientProxy object that has 3 functions; test, echo, and add. The add function has a synchronous, and asynchronous form. Both forms call the same function on the sever, as synchronous functions are a feature of the client api. There is also an event testEvent. You can treat events just like ordinary qt signals.  There is also a callback. Callbacks are functions that are run on the server, and excecuted on the client. The return value of the callback will be sent back to the server.

	Before you can use a ClientProxy object, you must connect to a server. To do this, call the connect() function. Here is an example
@code
TestObject obj;
ReturnValue val = obj.connect("tcps://test:testpass@127.0.0.1:1234/testService"); //Synchronous Connect
if(val.isError())
{
	qDebug() << "Connect Failed" << ret.errNumber() << ret.errString();
	exit(1);
}
else
{
	qDebug() << "Connect Succeded";
}
@endcode

	The example above shows a synchronous connect statement connting to a encrypted tcp server on the local machine at por 1234. After connection, the ClientProxy object will use testService as the service to make function calls on. Its importatnt for the functions,events, and callbacks of the ClientProxy object to match those of the service object specified in the url.

	You must pass a url to the connect function. The exact syntax of the url varies from protocol to protocol, but the basic syntax is this.
	protocol://user:password@host:port/service

	These are all valid urls
	tcp://localhost:1234/testService
	tcps://testuser:secretpw@localhost:2232/superSecureService
	socket:///tmp/server-socket:ServiceName

	Once a connection has been made, you can connecth to events, and run functions
@code
ReturnValue ret = obj.add(1,5);
if(ret.isError())
{
	qDebug() << "Call Failed" << ret.errNumber() << ret.errString();
}
else
{
	qDebug() << "1 + 5 =" << ret.toInt();
}
@endcode

	As you can see form the example above. You call the function, and save its return value. Before using the return value, its importatnt to check for errors.

	@brief Used by the client to access services
	@author Chris Vickery <chris@resara.com>
	@author Brendan Powers <brendan@resara.com>
	@sa ProxyBase
*/

class QTRPC2_EXPORT ClientProxy : public ProxyBase
{
	QXT_DECLARE_PRIVATE(ClientProxy);
	Q_OBJECT;
	friend class ServiceData;
public:
    class ReturnValueException : std::exception
	{
	public:
        virtual ~ReturnValueException() throw(){}
		ReturnValueException(const ReturnValue &ret)
		{
			_ret = ret;
		}

		virtual const char* what() const throw()
		{
			return _exceptionText.constData();
		}

	private:
		ReturnValue _ret;
		QByteArray _exceptionText;
	};

    class ExceptionThrower
    {
    public:
        virtual void throwException(const ReturnValue &ret) = 0;
    };

	template <class T>
	class ExceptionThrowerTemplated : public ExceptionThrower
	{
		void throwException(const ReturnValue &ret)
		{
			throw T(ret);
		}
	};

	enum State
	{
		Disconnected,
		Connecting,
		Connected
	};
	ClientProxy(QObject *parent = 0);
	ClientProxy(const ClientProxy&, QObject *parent = 0);

	~ClientProxy();

	ReturnValue connect(QString url);
	ReturnValue connect(QString url, const QtRpc::AuthToken &defaultToken);
	virtual ReturnValue connect(QUrl url, QObject *obj = NULL, const char *slot = NULL);
	virtual ReturnValue connect(QUrl url, QObject *obj, const char *slot, const QtRpc::AuthToken &auth);
	ReturnValue connect(QObject *obj, const char *slot, QString url);
	void disconnect();
	///@todo: Next time we break ABI, fix these function signatures too plox
	ReturnValue selectService(QString service);
	ReturnValue selectService(QObject *obj, const char *slot, const QString &service);
	ReturnValue selectService(QString service, AuthToken token);
	ReturnValue selectService(QObject *obj, const char *slot, const QString &service, const AuthToken &token);
	ReturnValue getService(QString service);
	ReturnValue getService(QObject *obj, const char *slot, const QString &service);
	ReturnValue getService(QString service, AuthToken token);
	ReturnValue getService(QObject *obj, const char *slot, const QString &service, const AuthToken &token);
	ReturnValue deselectService();
	ReturnValue deselectService(QObject *obj, const char *slot);
	void init();
	State state();
	ReturnValue listServices();
	ReturnValue listFunctions(const QString &service);
	ReturnValue listCallbacks(const QString &service);
	ReturnValue listEvents(const QString &service);
	QtRpc::AuthToken authToken() const;
	QtRpc::AuthToken &authToken();

	ClientProxy& operator=(const ReturnValue &service);
	ClientProxy& operator=(const ClientProxy &service);

	static void setExceptionsEnabled(bool enabled);
	static void setAsyncExceptionsEnabled(bool enabled);

	template <class T>
	static void setExceptionHandler()
	{
		if(_exceptionHandler)
			delete _exceptionHandler;

		_exceptionHandler = new ExceptionThrowerTemplated<T>();
	}

protected:
	virtual ReturnValue functionCalled(const Signature& sig, const Arguments& args, const QString& type);
	virtual ReturnValue functionCalled(QObject *obj, const char *slot, const Signature& sig, const Arguments& args, const QString& type);
	
	//Custom exception handling. Implement this function to throw an exception instead of returning a ReturnValue when a function fails
	virtual void throwException(const ReturnValue &ret);
	virtual void throwExceptionAsync(const ReturnValue &ret);

	static ExceptionThrower *_exceptionHandler;
	static bool _exceptionsEnabled;
	static bool _asyncExceptionsEnabled;

signals:
	/**
	 * Used internally to send callback return values to the message bus, Don't connect to this signal
	 * @param uid The id of the callback
	 * @param ret The return value of the callback
	 */
	void returnCallback(uint id, ReturnValue ret);

	//FUNCTION CALLS
	/**
	 * Retrieve a protocol property
	 * @param name Property Name
	 * @return Returns value of the property, or a null QVariant if the property was not found
	 */
	QtRpc::ReturnValue getProtocolProperty(QString name);

	/**
	 *  Sets a protocol property
	 * @param name The name of the property to set
	 * @param value The value to set the property to
	 * @return Undefinded, will probably return a null QVariant on error
	 */
	QtRpc::ReturnValue setProtocolProperty(QString name, QVariant value);

	/**
	* This is emited when the protocol becomes disconnected.
	*/
	void disconnected();

signals:
	void asyncronousSignaler(uint, ReturnValue);
	void asyncronousSignalerNamespace(uint, QtRpc::ReturnValue);
	void asyncronousSignalerFunction(uint, ReturnValue);
	void asyncronousSignalerNamespaceFunction(uint, QtRpc::ReturnValue);
};

}

#endif
