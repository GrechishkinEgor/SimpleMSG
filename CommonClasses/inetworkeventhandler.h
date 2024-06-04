#ifndef INETWORKEVENTHANDLER_H
#define INETWORKEVENTHANDLER_H

#include "userconnection.h"

class INetworkEventHandler
{
public:
    INetworkEventHandler() { }
    virtual ~INetworkEventHandler() { };
    virtual void Handle(quint32 requestId, UserConnection* user, QDataStream& params) = 0;
};

#endif // INETWORKEVENTHANDLER_H
