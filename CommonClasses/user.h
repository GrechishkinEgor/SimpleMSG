#ifndef USER_H
#define USER_H

#include <QString>

/*Содержит данные конкретного пользователя мессенджера*/
class User
{
private:
    qint32 idUser;
    QString login;
    QString surname;
    QString userName;
    qint32 idUserRights;
    bool isAuthenticated;

public:
    User();

    qint32 GetIdUser();
    QString GetLogin();
    QString GetSurname();
    QString GetUserName();
    qint32 GetIdUserRights();
    bool IsAuthenticated();

    void SetIdUser(qint32 newIdUser);
    void SetLogin(const QString &newLogin);
    void SetSurname(const QString &newSurname);
    void SetUserName(const QString &newUserName);
    void SetIdUserRights(qint32 newIdUserRights);
    void SetAuthenticatedFlag(bool authenticatedFlag);
};

#endif // USER_H
