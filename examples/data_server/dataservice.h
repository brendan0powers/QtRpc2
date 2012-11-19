#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <ServiceProxy>

using namespace QtRpc;

class DataService : public ServiceProxy
{
    Q_OBJECT
public:
    //A custom data type returned by getData()
    struct CustomData
    {
        int x;
        int y;
        int z;
    };

    explicit DataService(QObject *parent = 0);

    ReturnValue auth(QString user, QString passwd);

public slots:
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
