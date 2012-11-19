#include "testobject.h"

#include <ServiceFinder>
#include <QDebug>
#include <QApplication>
#include <QHostAddress>
#include "basicservice.h"

TestObject::TestObject(QObject *parent) :
    QObject(parent)
{
    //Create a service finder object that searches for MyService instances.
    ServiceFinder *finder = new ServiceFinder("MyService",this);
    //Connect the serviceEvent signal
    QObject::connect(finder, SIGNAL(serviceEvent(ServiceFinder::Service)), this, SLOT(serviceEvent(ServiceFinder::Service)));
    //Start scanning for services
    finder->scan();
}

//This slot is called when a service is found
void TestObject::serviceEvent(ServiceFinder::Service service)
{
    //Bonjour may hand us bogus services. Skip them
    if(service.port() == 0) return;

    //Get the host and port of the service
    QString host = service.address().toString();
    int port = service.port();

    //Construct a URL
    QString url = QString("tcp://%1:%2/MyService").arg(host).arg(port);

    qDebug() << "Found Service:" << url;
    //Connect to the service
    connect(url);

}

//This is the code from the main() function of basic_client.
void TestObject::connect(const QString &url)
{
    //Create an instance of the service object
    BasicService service;

    qDebug() << "Connecting to:" << url;

    //Connect to the server, specifying the port, and the remove service to connect to
    ReturnValue ret = service.connect("tcp://localhost:10123/MyService");
    //If the connection failes, ret.isError() will be true
    if(ret.isError())
    {
        //You can use ReturnValues in qDebug() statements to quickly print error messages.
        qCritical() << "Failed to connect:" << ret;
        qApp->exit(1);
        return;
    }

    //Run the remove function. This will block untill the function returns,
    //Or untill the default time-out is reached
    ret = service.addNumbers(3,5);
    if(ret.isError())
    {
        qCritical() << "Failed to call addNumbers():" << ret;
        qApp->exit(1);
        return;
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

    qApp->exit(0);
}
