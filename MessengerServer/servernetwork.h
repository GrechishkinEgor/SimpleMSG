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
        ServerCommand_AUTHENTICATE() { }
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_CHAT_LIST : public INetworkEventHandler{
    public:
        ServerCommand_CHAT_LIST() { }
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_CHAT_MESSAGES : public INetworkEventHandler{
    public:
        ServerCommand_CHAT_MESSAGES() { }
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_SEND_MESSAGE : public INetworkEventHandler{
    public:
        ServerCommand_SEND_MESSAGE() {  }
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class ServerCommand_UnrecognizedCommand : public INetworkEventHandler{
    public:
        ServerCommand_UnrecognizedCommand() { }
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };


private:
    QVector<UserConnection*> users;                                 //Все текущие подключения к серверу
    int tcpPort;                                                    //Порт, на котором работает сервер

    void SerializeSql(UserConnection* user, QSqlQuery* query);      //Записывает в поток результаты запроса

    void SendRequest_CHAT_UPDATE_MESSAGES(QSqlQuery* messagesInfo); //Отправить одно сообщение всем участникам чата, которые онлайн

public:
    void RunServer(int tcpPort);
    void close();

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void ClosingConnection();
};

#endif // SERVERNETWORK_H
