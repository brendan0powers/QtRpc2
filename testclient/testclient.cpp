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
#include "testclient.h"

#include <QTime>

using namespace QtRpc;

TestClient::TestClient(QObject* parent): ClientProxy(parent)
{
	QObject::connect(this, SIGNAL(event_0(QString)), this, SLOT(receiveEvent(QString)));
	megaString = "zxcvbnmasdfghjklqwertyuiop";
	for (int i = 0 ; i < 2 ; i++)
	{
		megaString += megaString;
	}
}

TestClient::~TestClient()
{
	qDebug() << "TEST CLIENT GOES BYEBYE NOW!";
}

void TestClient::call_syncfunction_1()
{
	qDebug() << Q_FUNC_INFO;
	qDebug() << "Iteration!";
	ReturnValue ret = syncfunction_1(megaString);
	QString value = ret.isError() ? ret.errString() : ret.toString();
	if (value != megaString)
		qFatal(qPrintable(QString("ERROR: Malformed megastring: %1 (%2)").arg(value).arg(value)));
}

ReturnValue TestClient::callback_2(int num, QVariant rediculous)
{
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
	//hash["some toehr stuff"] = QVariant(hash);
	list << hash << list;
	if (rediculous != list)
	{
		qDebug() << rediculous;
		qDebug() << list;
		qFatal("Failed something aweful in testCallback");
	}
	if (num != 367124)
		qFatal("Failed integer in testCallback");
	asyncfunction_3(this, SLOT(functionReturn(uint, ReturnValue)), QByteArray("this is some text that will become binary data"));
	return "Warren is a Niger Fig Aught";
}

void TestClient::functionReturn(uint id, ReturnValue ret)
{
	qDebug() << Q_FUNC_INFO;
	if (ret != QTime(11, 23, 58, 13))
		qFatal("Failure in asyncfunction_3 return value");
	ret = syncfunction_4();
	if(ret != "true")
		qFatal("Failure in syncfunction_4 return value");
	default_param_function("asdasd");
	deleteLater();
}

void TestClient::receiveEvent(QString asdf)
{
	qDebug() << Q_FUNC_INFO;
	if (asdf != "Indeed! Indeed! Indeed! Indeed! Indeed! Indeed!")
		qFatal("Failure in testEvent!!");
	call_syncfunction_1();
}

