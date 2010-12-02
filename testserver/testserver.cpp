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
#include "testserver.h"

#include <QTime>
#include <QTimer>

using namespace QtRpc;

TestServer::TestServer(QObject *parent)
		: ServiceProxy(parent)
{
	doubleCall = true;
	megaString = "zxcvbnmasdfghjklqwertyuiop";
	for (int i = 0 ; i < 2 ; i++)
	{
		megaString += megaString;
	}
}

TestServer::~TestServer()
{
}

ReturnValue TestServer::auth(QString user, QString passwd)
{
	qDebug() << Q_FUNC_INFO << user << passwd;
	qDebug() << "I have authed";
	return "You are now connected";
}

ReturnValue TestServer::syncfunction_1(QString value)
{
	if (value != megaString)
		qFatal("ERROR: Malformed megastring: ");
	qDebug() << Q_FUNC_INFO;

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
	callback_2(this, SLOT(callbackReturned(uint, ReturnValue)), 367124, QVariant(list));
	qDebug() << "Silent failure?" << QMetaObject::invokeMethod(this, "return_syncfunction_1", Qt::QueuedConnection, Q_ARG(quint32, currentFunctionId()));
	return ReturnValue::asyncronous();
// 	return "This";
}

void TestServer::return_syncfunction_1(quint32 id)
{
	qDebug() << "Sending my reply to syncfunction_1" << id;
	sendReturn(id, megaString);
}

void TestServer::callbackReturned(uint id, ReturnValue ret)
{
	qDebug() << Q_FUNC_INFO;
	if (ret != "Warren is a Niger Fig Aught")
		qFatal("Failed in retrun value of testCallback");
}

ReturnValue TestServer::asyncfunction_3(QByteArray value)
{
	qDebug() << Q_FUNC_INFO;
	qDebug() << "Got the something else!";
	if (value != QByteArray("this is some text that will become binary data"))
		qFatal("Failure in somethingElse()");
// 	emit testEvent("Indeed! Indeed! Indeed! Indeed! Indeed! Indeed!");
	return QTime(11, 23, 58, 13);
}

void TestServer::event_0()
{
	qDebug() << Q_FUNC_INFO;
	emit event_0("Indeed! Indeed! Indeed! Indeed! Indeed! Indeed!");
}

ReturnValue TestServer::syncfunction_4()
{
	Q_ASSERT(doubleCall);
	doubleCall = false;
	QTimer::singleShot(100, this, SLOT(event_0()));
	return "true";
}

ReturnValue TestServer::default_param_function(QString arg1, int arg2)
{
	Q_ASSERT(!doubleCall);
	doubleCall = true;
	qDebug() << "I was called right now";
	return true;
}

