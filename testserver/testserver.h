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
#ifndef QTRPCTESTSERVER_H
#define QTRPCTESTSERVER_H

#include <ServiceProxy>
#include <QDebug>

using namespace QtRpc;

/**
	@author Chris Vickery <chris@resara.com>
*/
class TestServer : public ServiceProxy
{
	Q_OBJECT
public:
	TestServer(QObject *parent = 0);

	~TestServer();

	virtual QtRpc::ReturnValue auth(QString user, QString passwd);
signals:
	Event event_0(QString);
	CallbackValue callback_2(QObject *obj, const char *slot, int num, QVariant rediculous);

protected slots:
	ReturnValue syncfunction_1(QString);
	ReturnValue asyncfunction_3(QByteArray);
	ReturnValue syncfunction_4();
	ReturnValue default_param_function(QString arg1, int arg2 = 5);
	void event_0();

public slots:
	void return_syncfunction_1(quint32 id);
	void callbackReturned(uint id, ReturnValue ret);
private:
	QString megaString;
	bool doubleCall;

};


#endif
