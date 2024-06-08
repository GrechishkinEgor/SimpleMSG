#ifndef APPLICATIONCONTEXT_H
#define APPLICATIONCONTEXT_H

#include <QTcpSocket>
#include <QByteArray>
#include <QMap>
#include <QTimer>
#include "qapplication.h"

#include "identificationform.h"
#include "messengermainform.h"
#include "helloform.h"
#include "registrationform.h"
#include "startpersonalchatform.h"


#include "inetworkeventhandler.h"
#include "networkeventmainhandler.h"
#include "chat.h"


class ApplicationContext : public QObject
{
    Q_OBJECT

/*Класс-одиночка*/
private:
    static ApplicationContext* instance;
    ApplicationContext();
    ~ApplicationContext();
public:
    static ApplicationContext* GetContext();


/*Описание обработчиков сетевых событий*/
private:

    class ServerAnswer_AUTHENTICATE : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class Server_Answer_CHAT_LIST : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class Server_Answer_CHAT_MESSAGES : public INetworkEventHandler{
    private:
        QMap<quint32, Chat*> requestList;
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
        void AddRequest(quint32 requestId, Chat* requiredChat);
    };

    class Server_Answer_SEND_MESSAGE : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class Server_Answer_CHAT_UPDATE_MESSAGES : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class Server_Answer_REGISTER : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class Server_Answer_CHAT_CREATE : public INetworkEventHandler{
    private:
        bool isPersonalChat;
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
        void SetPersonalChatFlag(bool flag);
    };

    class Server_Answer_CHAT_UPDATE_LIST : public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

    class Server_Answer_Unrecognized_Command: public INetworkEventHandler{
    public:
        void Handle(quint32 requestId, UserConnection* user, QDataStream& params) override;
    };

//Указатель на объект-цикл обработки сообщений всего приложения
private:
     QApplication* applicationMainEventLoop;
public:
     void SetApplicationMainEventLoop(QApplication* eventLoop);


private:
    //Окна
    HelloForm* helloWindow;
    IdentificationForm* identificationWindow;
    MessengerMainForm* messengerMainWindow;
    RegistrationForm* registrationWindow;
    StartPersonalChatForm* startPersonalChatWindow;

    //Работа с сетью
    NetworkEventMainHandler mainHandler;        //Главный обработчик сетевых событий, принимающий сообщения
    UserConnection currentUser;                 //Соединение с сервером
    QTimer* waitingNetworkEventTimer;           //Таймер ожидания сетевых событий

    //Данные чатов
    QMap<qint32, Chat*> chats;                  //Данные обо всех чатах пользователя
    QMap<qint32, User*> users;                  //Данные о пользователях, которые запрашивались у сервера за время работы приложения
    Chat* selectedChat;                         //Чат, который в данный момент обозревается пользователем

public:
    //Показ окон
    void ShowHelloForm();
    void ShowIdentificationForm();
    void ShowRegistrationForm();
    void ShowMessengerMainForm();               //Также скрывает приветственное окно и окно идентификации
    void ShowStartPersonalChatWindow();
    void HideCreatingChatWindows();             //Скрывает окна, отвечающие за создание чатов

    QMap<qint32, Chat*>& GetChats();
    QMap<qint32, User*>& GetUsers();
    UserConnection& GetUserConnection();

    //Запросы серверу
    void SendRequest_AUTHETICATE(QString login, QString password);
    void SendRequest_CHAT_LIST();
    void SendRequest_CHAT_MESSAGES(Chat* selectedChat);
    void SendRequest_SEND_MESSAGE(Message* newMessage);
    void SendRequest_REGISTER(User* newUser, QString password);
    void SendRequest_CHAT_CREATE(QString login);    //Для личных сообщений

public slots:
    void ConnectToServerTimeout();
    void ConnectToServerSuccess();
    void ServerDisconnected();
};

#endif // APPLICATIONCONTEXT_H
