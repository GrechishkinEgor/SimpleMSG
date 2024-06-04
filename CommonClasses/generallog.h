#ifndef GENERALLOG_H
#define GENERALLOG_H

#include "log.h"

class GeneralLog : public Log
{
private:
    GeneralLog();
    static GeneralLog* instance;
public:
    static GeneralLog* GetLog();
};

#endif // GENERALLOG_H
