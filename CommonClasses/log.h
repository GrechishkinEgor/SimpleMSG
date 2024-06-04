#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QTextStream>
#include <QFile>

class Log
{
private:
    QTextStream* logStream;
    QFile* logFile;
    QString logFileName;

    bool writingInConsole;
    bool writingInFile;
public:
    Log(QString logFileName);
    ~Log();

    bool EnableWritingInFile();
    bool EnableWritingInConsole();
    bool DisableWritingInFile();
    bool DisableWritingInConsole();

    void Write(QString msg);
};

#endif // LOG_H
