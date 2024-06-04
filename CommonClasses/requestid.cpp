#include "requestid.h"

quint32 RequestID::currentId = 0;

quint32 RequestID::NextID()
{
    currentId++;
    return currentId;
}
