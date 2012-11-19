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
#include <QDebug>
#include "basicservice.h"
#include "dataservice.h"

int main(int argc, char *argv[])
{
	QApplication app(argc,argv);

    //Create an instance of the service object
    DataService service;

    //Connect to the server, specifying the port, and the remove service to connect to
    ReturnValue ret = service.connect("tcp://localhost:10123/Data");
    //If the connection failes, ret.isError() will be true
    if(ret.isError())
    {
        //You can use ReturnValues in qDebug() statements to quickly print error messages.
        qCritical() << "Failed to connect:" << ret;
        return(1);
    }

    //Retrive the custom data.
    ret = service.getData();
    if(ret.isError())
    {
        qCritical() << "Failed to call getData():" << ret;
        return(1);
    }

    //Pull the custom data out of ReturnValue.
    DataService::CustomData data = ret.value<DataService::CustomData>();
    qDebug() << "Custom Data: " << data.x << data.y << data.z;

    //Retrieve an instance to a service
    ret = service.getBasicService();
    if(ret.isError())
    {
        qCritical() << "Failed to call getBasicService():" << ret;
        return(1);
    }

    //Simply assign an instance of the service class to the returned ReturnValue.
    BasicService basic = ret;

    //Then use the service as you normally would.
    ret = basic.addNumbers(3,5);
    if(ret.isError())
    {
        qCritical() << "Failed to call addNumbers():" << ret;
        return(1);
    }

    //You can use ReturnValue just like a QVariant
    qDebug() << "Call to add() succeeded.";
    qDebug() << "Result:" << ret.toInt();
    qDebug();

    //Calling this function will return an error.
    ret = basic.returnError();
    if(ret.isError())
    {
        //The ReturnValue class can be used to inspect the error.
        qDebug() << "Failed to call returnError():" << ret;
        qDebug() << "Error Number:" << ret.errNumber();
        qDebug() << "Error String:" << ret.errString();
        qDebug() << "This was expected";
    }

    return(0);
}
