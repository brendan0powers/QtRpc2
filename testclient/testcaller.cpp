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
#include "testcaller.h"

#include <QTime>
#include <QThread>
#include <QHostAddress>

using namespace QtRpc;

TestCaller::TestCaller(QObject* parent): ClientProxy(parent)
{
// 	ServiceFinder* finder =  new ServiceFinder("TestService", this);
// 	QObject::connect(finder, SIGNAL(serviceEvent(ServiceFinder::Service)), this, SLOT(serviceEvent(ServiceFinder::Service)));
// 	finder->scan();
	ReturnValue ret = connect("tcp://localhost:18777/TestService");
	if(ret.isError())
		qDebug() << ret.errString();
	else
		qDebug() << ret;
	ret = somethingElse("this is some text that will become binary data");
	if(ret.isError())
		qDebug() << ret.errString();
	else
		qDebug() << ret;
	megaString = "zxcvbnmasdfghjklqwertyuiop";
	for (int i = 0 ; i < 2 ; i++)
	{
		megaString += megaString;
	}
	QObject::connect(this, SIGNAL(testEvent(QString)), this, SLOT(receiveEvent(QString)));
}

void TestCaller::serviceEvent(ServiceFinder::Service service)
{
	qDebug() << "serviceType" << service.serviceType();
	qDebug() << "serviceName" << service.serviceName();
	qDebug() << "domain" << service.domain();
	qDebug() << "allAddresses" << service.allAddresses();
	qDebug() << "\nvalidAddresses" << service.validAddresses();
	qDebug() << "\naddress" << service.address();
	qDebug() << "event" << service.event();
	qDebug() << "port" << service.port();
	
	connect(QString("tcp://%1:%2/%3").arg(service.address().toString()).arg(service.port()).arg(service.serviceType()));
}

TestCaller::TestCaller(const TestCaller& tc, QObject* parent): ClientProxy(tc, parent)
{
	megaString = "zxcvbnmasdfghjklqwertyuiop";
	for (int i = 0 ; i < 2 ; i++)
	{
		megaString += megaString;
	}
	QObject::connect(this, SIGNAL(testEvent(QString)), this, SLOT(receiveEvent(QString)));
}

TestCaller::~TestCaller()
{
	QMutexLocker locker(&mutex);
}

void TestCaller::callMegaString()
{
	QMutexLocker locker(&mutex);
	qDebug() << "Iteration!" << this << thread() << QThread::currentThread();
	ReturnValue ret = largeValues(megaString);
	QString value = ret.isError() ? ret.errString() : ret.toString();
	if (value != megaString)
		qFatal(qPrintable(QString("ERROR: Malformed megastring: %1 (%2)").arg(value).arg(ret.errString())));
}

ReturnValue TestCaller::testCallback(int num, QVariant rediculous)
{
	QMutexLocker locker(&mutex);
	QList<QVariant> list;
	QMap<QString, QVariant> hash;
	QMap<QString, QVariant> map;
	list << "This is a string" << 12345 << QByteArray("this is some text that will become binary");
	map["472"] = "tyjfsgnseth";
	map["701023"] = "agh530jrfksuj";
	hash["key"] = 1234;
	hash["some random shit"] = QVariant(map);
	hash["some random shit2"] = list;
	hash["some toehr stuff"] = QVariant(hash);
	list << hash << list;
	if (rediculous != list)
	{
		qDebug() << rediculous;
		qDebug() << list;
		qFatal("Failed something aweful in testCallback");
	}
	if (num != 367124)
		qFatal("Failed integer in testCallback");
	return "Niger Fig Aught";
}

void TestCaller::functionReturn(uint id, ReturnValue ret)
{
	QMutexLocker locker(&mutex);
	if (ret != QTime(11, 23, 58, 13))
		qFatal("Failure in somethingElse return value");
}

void TestCaller::receiveEvent(QString asdf)
{
	QMutexLocker locker(&mutex);
	if (asdf != "Indeed! Indeed! Indeed! Indeed! Indeed! Indeed!")
		qFatal("Failure in testEvent!!");
}

void TestCaller::receiveEvent()
{
	qDebug() << "Got an event, disconnecting and reconnecting";
	QMutexLocker locker(&mutex);
	qDebug() << "Got an event, disconnecting and reconnecting";
	
}

void TestCaller::callSomethingElse(QObject* obj, const char* slot, QByteArray var)
{
// 	QMutexLocker locker(&mutex);
	somethingElse(var);
}
