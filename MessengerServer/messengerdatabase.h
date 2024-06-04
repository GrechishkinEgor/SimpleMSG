#ifndef MESSENGERDATABASE_H
#define MESSENGERDATABASE_H

#include <QtSql>

class MessengerDatabase
{
private:
    static MessengerDatabase* instance;
    MessengerDatabase();
public:
    static QSqlDatabase &GetDB();

private:
    QSqlDatabase db;
};

#endif // MESSENGERDATABASE_H
