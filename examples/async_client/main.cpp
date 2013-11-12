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
#include <QCoreApplication>
#include <QDebug>
#include "basicservice.h"
#include "testobject.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc,argv);

    //Create an instance of the service object
    BasicService service;

    //Connect to the server, specifying the port, and the remove service to connect to
    ReturnValue ret = service.connect("tcp://localhost:10123/MyService");
    //If the connection failes, ret.isError() will be true
    if(ret.isError())
    {
        //You can use ReturnValues in qDebug() statements to quickly print error messages.
        qCritical() << "Failed to connect:" << ret;
        return(1);
    }

    TestObject object;

    //Run the asynchronous pause function on the server. This will call the
    //pauseAsnc slot on object when it returns.
    ret = service.pauseAsync(&object, SLOT(pauseAsync(uint,ReturnValue)));
    if(ret.isError())
    {
        qCritical() << "Failed to call pauseAsync():" << ret;
        return(1);
    }
    //Calling an asynchronous function returns a numeric ID. This ID can be
    //Used to identify the function when it returns.ret = service.pauseAsync(&object, SLOT(pauseAsync(uint,ReturnValue)));
    if(ret.isError())
    {
        qCritical() << "Failed to call pauseAsync():" << ret;
        return(1);
    }
    qDebug() << "pauseAsync() ID:" << ret;
    qDebug() << "pauseAsync() ID:" << ret;

    //Run the synchronous pause function. This will block both the client
    //and the server untill it returns
    qDebug() << "Starting Blocking Pause";
    ret = service.pause();
    if(ret.isError())
    {
        qCritical() << "Failed to call pause():" << ret;
        return(1);
    }
    qDebug() << "Blocking Pause Returned";

    //Run the pause() function asynchronously. This will block on the server,
    //but not on the client. When the function returns, the pause slot on object
    //will be called
    ret = service.pause(&object, SLOT(pause(uint,ReturnValue)));
    if(ret.isError())
    {
        qCritical() << "Failed to call pause():" << ret;
        return(1);
    }
    qDebug() << "pause() ID:" << ret;

    //An event loop is needed for asynchronous calls.
    return(app.exec());
}
