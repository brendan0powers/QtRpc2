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
#include <QApplication>
#include <Server>
#include <ServerProtocolListenerTcp>
#include <QDebug>
#include "basicservice.h"

using namespace QtRpc;

int main(int argc, char *argv[])
{
	QApplication app(argc,argv);

    //Create a server object with default threading options
    //Thread pool with the number of threads equal to the number of cores
    Server srv;

    //Create a single threaded server
    //Server srv(NULL, Server::SingleThread);

    //Create a server that uses a thread pool of 3
    //Server srv(NULL, Server::ThreadPool, 3);

    //Create a server that uses a new thread for each connection
    //Server srv(NULL, Server::ThreadPerInstance);

    //Create a server that forkes a new process for each connection
    //This is not supported for win32 platforms
    //Server srv(NULL, Server::ThreadPerProcess);

    //Create a TCP listener object
    ServerProtocolListenerTcp tcp(&srv);

    //Listen on port 10123 on all network interfaces
    if(!tcp.listen(QHostAddress::Any, 10123))
    {
        //This function returns false if the port is busy
        qCritical() << "Failed to listen on port 10123!";
        return(1);
    }

    //Register a service. The template argument is the ServiceProxy class to use
    //The string argument is the name used to connect to the service
    //A new instance of the BasicService class is created each time a client connects
    srv.registerService<BasicService>("MyService");

    //Process Events
	return app.exec();
}
