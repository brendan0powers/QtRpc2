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
#include "serviceproxy.h"
#include "serviceproxy_p.h"
#include <QMutexLocker>
#include <QDebug>
#include <QStringList>
#include <ReturnValue>
#include <Message>

#include <ServerProtocolInstanceBase>

namespace QtRpc
{

ServiceProxy::ServiceProxy(QObject *parent)
		: ProxyBase(parent)
{
	QXT_INIT_PRIVATE(ServiceProxy);
	qxt_d().server = NULL;
	qxt_d().instance = NULL;
}

ServiceProxy::~ServiceProxy()
{
}

ServiceProxy *ServiceProxy::newInstance(Server *, ServerProtocolInstanceBase *)
{
	Q_ASSERT(false);
	qCritical("newInstance should never be called");
	return NULL;
}

QtRpc::ReturnValue ServiceProxy::auth(QString user, QString passwd)
{
	Q_UNUSED(user);
	Q_UNUSED(passwd);
	qFatal("Auth is not implemented");
	return ReturnValue(1, "Auth is not implemented");
}

ReturnValue ServiceProxy::auth(AuthToken token)
{
	return auth(token.clientData()["username"].toString(), token.clientData()["password"].toString());
}

AuthToken ServiceProxy::authToken()
{
	return qxt_d().token;
}

/**
 * When an event is emited, this function send the event to the instance object
 * @param sig Signature of the event
 * @param args Arguments to the event
 * @param type type of events
 * @return The return value of the event, witch usually isn't important
 */
ReturnValue QtRpc::ServiceProxy::functionCalled(const Signature& sig, const Arguments& args, const QString& type)
{
	if (type == "CallbackValue")
	{
		qCritical() << "Synchronous callbacks are not supported" << sig.toString();
		return(ReturnValue(1, "Synchronous callbacks are not supported"));
	}
	if(!qxt_d().instance)
		return ReturnValue(1, "Invalid internal object");

	qxt_d().instance->sendEvent(qxt_d().instance->serviceId(this), sig, args);

	return(true);
}


/**
 * When a callback is called, this runs the callback on the instance object. Callback are always asynchronous
 * @param obj object of the async callback
 * @param slot slot to call when the return arives
 * @param sig the signature of the callback
 * @param args the argument of the ballback
 * @param type the type of the callback
 * @return Returns an integer id identifying the call, or an error of something went wrong
 */
ReturnValue QtRpc::ServiceProxy::functionCalled(QObject *obj, const char *slot, const Signature& sig, const Arguments& args, const QString& type)
{
	if (type == "Event")
	{
		qCritical() << "Asynchronous events are not supported" << sig.toString();
		return(ReturnValue(1, "Asynchronous events are not supported"));

	}
	if(!qxt_d().instance)
		return ReturnValue(1, "Invalid internal object");

	//strip the argument list from the slot
	QString tmpslot = QString(slot).section("(", 0, 0);
	//remove the 1 from the begining of the function if SLOT() was used
	if (tmpslot.startsWith("1"))
		tmpslot = tmpslot.remove(0, 1);
	Signature s(tmpslot + "()"); //create the signature object from the slot

	return(qxt_d().instance->callCallback(obj, s, qxt_d().instance->serviceId(this), sig, args));
}

/**
 * Internal call that initializes the ServiceProxy object.
 * @param server A pointer to the server object
 * @param instance A pointer to the protocol instance object
 * @param data A pointer to the internal data of the parent service
 */
void QtRpc::ServiceProxy::initProxy(Server *server, ServerProtocolInstanceBase *instance, const QHash<QString, void *>& data)
{
	qxt_d().server = server;
	qxt_d().instance = instance;
	qxt_d().data = data;

	QStringList funclist;
	QStringList eventlist;
	QStringList calllist;

	//Define what types we allow
	funclist << "Event" << "QtRpc::Event" << "CallbackValue" << "QtRpc::CallbackValue";
	calllist << "ReturnValue" << "QtRpc::ReturnValue";

	//Initialize proxy object so the varisous functions work
	ProxyBase::init(funclist, calllist, eventlist);
}


QHash<QString, void *> QtRpc::ServiceProxy::getRawData() const
{
	QMutexLocker locker(const_cast<QMutex*>(&qxt_d().datamutex));
	return qxt_d().data;
}

/**
 * Returns a peice of data set before the service was created
 * @param name The name of the data to retrieve
 * @return Returns a pointer to the data, or NULL if the data was not found
 */
void * QtRpc::ServiceProxy::getData(const QString& name)
{
	QMutexLocker locker(&qxt_d().datamutex);

	if (!qxt_d().data.keys().contains(name))
	{
		qWarning() << "Data named" << name << "does not exist";
		return(NULL);
	}

	return(qxt_d().data[name]);
}



/**
 * Sets a peive of data. This is usually run just after the service was registered with a service object
 * @param name The name of the data
 * @param data A pointer of the data to store
 * @return Returns a copy of the data, or NULL if there was an error.
 */
void * QtRpc::ServiceProxy::setData(const QString& name, void *data)
{
	QMutexLocker locker(&qxt_d().datamutex);

	if (qxt_d().data.keys().contains(name))
		qWarning() << "Overwriting data:" << name;

	qxt_d().data[name] = data;
	return(data);
}



/**
 * Sets a protocol property
 * @param name Name of the property
 * @param value The value of the property
 * @return Returns true on success, false on failure
 */
bool QtRpc::ServiceProxy::setProtocolData(const QString& name, const QVariant& value)
{
	if(!qxt_d().instance)
		return false;
	qxt_d().instance->setProperty(name, value);
	return(true);
}



/**
 * Retrieve the value of a protocol property
 * @param name The name of the property to retrieve
 * @return Returns the value of the property, or a null QVariant on error
 */
QVariant QtRpc::ServiceProxy::getProtocolProperty(const QString& name) const
{
	if(!qxt_d().instance)
		return QVariant();
	return(qxt_d().instance->getProperty(name));
}


/**
 * An internal function called by the protocol instance when the client runs a function
 * @param sig The signature of the function
 * @param args The arguments to pass to the function
 * @return Returns the return value of the function, or an error if something goes wrong
 */
ReturnValue QtRpc::ServiceProxy::callFunction(const Signature& sig, const Arguments& args)
{
	return(callCallback(sig , args));
}

}

void QtRpc::ServiceProxy::disconnected(const QString& reason)
{
	Q_UNUSED(reason);
}

void QtRpc::ServiceProxy::setServiceName(const QString& name)
{
	QMutexLocker locker(&qxt_d().datamutex);
	qxt_d().serviceName = name;
}

QString QtRpc::ServiceProxy::serviceName() const
{
	QMutexLocker locker(const_cast<QMutex*>(&qxt_d().datamutex));
	return qxt_d().serviceName;
}

quint32 QtRpc::ServiceProxy::currentFunctionId() const
{
	QMutexLocker locker(const_cast<QMutex*>(&qxt_d().datamutex));
	if(!qxt_d().instance)
		return -1;
	return qxt_d().instance->currentFunctionId();
}

void QtRpc::ServiceProxy::sendReturn(quint32 id, ReturnValue ret) const
{
	QMutexLocker locker(const_cast<QMutex*>(&qxt_d().datamutex));
	if(!qxt_d().instance)
		return;
	qxt_d().instance->writeMessage(Message(id, ret));
}



