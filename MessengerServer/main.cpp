#include <QDateTime>
#include <QTextStream>
#include <QIODevice>
#include <QFile>
#include <QTcpServer>
#include <QCoreApplication>

#include "servernetwork.h"
#include "generallog.h"
#include "messengerdatabase.h"
#include "applicationinfo.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);   //Цикл обработки сообщений

    GeneralLog::GetLog()->Write("\n------------\n");
    GeneralLog::GetLog()->Write("Starting the server");
    GeneralLog::GetLog()->Write(QString("Server version: ") + SERVER_VERSION);
    GeneralLog::GetLog()->Write(QString("MSG protocol version: ") + MSG_PROTOCOL_VERSION);
    GeneralLog::GetLog()->Write(QString("Start time: ") + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));

    GeneralLog::GetLog()->Write("Initialize messenger database");
    QSqlDatabase& dbase = MessengerDatabase::GetDB();

    GeneralLog::GetLog()->Write("Initialize network");
    ServerNetwork::GetServerNetwork()->RunServer(8000);

    int resultCode = 0;
    if (dbase.isOpen() && ServerNetwork::GetServerNetwork()->isListening())
    {
        GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ": Run message processing");
        resultCode = app.exec();
    }

    GeneralLog::GetLog()->Write("Stopping server");
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

    GeneralLog::GetLog()->Write("Stopping time: " + QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
    return resultCode;
}
