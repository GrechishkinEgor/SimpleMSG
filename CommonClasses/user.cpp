#include "user.h"

qint32 User::GetIdUser()
{
    return idUser;
}

void User::SetIdUser(qint32 newIdUser)
{
    idUser = newIdUser;
}

QString User::GetLogin()
{
    return login;
}

void User::SetLogin(const QString &newLogin)
{
    login = newLogin;
}

QString User::GetSurname()
{
    return surname;
}

void User::SetSurname(const QString &newSurname)
{
    surname = newSurname;
}

QString User::GetUserName()
{
    return userName;
}

void User::SetUserName(const QString &newUserName)
{
    userName = newUserName;
}

qint32 User::GetIdUserRights()
{
    return idUserRights;
}

bool User::IsAuthenticated()
{
    return isAuthenticated;
}

void User::SetIdUserRights(qint32 newIdUserRights)
{
    idUserRights = newIdUserRights;
}

void User::SetAuthenticatedFlag(bool authenticatedFlag)
{
    isAuthenticated = authenticatedFlag;
}

User::User()
{
    idUser = 0;
    login = "UNDEFINED";
    surname = "UNDEFINED";
    userName = "UNDEFINED";
    idUserRights = 0;
}
