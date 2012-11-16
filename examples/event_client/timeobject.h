#ifndef TIMEOBJECT_H
#define TIMEOBJECT_H

#include <QObject>
#include <QDateTime>

class TimeObject : public QObject
{
    Q_OBJECT
public:
    explicit TimeObject(QObject *parent = 0);
    
public slots:
    void currentTime(QDateTime time);
    
};

#endif // TIMEOBJECT_H
