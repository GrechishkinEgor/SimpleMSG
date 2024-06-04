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

bool Log::EnableWritingInConsole()
{
    writingInConsole = true;
    return true;
}

bool Log::EnableWritingInFile()
{
    if (!logFile)
    {
        logFile = new QFile(logFileName);
        logStream = new QTextStream(logFile);
    }

    if (logFile->open(QIODevice::Append))
    {
        writingInFile = true;
        return true;
    }
    else
    {
        if (writingInConsole)
            qDebug() << "Can't open log file '" + logFileName + "'";
        writingInFile = false;
        return false;
    }
}

bool Log::DisableWritingInConsole()
{
    writingInConsole = false;
    return true;
}

bool Log::DisableWritingInFile()
{
    writingInFile = false;
    return true;
}
