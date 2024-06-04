#include "servernetwork.h"
#include "qtcpsocket.h"
#include "generallog.h"
#include "messengerdatabase.h"
#include "requestid.h"

ServerNetwork* ServerNetwork::instance = nullptr;

ServerNetwork::ServerNetwork()
{
    QMap<QString, INetworkEventHandler*>& networkHandlers = mainHandler.GetHandlers();

    networkHandlers[QString("AUTHENTICATE")] = new ServerCommand_AUTHENTICATE();
    networkHandlers[QString("CHAT LIST")] = new ServerCommand_CHAT_LIST();
    networkHandlers[QString("CHAT MESSAGES")] = new ServerCommand_CHAT_MESSAGES();
    networkHandlers[QString("SEND MESSAGE")] = new ServerCommand_SEND_MESSAGE();
    mainHandler.SetUnrecognizedEventHandler(new ServerCommand_UnrecognizedCommand());
    return;
}

void ServerNetwork::SerializeSql(UserConnection* user, QSqlQuery *query)
{
    int recordCount = 0;
    if (query->first())
    {
        recordCount++;
        while (query->next())
            recordCount++;
    }
    user->GetConnectionStream() << quint32(recordCount);
    GeneralLog::GetLog()->Write("Found " + QString::number(recordCount) + " records");

    if (query->first())
        do
            for (int i = 0; i < query->record().count(); i++)
                user->GetConnectionStream() << query->value(i).toString();
        while (query->next());
}

void ServerNetwork::RunServer(int tcpPort)
{
    this->tcpPort = tcpPort;
    if (this->listen(QHostAddress::Any, tcpPort))
        GeneralLog::GetLog()->Write("The server is listening network on TCP-port " + QString::number(tcpPort));
    else
        GeneralLog::GetLog()->Write("Error. Can't listen network");
}

ServerNetwork *ServerNetwork::GetServerNetwork()
{
    if (!instance)
        instance = new ServerNetwork();
    return instance;
}

void ServerNetwork::close()
{
    for (auto i = users.begin(); i != users.end(); i++)
    {
        (*i)->disconnectFromHost();
        (*i)->deleteLater();
    }
    users.clear();
    QTcpServer::close();
}

void ServerNetwork::SendRequest_CHAT_UPDATE_MESSAGES(QSqlQuery *messagesInfo)
{
    QSqlQuery chatMembersQuery(MessengerDatabase::GetDB());  //Кому отправлять сообщение
    messagesInfo->first();
    chatMembersQuery.exec("SELECT id_user FROM chat_member WHERE id_chat = " +
                     messagesInfo->value(messagesInfo->record().indexOf("id_chat")).toString() + " ;");
    QVector<qint32> membersId;
    if (chatMembersQuery.first())
        do
            membersId.push_back(chatMembersQuery.value(0).toInt());
        while(chatMembersQuery.next());

    for (auto i = users.begin(); i != users.end(); i++)
        for (int j = 0; j < membersId.length(); j++)
            if ((*i)->GetUserInfo().GetIdUser() == membersId[j])
            {
                GeneralLog::GetLog()->Write("Sending new message to user " + QString::number((*i)->GetUserInfo().GetIdUser()));
                (*i)->GetConnectionStream() << QString("CHAT UPDATE MESSAGES") << RequestID::NextID();
                this->SerializeSql((*i), messagesInfo);
                (*i)->SendData();
                break;
            }
}

void ServerNetwork::incomingConnection(qintptr socketDescriptor)
{
    UserConnection* currentUser = new UserConnection(socketDescriptor);
    connect(currentUser, &QTcpSocket::readyRead, &mainHandler, &NetworkEventMainHandler::SocketReadyRead);
    connect(currentUser, &QTcpSocket::disconnected, this, &ServerNetwork::ClosingConnection);
    users.push_back(currentUser);
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") +
                ". User connected. Socket descriptor: " + QString::number(socketDescriptor));
}

void ServerNetwork::ClosingConnection()
{
    UserConnection* closingConnection = (UserConnection*)sender();
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") +
                                ". User disconnected. Socket descriptor: " +
                                QString::number(closingConnection->GetSavedSocketDescriptor()));
    for (auto i = users.begin(); i != users.end(); i++)
        if ((*i) == closingConnection)
        {
            (*i)->deleteLater();
            users.erase(i);
            break;
        }
}

void ServerNetwork::ServerCommand_AUTHENTICATE::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    QSqlQuery query(MessengerDatabase::GetDB());
    QString login, password;
    QString correctPassword;

    params >> login >> password;
    query.exec("SELECT * FROM user WHERE user.login = '" + login + "';");
    if (query.first())
    {
        QSqlRecord record = query.record();
        correctPassword = query.value(record.indexOf("password")).toString();
        if (password == correctPassword)
        {
            user->GetUserInfo().SetAuthenticatedFlag(true);
            user->GetUserInfo().SetIdUser(query.value(record.indexOf("id_user")).toInt());
            user->GetUserInfo().SetLogin(login);
            user->GetUserInfo().SetSurname(query.value(record.indexOf("surname")).toString());
            user->GetUserInfo().SetUserName(query.value(record.indexOf("user_name")).toString());
            GeneralLog::GetLog()->Write("Authentication user '" + login + "' is successfull");

            user->GetConnectionStream() << QString("SUCCESS") <<  user->GetUserInfo().GetIdUser()
                                        << user->GetUserInfo().GetSurname()
                                        << user->GetUserInfo().GetUserName();
        }
        else
        {
            GeneralLog::GetLog()->Write("Authentication of user '" + login + "' failed. Password is wrong");
            user->GetConnectionStream() << "ERROR";
        }
    }
    else
    {
        GeneralLog::GetLog()->Write("User '" + login + "' isn't detected");
        user->GetConnectionStream() << "ERROR";
    }
    user->SendData();
}

void ServerNetwork::ServerCommand_CHAT_LIST::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    if (!user->GetUserInfo().IsAuthenticated())
    {
        GeneralLog::GetLog()->Write("User isn't authenticated. Command is declined");
        user->GetConnectionStream() << qint32(0);
        user->SendData();
        return;
    }

    QSqlQuery query(MessengerDatabase::GetDB());
    query.exec("SELECT chat.id_chat, chat.id_chat_type, chat.char_name "
               " FROM chat, chat_member WHERE chat.id_chat = chat_member.id_chat AND chat_member.id_user = "
               + QString::number(user->GetUserInfo().GetIdUser()) + ";");

    ServerNetwork::GetServerNetwork()->SerializeSql(user, &query);

    user->SendData();
    GeneralLog::GetLog()->Write("Command CHAT LIST is completed");
    return;
}

void ServerNetwork::ServerCommand_UnrecognizedCommand::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    user->GetConnectionStream() << QString("ERROR");
    user->SendData();
}

void ServerNetwork::ServerCommand_CHAT_MESSAGES::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    if (!user->GetUserInfo().IsAuthenticated())
    {
        GeneralLog::GetLog()->Write("User isn't authenticated. Command is declined");
        user->GetConnectionStream() << qint32(0) << quint32(0);
        user->SendData();
        return;
    }

    int idChat;
    params >> idChat;

    QSqlQuery query(MessengerDatabase::GetDB());

    //Формируем список участников чата
    GeneralLog::GetLog()->Write("Select members");
    query.exec("SELECT user.id_user, user.surname, user.user_name "
               " FROM user, chat_member WHERE user.id_user = chat_member.id_user AND chat_member.id_chat = "
               + QString::number(idChat) + ";");
    ServerNetwork::GetServerNetwork()->SerializeSql(user, &query);

    //Формируем список сообщений
    GeneralLog::GetLog()->Write("Select messages");
    query.exec("SELECT message.id_message, message.message_time, message.id_user, message.message_text "
               " FROM message WHERE message.id_chat = "
               + QString::number(idChat) + " ORDER BY message.id_message;");
    ServerNetwork::GetServerNetwork()->SerializeSql(user, &query);

    user->SendData();
    GeneralLog::GetLog()->Write("Command CHAT MESSAGES is completed");
    return;
}

void ServerNetwork::ServerCommand_SEND_MESSAGE::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    if (!user->GetUserInfo().IsAuthenticated())
    {
        GeneralLog::GetLog()->Write("User isn't authenticated. Command is declined");
        user->GetConnectionStream() << "ERROR";
        user->SendData();
        return;
    }

    int idChat;
    params >> idChat;
    QString messageText;
    params >> messageText;

    /*Проверяем, что пользователь, отправивший сообщение, состоит в чате*/
    QSqlQuery query(MessengerDatabase::GetDB());
    query.exec("SELECT * FROM chat_member WHERE chat_member.id_user = " + QString::number(user->GetUserInfo().GetIdUser()) + " AND "
              " chat_member.id_chat = " + QString::number(idChat) + ";");
    if (!query.first())
    {
        GeneralLog::GetLog()->Write("The user isn't a member of chat " + QString::number(idChat));
        user->GetConnectionStream() << "ERROR";
        user->SendData();
        return;
    }

    query.exec("BEGIN;");
    if (query.exec("INSERT INTO message(message_time, message_text, id_chat, id_user) VALUES ('" +
                QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss") + "', '" +
                       messageText + "', " + QString::number(idChat) + ", " + QString::number(user->GetUserInfo().GetIdUser()) + ");"))
    {
        GeneralLog::GetLog()->Write("The message is accepted");
        user->GetConnectionStream() << "SUCCESS";
        user->SendData();

        QSqlQuery newMessage(MessengerDatabase::GetDB());   //Получаем только что добавленное сообщение
        newMessage.exec("SELECT id_chat, id_message, message_time, id_user, message_text FROM message ORDER BY id_message DESC LIMIT 1;");
        query.exec("COMMIT;");

        GeneralLog::GetLog()->Write("Initializing request CHAT UPDATE MESSAGES");
        ServerNetwork::GetServerNetwork()->SendRequest_CHAT_UPDATE_MESSAGES(&newMessage);
    }
    else
    {
        GeneralLog::GetLog()->Write("The message isn't accepted");
        query.exec("ROLLBACK;");
        user->GetConnectionStream() << "ERROR";
        user->SendData();
    }

    GeneralLog::GetLog()->Write("Command SEND MESSAGE is completed");
    return;
}
