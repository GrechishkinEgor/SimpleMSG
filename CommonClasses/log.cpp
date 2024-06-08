#include "log.h"
#include <QDebug>

Log::Log(QString logFileName)
{
    logFile = nullptr;
    logStream = nullptr;
    writingInFile = false;
    writingInConsole = true;
    this->logFileName = logFileName;
}

Log::~Log()
{
    if (logStream)
    {
        delete logStream;
        logStream = nullptr;
    }
    if (logFile)
    {
        logFile->close();
        delete logFile;
        logFile = nullptr;
    }
}

bool Log::WriteInFile(bool flag)
{
    if (flag)
    {
        if (!logFile)
        {
            logFile = new QFile(logFileName);
            logFile->open(QIODevice::Append);
            logStream = new QTextStream(logFile);
        }


        if (logFile->isOpen())
        {
            writingInFile = true;
            return true;
        }
        else
        {
            writingInFile = false;
            this->Write("Can't open log file '" + logFileName + "'");
            return false;
        }
    }
    else
    {
        writingInFile = false;
        return true;
    }

}

bool Log::WriteInConsole(bool flag)
{
    writingInConsole = flag;
    return true;
}

void Log::Write(QString msg)
{
    if (writingInFile)
    {
        (*logStream) << msg << "\n";
        logStream->flush();
    }
    if (writingInConsole)
        qDebug() << msg;
}
