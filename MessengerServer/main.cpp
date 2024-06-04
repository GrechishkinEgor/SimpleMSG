#include <QDateTime>
#include <QTextStream>
#include <QIODevice>
#include <QFile>
#include <QTcpServer>
#include <QCoreApplication>

#include "servernetwork.h"
#include "generallog.h"
#include "messengerdatabase.h"

#define SERVER_VERSION "0.1"

int main(int argc, char *argv[])
{
    GeneralLog::GetLog()->Write("\n------------\n");

    QCoreApplication app(argc, argv);
    GeneralLog::GetLog()->Write(QString("Start server version ") + SERVER_VERSION +
                "\nStart time: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));

    GeneralLog::GetLog()->Write("Initialize messenger database");
    QSqlDatabase& dbase = MessengerDatabase::GetDB();

    GeneralLog::GetLog()->Write("Initialize network");
    ServerNetwork::GetServerNetwork()->RunServer(8000);

    int resultCode = 0;
    if (dbase.isOpen() && ServerNetwork::GetServerNetwork()->isListening())
    {
        GeneralLog::GetLog()->Write("Run message processing");
        resultCode = app.exec();
    }

    GeneralLog::GetLog()->Write("Stop server");
    if (ServerNetwork::GetServerNetwork()->isListening())
    {
        GeneralLog::GetLog()->Write("Stopping network");
        ServerNetwork::GetServerNetwork()->close();
    }
    if (dbase.isOpen())
    {
        GeneralLog::GetLog()->Write("Closing messenger database");
        dbase.close();
    }

    GeneralLog::GetLog()->Write("Stop time: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
    return resultCode;
}
