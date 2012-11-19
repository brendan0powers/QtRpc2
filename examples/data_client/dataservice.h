#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <ClientProxy>

using namespace QtRpc;

class DataService : public ClientProxy
{
    Q_OBJECT
    //This macro declares a couple of operators for converting
    //ReturnValues to services.
    QTRPC_CLIENTPROXY(DataService)
public:
    explicit DataService(QObject *parent = 0);
    
    //A custom data type returned by getData()
    struct CustomData
    {
        int x;
        int y;
        int z;
    };

signals:
    ReturnValue getData();
    ReturnValue getBasicService();
};

//Declare QDataStream operators for any custom types to be passed over QtRpc.
//These will serialize/deserialize the data for transport over the network.
QDataStream& operator<<(QDataStream& d, const DataService::CustomData& object);
QDataStream& operator>>(QDataStream& d, DataService::CustomData& object);

//Register the custom type with the meta-object system.
Q_DECLARE_METATYPE(DataService::CustomData)

#endif // DATASERVICE_H
