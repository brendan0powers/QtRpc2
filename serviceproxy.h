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
#ifndef QTRPCSERVICEPROXY_H
#define QTRPCSERVICEPROXY_H

#include <ProxyBase>
#include <AuthToken>
#include <QxtPimpl>
#include <QtRpcGlobal>


#define QTRPC_SERVICEPROXY(srv) \
	public: srv& operator=(const ReturnValue &ret) \
	{ \
		srv* s = qobject_cast<srv*>(ret.service()); \
		if(s == 0) \
		{ \
			qCritical() << "Assigned a service to a ReturnValue that does not contain a service of that type: Searching for" << metaObject()->className() << "but found" << (ret.service() ? ret.service()->metaObject()->className() : "NULL"); \
			return *this; \
		} \
		return operator=(*s); \
	} \
	public: srv(const ReturnValue &ret) \
	{ \
		srv* s = qobject_cast<srv*>(ret.service()); \
		if(s == 0) \
			qCritical() << "Constructed a service from a ReturnValue that does not contain a service of that type: Searching for" << metaObject()->className() << "but found" << (ret.service() ? ret.service()->metaObject()->className() : "NULL"); \
		else \
			operator=(*s); \
	} \
	private:

#define QTRPC_SERVICEPROXY_PIMPL(name) \
	public: \
	name& operator=(const ReturnValue &ret); \
	name(const ReturnValue& ret); \
	private:

#define QTRPC_SERVICEPROXY_PIMPL_IMPLEMENT(srv) \
	srv& srv::operator=(const ReturnValue &ret) \
	{ \
		srv* s = qobject_cast<srv*>(ret.service()); \
		if(s == 0) \
		{ \
			qCritical() << "Assigned a service to a ReturnValue that does not contain a service of that type: Searching for" << metaObject()->className() << "but found" << (ret.service() ? ret.service()->metaObject()->className() : "NULL"); \
			return *this; \
		} \
		return operator=(*s); \
	} \
	srv::srv(const ReturnValue& ret) \
	{ \
		srv* s = qobject_cast<srv*>(ret.service()); \
		if(s == 0) \
			qCritical() << "Constructed a service from a ReturnValue that does not contain a service of that type: Searching for" << metaObject()->className() << "but found" << (ret.service() ? ret.service()->metaObject()->className() : "NULL"); \
		else \
			operator=(*s); \
	} \
	 
#define QTRPC_RETURNVALUE_OPERATOR(x) QTRPC_SERVICEPROXY(x)

/**
	@brief This macro is not out of date and should no longer be used, in favor of the new template based service registration system. Use this macro in the public section of a ServicePbject class. You pass the name of the ServiceProxy class as the argument.
	@sa ServiceProxy
*/

namespace QtRpc
{

typedef ReturnValue Event;
typedef ReturnValue CallbackValue;
class Server;
class ServerProtocolInstanceBase;
class ServiceProxyPrivate;

/**
	The ServiceProxy class is inherited by you to provide services to be used by ClientProxy objects.

	Here is an example ServiceProxy Object
@code
class TestService : public ServiceProxy
{
	Q_OBJECT
	public:
	QTRPC_SERVICE(TestService); //Important, see below

	TestService(QObject *parent = 0) {} //Don't put anything in the constructory, do initialization in auth
	~TestService();

	ReturnValue auth(QString user, QString passwd)   //Do authentication and initilization, every base class must implement this function
	{
		if(user == "test" && passwd == "test")
			return(true); //authentication succeeded
		else
			return(ReturnValue(1,"Username or password is not correct")); //login failed, return an error
	}

	signals:
	Event testEvent(QString text);
	CallbackValue testCallback(QObject *obj, char *slot,int num,QString text);

	protected slots:
	ReturnValue echo(QString text);
	{
		return(text);
	}

	ReturnValue test();
	{
		//send an event to the client
		testEvent("This is a test Event");

		//run a callback on the client
		//Notice that callbacks are allways asynchronous
		testCallback(this,SLOT(callbackReturned(uint, ReturnValue)),123,"Here is some example text");

		//return an error
		return(ReturnValue(1232,"An error; because i can!"));
	}

	ReturnValue add(int a, int b)
	{
		return(a+b);
	}
	public slots:
       void callbackReturned(uint id, ReturnValue ret);
};
@endcode

	The service above is named TestService, and has 1 event, 1 callback, and 3 functions. Notice the QTRPC_SERVICE macro at the top of the service. This must be added, or new instances of the service cannot be created.

	Every service must implement an auth() function. The auth takes a username and password as strings. If authentication succeedes, you can return any value. If authentication failes, you must return an error. Also, any initialization of the service object must be done ohere, and not in the costructor.

	To emit an event, or run a callback on the client, just excecute the function. Callbacks are allways asynchronous.

	@brief Provides a base class for services exported by a server
	@author Brendan Powers <brendan@resara.com>
	@author Chris Vickery <chris@resara.com>
	@sa ProxyBase
*/
class QTRPC2_EXPORT ServiceProxy : public ProxyBase
{
	QXT_DECLARE_PRIVATE(ServiceProxy);
	friend class ServerProtocolInstanceBase;
	friend class ReturnValue;
	Q_OBJECT
public slots:

	/**
	 * The authfunction must be implemented by any child object of ServiceProxy. It does any authentication and initialization of the service
	 * @param user Username; may be ignored
	 * @param passwd Password; may be ingored
	 * @return Return any value on success, an error on failure.
	 */
	virtual QtRpc::ReturnValue auth(QString user, QString passwd);
	virtual QtRpc::ReturnValue auth(QtRpc::AuthToken auth);
public:
	ServiceProxy(QObject *parent = 0);

	~ServiceProxy();

	/**
	 * The newInstance function is used internally to create a new ServiceProxy instance. You should never call this directly.
	 * @param server The server object the service belongs to
	 * @param instance The protocol instance this service belongs to
	 * @return A new service instance
	 */
	virtual ServiceProxy *newInstance(Server *server, ServerProtocolInstanceBase *instance);

	/**
	 * This function is inherited by a child object of ServiceProxy, and is called whenever a client disconnects from the service
	 * @param reason The resaon the client disconnected
	 */
	virtual void disconnected(const QString& reason);
	void initProxy(Server *server, ServerProtocolInstanceBase *instance, const QHash<QString, void *>& data);
	void * getData(const QString& name);
	void * setData(const QString& name, void *data);
	ReturnValue callFunction(const Signature& sig, const Arguments& args);
	QHash<QString, void *> getRawData() const;
	void setServiceName(const QString& name);
	QString serviceName() const;
	bool setProtocolData(const QString& name, const QVariant& value);
	QVariant getProtocolProperty(const QString& name) const;

	quint32 id() const;
	void setId(quint32 id);

protected:
	AuthToken authToken();
	quint32 currentFunctionId() const;
	void sendReturn(quint32 id, ReturnValue ret) const;
	virtual ReturnValue functionCalled(const Signature& sig, const Arguments& args, const QString& type);
	virtual ReturnValue functionCalled(QObject *obj, const char *slot, const Signature& sig, const Arguments& args, const QString& type);

};

}

#endif
