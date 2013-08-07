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

int main(int argc, char *argv[])
{
	QCoreApplication app(argc,argv);

    //Create an instance of the service object
    BasicService service;

    //Connect to the server using an incorrect password
    ReturnValue ret = service.connect("tcp://bob:wrongpw@localhost:10123/MyService");
    //If the connection failes, ret.isError() will be true
    if(ret.isError())
    {
        qDebug() << "Failed to connect:" << ret;
        qDebug() << "This was expected.";
    }

    //Connect to the server using the correct username and password
    ret = service.connect("tcp://jon:secret@localhost:10123/MyService");
    //If the connection failes, ret.isError() will be true
    if(ret.isError())
    {
        //You can use ReturnValues in qDebug() statements to quickly print error messages.
        qCritical() << "Failed to connect:" << ret;
        return(1);
    }

    //Run the remove function. This will block untill the function returns,
    //Or untill the default time-out is reached
    ret = service.addNumbers(3,5);
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
    ret = service.returnError();
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
