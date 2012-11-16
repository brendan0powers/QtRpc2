#include "testobject.h"
#include <QDebug>

TestObject::TestObject(QObject *parent) :
    QObject(parent)
{
}

void TestObject::pause(uint id, ReturnValue ret)
{
    if(ret.isError())
    {
        qDebug() << "Async Pause returned with an error:" << ret;
    }
    else
    {
        qDebug() << "Async Pause returned.";
    }
}

void TestObject::pauseAsync(uint id, ReturnValue ret)
{
    if(ret.isError())
    {
        qDebug() << "Server Async Pause returned with an error:" << ret;
    }
    else
    {
        qDebug() << "Server Async Pause returned.";
    }
}
