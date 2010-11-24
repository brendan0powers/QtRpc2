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
#ifndef QTRPCCLIENTPROTOCOLTHREAD_H
#define QTRPCCLIENTPROTOCOLTHREAD_H

#include <QThread>
#include <QxtPimpl>
#include <QMutex>
#include <QPointer>

#include <ClientMessageBus>
#include <ClientProtocolBase>
#include <qtrpcprivate.h>

namespace QtRpc
{

	class ClientProtocolThreadPrivate;
	
/**
This class creates a thread and places a ClientMessageBus and an appropriate ClientProtocolBase object. The init() function is templated to the protocol object to be created.

This class should never be used directly, and instead a ClientMessageBus should be used. To create a ClientProtocolThread call ClientMessageBus::instance()

	@sa ClientMessageBus
	@brief Creates a thread to be used client side.
	@author Chris Vickery <chris@resara.com>
 */
class ClientProtocolThread : public QThread
{
	QXT_DECLARE_PRIVATE(ClientProtocolThread);
	Q_OBJECT
public:
	ClientProtocolThread(QObject *parent = 0);
	~ClientProtocolThread();

	/**
	 *        This function initializes the thread and creates the protocol object. Once the protocol object and the ClientMessageBus have been created, the function pushes the protocol object to the message bus's thread.
	 * @tparam ClientProtocolType The type of Protocol to create.
	 * @return Returns a pointer to the fully initialized ClientMessageBus object
	 */
	template<typename ClientProtocolType> ClientMessageBus* init()
	{
		_initMutex.lock();
		if (isRunning())
		{
			_initMutex.unlock();
			return _bus;
		}
		_mutex.lock();
		_protocol = new ClientProtocolType();
		return initialize();
	}

private:
	ClientMessageBus* initialize();
	using QThread::start;
	virtual void run();
	QMutex _mutex;
	QMutex _initMutex;
	QPointer<ClientProtocolBase> _protocol;
	QPointer<ClientMessageBus> _bus;

};

}

#endif
