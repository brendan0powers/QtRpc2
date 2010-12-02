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
#include "callerservice.h"

#include <QTime>

using namespace QtRpc;

CallerService::CallerService(QObject *parent)
	: ServiceProxy(parent)
{
}

CallerService::~CallerService()
{
}

ReturnValue CallerService::auth(QString user, QString passwd)
{
	return "You are now connected";
}

ReturnValue CallerService::largeValues(QString value)
{
	QString string = "zxcvbnmasdfghjklqwertyuiop";
	for (int i = 0 ; i < 2 ; i++)
	{
		string += string;
	}
	if (value != string)
		qFatal("ERROR: Malformed megastring: ");

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
	testCallback(this, SLOT(callbackReturned(uint, ReturnValue)), 367124, QVariant(list));
	return string;
}

void CallerService::callbackReturned(uint id, ReturnValue ret)
{
	if(ret!="Warren is a Niger Fig Aught")
		qFatal("Failed in retrun value of testCallback");
}

ReturnValue CallerService::somethingElse(QByteArray value)
{
	if(value!=QByteArray("this is some text that will become binary data"))
		qFatal("Failure in somethingElse()");
	emit testEvent("Indeed! Indeed! Indeed! Indeed! Indeed! Indeed!");
	return QTime(11,23,58,13);
}