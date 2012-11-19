#include "dataservice.h"

    QTRPC_REGISTER_METATYPE(DataService::CustomData)

DataService::DataService(QObject *parent) :
    ClientProxy(parent)
{
}

QDataStream& operator<<(QDataStream& d, const DataService::CustomData& object)
{
    d << object.x;
    d << object.y;
    d << object.z;
    return(d);
}

QDataStream& operator>>(QDataStream& d, DataService::CustomData& object)
{
    d >> object.x;
    d >> object.y;
    d >> object.z;
    return(d);
}
