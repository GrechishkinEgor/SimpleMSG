#include "messengerdatabase.h"
#include "generallog.h"

MessengerDatabase* MessengerDatabase::instance = nullptr;

MessengerDatabase::MessengerDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Messenger.db");
    GeneralLog::GetLog()->Write(QString("Trying to open messenger database ") + "Messenger.db");
    if (!db.open())
        GeneralLog::GetLog()->Write("Error. Can't open database");
    else
        GeneralLog::GetLog()->Write("Messenger database is opened");
}

QSqlDatabase &MessengerDatabase::GetDB()
{
    if (!instance)
        instance = new MessengerDatabase();
    return instance->db;
}
