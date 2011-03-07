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
#ifndef QTRPCCLIENTPROTOCOLTEST_H
#define QTRPCCLIENTPROTOCOLTEST_H

#include <ClientProtocolBase>
#include <QxtPimpl>
#include <qtrpcprivate.h>

namespace QtRpc
{

class ClientProtocolTestPrivate;

/**
This class was created for testing purposes only, and will more than likely be deleted very soon.

	@brief Test protocol. Will be deleted soon
	@author Chris Vickery <chris@resara.com>
*/
class ClientProtocolTest : public ClientProtocolBase
{
	QXT_DECLARE_PRIVATE(ClientProtocolTest);
	Q_OBJECT
public:
	ClientProtocolTest(QObject *parent = 0);
	~ClientProtocolTest();
public slots:
// 	virtual void callbackReturn(uint, ReturnValue) { }
	virtual void callbackReturn(Message msg) { }
protected:
// 	virtual void function(uint, Signature, Arguments);
	virtual void function(Message msg);
	virtual ReturnValue setProperty(QString, QVariant);
	virtual ReturnValue getProperty(QString);
	virtual void protocolConnect();
	virtual ReturnValue protocolDisconnect();
	virtual bool isConnected();
};

}

#endif
