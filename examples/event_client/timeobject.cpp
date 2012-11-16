#include "timeobject.h"
#include <QDebug>

TimeObject::TimeObject(QObject *parent) :
    QObject(parent)
{
}

void TimeObject::currentTime(QDateTime time)
{
    qDebug() << "Current Time:" << time.toString();
}
