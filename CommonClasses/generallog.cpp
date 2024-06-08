#include "generallog.h"
#include <QDebug>

GeneralLog* GeneralLog::instance = nullptr;

GeneralLog::GeneralLog() : Log("GeneralLog.txt")
{
    this->WriteInFile(true);
}

GeneralLog *GeneralLog::GetLog()
{
    if (!instance)
        instance = new GeneralLog();
    return instance;
}
