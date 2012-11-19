#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <ServiceFinder>

using namespace QtRpc;

class TestObject : public QObject
{
    Q_OBJECT
public:
    explicit TestObject(QObject *parent = 0);
    
public slots:
    void serviceEvent(ServiceFinder::Service service);

public slots:
    void connect(const QString &url);
};

#endif // TESTOBJECT_H
