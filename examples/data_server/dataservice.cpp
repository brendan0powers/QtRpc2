#include "dataservice.h"
#include "basicservice.h"
#include <QDataStream>

//Register the custom data type, and it's QDataStream operators with Qt.
QTRPC_REGISTER_METATYPE(DataService::CustomData)

DataService::DataService(QObject *parent) :
    ServiceProxy(parent)
{
}

ReturnValue DataService::auth(QString user, QString passwd)
{
    return(true);
}

//Returns a custom data type.
ReturnValue DataService::getData()
{
    //Create the data type, and add some data.
    CustomData data;
    data.x = -1;
    data.y = 500;
    data.z = 52;

    //Pack the data type into a QVariant.
    return(QVariant::fromValue(data));
}

//Returns an instance of BasicService.
ReturnValue DataService::getBasicService()
{
    //Simply return a pointer to a service. The service should have no parent.
    //QtRpc will take ownership of the service and remove it when the client no longer
    //needs it.
    //
    //Multiple clients can be passed the same service instance. In which case, the service
    //will be deleted once all clients are done using it.
    return(new BasicService());
}

//Pack the custom data into a QDataStream
QDataStream& operator<<(QDataStream& d, const DataService::CustomData& object)
{
    d << object.x;
    d << object.y;
    d << object.z;
    return(d);
}

//Unpack the QDataStream into the custom data class.
QDataStream& operator>>(QDataStream& d, DataService::CustomData& object)
{
    d >> object.x;
    d >> object.y;
    d >> object.z;
    return(d);
}
