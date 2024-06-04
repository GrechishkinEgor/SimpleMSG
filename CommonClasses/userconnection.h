#ifndef USERCONNECTION_H
#define USERCONNECTION_H

#include <QTcpSocket>
#include "user.h"

/*Конкретное соединение между сервером и клиентом*/
class UserConnection : public QTcpSocket
{
protected:
    User userInfo;                      //Кто использует соединение
    qintptr savedSocketDescriptor;      //Дескриптор сокета, сохраняемый для логов при закрытии соединения

    //Для формирования ответа на запрос
    QByteArray* resultData;
    QDataStream* resultStream;

public:
    UserConnection();
    UserConnection(qintptr socketDescriptor);

    qintptr GetSavedSocketDescriptor();
    User& GetUserInfo();

    QDataStream& GetConnectionStream(); //Предоставляет доступ к потоку для формирования ответа на запрос
    void SendData();                    //Отправляет сформированный ответ
    void ClearConnectionStream();       //Отменяет отправку ответа, очищает поток ответа
};

#endif // USERCONNECTION_H
