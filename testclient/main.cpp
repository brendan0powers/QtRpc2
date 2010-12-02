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
#define USE_QTRPC_PRIVATE_API
#include <ClientMessageBus>
#include <AuthToken>
#include <QStringList>
#include <Signature>
#include "testclient.h"
#include "testcaller.h"
#include "testthread.h"
#include "testsyncro.h"
#include "servicefinder.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	bool thread = false;
	bool checkString = false;
	if (argc > 1)
	{
		QString arg(argv[1]);
		if (arg == " -h" || arg == "--help")
		{
			qDebug() << "This is the testcase app for qtrpc2. By default it tests single threaded mode because multithreaded mode really only works as a hack. qtrpc2 is working if , when run, this command outputs \"Iteration\" a whole bunch of times forever. You must run the testserver before running the test client...\n\nCommand line options:\n	--help (-h)		Display this help message.\n	--thread (-t)		Run in multithreaded mode.\n	--string (-s)		Checks if sending massive string is still broken.\n";
			return 0;
		}
		else if (arg == "--thread" || arg == "-t")
		{
			qDebug() << "Doing threads";
			thread = true;
		}
		else if (arg == "--string" || arg == "-s")
		{
			checkString = true;
		}
		else
		{
			qDebug() << "Unknown argument: " << arg << ". (--help for usage)";
			return 1;
		}
	}
	if (thread)
	{
		TestCaller callertcp(&app);
		qDebug() << callertcp.connect("tcp://user:password@localhost:18777/TestService");
// 		qDebug() << callertcp.connect("tcp://user:password@localhost:18777");

		TestCaller callertcps(&app);
		qDebug() << callertcps.connect("tcps://user:password@localhost:18777/TestService");
// 		qDebug() << callertcps.connect("tcps://user:password@localhost:18777");

		TestCaller callersocket(&app);
		qDebug() << callersocket.connect("socket://user:password@/tmp/qtrpc-socket:TestService");
// 		qDebug() << callersocket.connect("socket://user:password@/tmp/qtrpc-socket");
		return 0;
		for (int i = 0 ; i < 5 ; i++)
		{
			TestThread* thread = new TestThread(&app);
			thread->setTestCaller(&callertcp);
			thread->start();
		}
		/*
		for (int i = 0 ; i < 5 ; i++)
		{
			TestThread* thread = new TestThread(&app);
			thread->setTestCaller(&callertcps);
			thread->start();
		}
		for (int i = 0 ; i < 5 ; i++)
		{
			TestThread* thread = new TestThread(&app);
			thread->setTestCaller(&callersocket);
			thread->start();
		}
		*/
		qDebug() << "INITIALIZED";
		return app.exec();
	}
	else if (checkString)
	{
		TestSyncro client(&app);
		client.connect("tcp://user:password@localhost:18777/StringService");
		while (true)
		{
			client.callMegaString();
		}
	}
	else
	{
		for (int i = 0; i < 100; ++i)
		{
			TestClient* client = new TestClient(&app);
			qDebug() << "Connect:" << client->connect("tcp://user:password@localhost:18777");
			foreach(QString service, client->listServices().toStringList())
			{
				qDebug() << service;
				qDebug() << "	Functions:" << client->listFunctions(service).value<QList<Signature> >();
				qDebug() << "	Callbacks:" << client->listCallbacks(service).value<QList<Signature> >();
				qDebug() << "	Events:" << client->listEvents(service).value<QList<Signature> >();
			}
			qDebug() << "Select Service" << client->selectService("TestService");
			client->call_syncfunction_1();
			qDebug() << "INITIALIZED" << i;
		}
		return app.exec();
	}
}
