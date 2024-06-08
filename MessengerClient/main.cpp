#include "applicationcontext.h"
#include "generallog.h"
#include "applicationinfo.h"

#include <QApplication>
#include <QDateTime>

int main(int argc, char *argv[])
{
    GeneralLog::GetLog()->WriteInFile(false);
    GeneralLog::GetLog()->WriteInConsole(true);

    GeneralLog::GetLog()->Write("\n------------\n");
    GeneralLog::GetLog()->Write("Starting the client");
    GeneralLog::GetLog()->Write(QString("Client version: ") + CLIENT_VERSION);
    GeneralLog::GetLog()->Write(QString("MSG protocol version: ") + MSG_PROTOCOL_VERSION);
    GeneralLog::GetLog()->Write(QString("Starting time: ") + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));

    QApplication eventLoop(argc, argv);
    ApplicationContext::GetContext()->SetApplicationMainEventLoop(&eventLoop);

    int resultCode = eventLoop.exec();

    GeneralLog::GetLog()->Write("Stopping server");
    GeneralLog::GetLog()->Write("Stopping time: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
    return resultCode;
}
