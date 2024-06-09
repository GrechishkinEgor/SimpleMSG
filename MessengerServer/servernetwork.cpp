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
    networkHandlers[QString("REGISTER")] = new ServerCommand_REGISTER();
    networkHandlers[QString("CHAT CREATE")] = new ServerCommand_CHAT_CREATE();
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


///////////////////////////////////Запросы/////////////////////////////////////////


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
                GeneralLog::GetLog()->Write("Sending new message to connection " + QString::number((*i)->GetSavedSocketDescriptor()));
                (*i)->GetConnectionStream() << QString("CHAT UPDATE MESSAGES") << RequestID::NextID();
                this->SerializeSql((*i), messagesInfo);
                (*i)->SendData();
                break;
            }
}

void ServerNetwork::SendRequest_CHAT_UPDATE_LIST(qint32 idChat, qint32 idChatType, QString chatName, QVector<qint32> membersId)
{
    for (auto i = users.begin(); i != users.end(); i++)
        for (int j = 0; j < membersId.length(); j++)
            if ((*i)->GetUserInfo().GetIdUser() == membersId[j])
            {
                GeneralLog::GetLog()->Write("Sending new chat to connection " + QString::number((*i)->GetSavedSocketDescriptor()));
                (*i)->GetConnectionStream() << QString("CHAT UPDATE LIST")
                                            << RequestID::NextID()
                                            << idChat
                                            << idChatType
                                            << chatName;
                (*i)->SendData();
                break;
            }
}


///////////////////////////////////Обработчики сетевых событий/////////////////////////////////////////



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
    query.exec("SELECT chat.id_chat, chat.id_chat_type, chat.chat_name "
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

void ServerNetwork::ServerCommand_REGISTER::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    QString login;
    QString password;
    QString surname;
    QString userName;
    params >> login >> password >> surname >> userName;

    QSqlQuery query(MessengerDatabase::GetDB());
    if (query.exec("INSERT INTO user(login, password, surname, user_name, id_user_rights) "
                   "VALUES ('" + login + "', '" +
                                    password + "', '" +
                                    surname + "', '" +
                                    userName + "', 2);"))
    {
        GeneralLog::GetLog()->Write("User '" + login + "' was successfully registered");
        user->GetConnectionStream() << QString("SUCCESS");
    }
    else
    {
        GeneralLog::GetLog()->Write("User '" + login + "' has already registered");
        user->GetConnectionStream() << QString("ERROR") << QString("LOGIN EXISTS");
    }
    user->SendData();

    GeneralLog::GetLog()->Write("Command REGISTER is completed");
    return;
}

void ServerNetwork::ServerCommand_CHAT_CREATE::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    if (!user->GetUserInfo().IsAuthenticated())
    {
        GeneralLog::GetLog()->Write("User isn't authenticated. Command is declined");
        user->GetConnectionStream() << QString("ERROR") << QString("ANOTHER");
        user->SendData();

        GeneralLog::GetLog()->Write("Command CHAT CREATE is completed");
        return;
    }

    qint32 idChatType;
    QString chatName;
    quint32 membersCount;
    QVector<QString> logins;
    params >> idChatType >> chatName >> membersCount;
    for (quint32 i = 0; i < membersCount; i++)
    {
        QString login;
        params >> login;
        logins.push_back(login);
    }
    GeneralLog::GetLog()->Write("Received " + QString::number(membersCount) + " logins");

    //Получаем id пользователей, которые будут состоять в создаваемом чате
    QSqlQuery idUserQuery(MessengerDatabase::GetDB());
    QVector<qint32> idUsers;
    for (quint32 i = 0; i < membersCount; i++)
    {
        idUserQuery.exec("SELECT id_user FROM user WHERE login = '" + logins[i] + "';");
        if (!idUserQuery.first())
        {
            GeneralLog::GetLog()->Write("User '" + logins[i] + "' doesn't exist");
            user->GetConnectionStream() << QString("ERROR") << QString("USER NOT EXIST") << logins[i];
            user->SendData();

            GeneralLog::GetLog()->Write("Command CHAT CREATE is completed");
            return;
        }
        else
            idUsers.push_back(idUserQuery.value(0).toInt());
    }

    //Проверяем, что личный чат не создается с самим собой
    if (idChatType == 1 && logins[0] == user->GetUserInfo().GetLogin())
    {
        GeneralLog::GetLog()->Write("Attempt to create chat with yourself");
        user->GetConnectionStream() << QString("ERROR") << QString("ANOTHER");
        user->SendData();
        return;
    }

    //Проверяем для личного чата, что таковой до сих пор не существует
    if (idChatType == 1)
    {
        QSqlQuery chatNotExistQuery(MessengerDatabase::GetDB());
        chatNotExistQuery.exec("SELECT ch.id_chat FROM chat_member chm, chat ch WHERE chm.id_chat = ch.id_chat AND "
                               "ch.id_chat_type = 1 AND "
                               "chm.id_user = " + QString::number(user->GetUserInfo().GetIdUser()) +
                               " INTERSECT SELECT ch2.id_chat FROM chat_member chm2, chat ch2 WHERE chm2.id_chat = ch2.id_chat AND "
                               "ch2.id_chat_type = 1 AND "
                                "chm2.id_user = " + QString::number(idUsers[0]) + ";");
        if (chatNotExistQuery.first())
        {
            GeneralLog::GetLog()->Write("Personal chat between users " + QString::number(user->GetUserInfo().GetIdUser()) + " and " +
                                        QString::number(idUsers[0]) + " exists");
            user->GetConnectionStream() << QString("ERROR") << QString("PERSONAL CHAT EXISTS");
            user->SendData();

            GeneralLog::GetLog()->Write("Command CHAT CREATE is completed");
            return;
        }
    }

    //Меняем имя для личного чата
    if (idChatType == 1)
        chatName = user->GetUserInfo().GetLogin() + "/" + logins[0];

    QSqlQuery createChatQuery(MessengerDatabase::GetDB());
    createChatQuery.exec("BEGIN;");
    if (!createChatQuery.exec("INSERT INTO chat(chat_name, id_chat_type) VALUES ('"
                                  + chatName + "', " + QString::number(idChatType) + ");"))
        createChatQuery.exec("ROLLBACK;");
    else
    {
        //Получаем id нового чата
        createChatQuery.exec("SELECT id_chat FROM chat ORDER BY id_chat DESC LIMIT 1");
        createChatQuery.first();
        qint32 idChat = createChatQuery.value(0).toInt();

        //Формируем запрос на добавление участников чата
        QString queryText = "INSERT INTO chat_member(id_chat, id_user) VALUES ";
        for (int i = 0; i < idUsers.size(); i++)
            queryText += "(" + QString::number(idChat) + ", " + QString::number(idUsers[i]) + "), ";
        queryText += "(" + QString::number(idChat) + ", " + QString::number(user->GetUserInfo().GetIdUser()) + "); ";
        idUsers.push_back(user->GetUserInfo().GetIdUser());

        //GeneralLog::GetLog()->Write(queryText);
        createChatQuery.exec(queryText);
        createChatQuery.exec("COMMIT;");

        user->GetConnectionStream() << QString("SUCCESS");
        user->SendData();

        GeneralLog::GetLog()->Write("Command CHAT CREATE is completed");
        GeneralLog::GetLog()->Write("Initializing request CHAT UPDATE LIST");
        ServerNetwork::GetServerNetwork()->SendRequest_CHAT_UPDATE_LIST(idChat, idChatType, chatName, idUsers);
    }
    return;
}
