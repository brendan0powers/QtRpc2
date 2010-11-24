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
#ifndef CLIENTPROTOCOLIODEVICE_P_H
#define CLIENTPROTOCOLIODEVICE_P_H

#include <QObject>
#include <QxtPimpl>
#include <Message>
#include <QMutex>
#include <qtrpcprivate.h>

#include "clientprotocoliodevice.h"

namespace QtRpc
{
/**
	@author Chris Vickery <chris@resara.com>
*/
class ClientProtocolIODevicePrivate : public QObject, public QxtPrivate<ClientProtocolIODevice>
{
	Q_OBJECT
public:
	ClientProtocolIODevicePrivate()
	{
	}
	bool checkProtocolFunction(Message);
	void parseMessage(Message);
	void writeMessage(Message);

	QMutex mutex;
	qint64 totalSize;
	qint64 read;
	QByteArray buffer;
	QDataStream stream;
	QIODevice* device;

public slots:
	void readyRead();
	void selectService();
	void moveToThread(QThread*);
};
}
#endif
