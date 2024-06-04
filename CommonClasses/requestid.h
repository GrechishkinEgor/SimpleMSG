#ifndef REQUESTID_H
#define REQUESTID_H

#include <QObject>

class RequestID
{
private:
    static quint32 currentId;
public:
    static quint32 NextID();
    virtual ~RequestID() = 0;
};


#endif // REQUESTID_H
