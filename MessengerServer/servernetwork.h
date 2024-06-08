#ifndef SERVERNETWORK_H
#define SERVERNETWORK_H

#include <QTcpServer>
#include <QVector>
#include <QtSql>
#include <QMap>

#include "userconnection.h"
#include "networkeventmainhandler.h"
#include "inetworkeventhandler.h"

class ServerNetwork : public QTcpServer
{
    Q_OBJECT
//Класс-одиночка
private:
    static ServerNetwork* instance;
    ServerNetwork();
public:
    static ServerNetwork* GetServerNetwork();

//Обработчики сетевых событий
private:
    //Основной обработчик событий, содержащий ссылки на конкретные обработчики
    NetworkEventMainHandler mainHandler;

    //Классы конкретных обработчиков
    class ServerCommand_AUTHENTICATE : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_CHAT_LIST : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_CHAT_MESSAGES : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_SEND_MESSAGE : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_REGISTER : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_CHAT_CREATE : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_UnrecognizedCommand : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };


private:
    QVector<UserConnection*> users;                                 //Все текущие подключения к серверу
    int tcpPort;                                                    //Порт, на котором работает сервер

    void SerializeSql(UserConnection* user, QSqlQuery* query);      //Записывает в поток результаты запроса

    void SendRequest_CHAT_UPDATE_MESSAGES(QSqlQuery* messagesInfo); //Отправить одно сообщение всем участникам чата, которые онлайн
    void SendRequest_CHAT_UPDATE_LIST(qint32 idChat, qint32 idChatType, QString chatName, QVector<qint32> members);

public:
    void RunServer(int tcpPort);
    void close();

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void ClosingConnection();
};

#endif // SERVERNETWORK_H
