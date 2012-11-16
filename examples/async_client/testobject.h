#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <ReturnValue>

class TestObject : public QObject
{
    Q_OBJECT
public:
    explicit TestObject(QObject *parent = 0);
        
public slots:
    void pause(uint id, ReturnValue ret);
    void pauseAsync(uint id, ReturnValue ret);

};

#endif // TESTOBJECT_H
