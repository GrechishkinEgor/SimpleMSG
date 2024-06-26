#include <QMessageBox>
#include <QDateTime>

#include "applicationcontext.h"
#include "generallog.h"
#include "chat.h"
#include "requestid.h"

ApplicationContext* ApplicationContext::instance = nullptr;

ApplicationContext::ApplicationContext()
{
    helloWindow = new HelloForm();
    identificationWindow = nullptr;
    messengerMainWindow = nullptr;
    registrationWindow = nullptr;
    startPersonalChatWindow = nullptr;
    selectedChat = nullptr;

    //Подключаемся к серверу
    GeneralLog::GetLog()->Write("Trying to connect to server");
    connect(&currentUser, &QTcpSocket::connected, this, &ApplicationContext::ConnectToServerSuccess);       //подключение к серверу прошло успешно
    connect(&currentUser, &QTcpSocket::readyRead, &mainHandler, &NetworkEventMainHandler::SocketReadyRead); //появились данные в сокете
    connect(&currentUser, &QTcpSocket::disconnected, this, &ApplicationContext::ServerDisconnected);        //соединение разорвано
    currentUser.connectToHost("127.0.0.1", 8000);

    //Заводим таймер на случай, если не удается подключиться к серверу
    waitingNetworkEventTimer = new QTimer();
    connect(waitingNetworkEventTimer, &QTimer::timeout, this, &ApplicationContext::ConnectToServerTimeout);
    waitingNetworkEventTimer->start(5000);

    //Инициализируем обработчики сетевых событий
    mainHandler.GetHandlers()["AUTHENTICATE"] = new ServerAnswer_AUTHENTICATE();
    mainHandler.GetHandlers()["CHAT LIST"] = new Server_Answer_CHAT_LIST();
    mainHandler.GetHandlers()["CHAT MESSAGES"] = new Server_Answer_CHAT_MESSAGES();
    mainHandler.GetHandlers()["SEND MESSAGE"] = new Server_Answer_SEND_MESSAGE();
    mainHandler.GetHandlers()["CHAT UPDATE MESSAGES"] = new Server_Answer_CHAT_UPDATE_MESSAGES();
    mainHandler.GetHandlers()["REGISTER"] = new Server_Answer_REGISTER();
    mainHandler.GetHandlers()["CHAT CREATE"] = new Server_Answer_CHAT_CREATE();
    mainHandler.GetHandlers()["CHAT UPDATE LIST"] = new Server_Answer_CHAT_UPDATE_LIST();
    mainHandler.SetUnrecognizedEventHandler(new Server_Answer_Unrecognized_Command());
}

ApplicationContext::~ApplicationContext()
{
    currentUser.close();
    if (helloWindow)
        delete helloWindow;
    if (identificationWindow)
        delete identificationWindow;
    if (messengerMainWindow)
        delete messengerMainWindow;
    if (registrationWindow)
        delete registrationWindow;
    if (startPersonalChatWindow)
        delete startPersonalChatWindow;

    if (waitingNetworkEventTimer)
    {
        waitingNetworkEventTimer->stop();
        delete waitingNetworkEventTimer;
    }
}

ApplicationContext* ApplicationContext::GetContext()
{
    if (!instance)
        instance = new ApplicationContext();
    return instance;
}



void ApplicationContext::SetApplicationMainEventLoop(QApplication *eventLoop)
{
    this->applicationMainEventLoop = eventLoop;
}

void ApplicationContext::ShowHelloForm()
{
    if (!helloWindow)
        helloWindow = new HelloForm();
    helloWindow->show();
}

void ApplicationContext::ShowIdentificationForm()
{
    if (!identificationWindow)
        identificationWindow = new IdentificationForm();
    identificationWindow->ClearFields();
    identificationWindow->exec();
}

void ApplicationContext::ShowRegistrationForm()
{
    if (!registrationWindow)
        registrationWindow = new RegistrationForm();
    registrationWindow->ClearFields();
    registrationWindow->exec();
}

QMap<qint32, Chat *>& ApplicationContext::GetChats()
{
    return this->chats;
}

QMap<qint32, User *> &ApplicationContext::GetUsers()
{
    return this->users;
}

UserConnection &ApplicationContext::GetUserConnection()
{
    return currentUser;
}

void ApplicationContext::ConnectToServerTimeout()
{
    GeneralLog::GetLog()->Write("Error to connect to server");
    QMessageBox::critical(helloWindow, "СимплМСГ", "Отсутствует подключение к серверу");
    applicationMainEventLoop->exit(0);  //Завершаем приложение
}

void ApplicationContext::ConnectToServerSuccess()
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Connecting to server success");
    ShowHelloForm();
    waitingNetworkEventTimer->stop();
    delete waitingNetworkEventTimer;
    waitingNetworkEventTimer = nullptr;
}

void ApplicationContext::ServerDisconnected()
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Server disconnected");
    QMessageBox::critical(helloWindow, "СимплМСГ", "Соединение с сервером разорвано");
    applicationMainEventLoop->exit(0);      //Закрываем приложение
}

void ApplicationContext::ShowMessengerMainForm()
{
    if (!messengerMainWindow)
        messengerMainWindow = new MessengerMainForm();
    messengerMainWindow->show();

    if (helloWindow)
        helloWindow->hide();
    if (identificationWindow)
        identificationWindow->hide();
}

void ApplicationContext::ShowStartPersonalChatWindow()
{
    if (!startPersonalChatWindow)
        startPersonalChatWindow = new StartPersonalChatForm();
    startPersonalChatWindow->ClearFields();
    startPersonalChatWindow->exec();
}

void ApplicationContext::HideCreatingChatWindows()
{
    if (startPersonalChatWindow)
        startPersonalChatWindow->hide();
}



///////////////////////////////////Запросы/////////////////////////////////////////




void ApplicationContext::SendRequest_AUTHETICATE(QString login, QString password)
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Trying to authenticate user");
    currentUser.GetConnectionStream() << QString("AUTHENTICATE") << RequestID::NextID() << login << password;
    currentUser.SendData();
}


void ApplicationContext::SendRequest_CHAT_LIST()
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Trying to get chat list");
    currentUser.GetConnectionStream() << QString("CHAT LIST") << RequestID::NextID();
    currentUser.SendData();
}

void ApplicationContext::SendRequest_CHAT_MESSAGES(Chat *selectedChat)
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Trying to download chat messages");
    this->selectedChat = selectedChat;
    if (!selectedChat->GetMessagesDownloadedFlag())
    {
        GeneralLog::GetLog()->Write("Send request to server");
        quint32 requestId = RequestID::NextID();
        Server_Answer_CHAT_MESSAGES* handler = (Server_Answer_CHAT_MESSAGES*)mainHandler.GetHandlers()["CHAT MESSAGES"];
        handler->AddRequest(requestId, selectedChat);

        currentUser.GetConnectionStream() << QString("CHAT MESSAGES") << requestId << qint32(selectedChat->GetIdChat());
        currentUser.SendData();
    }
    else
    {
        GeneralLog::GetLog()->Write("No need to download chat messages");
        messengerMainWindow->ViewChatMessages(selectedChat);
    }
}

void ApplicationContext::SendRequest_SEND_MESSAGE(Message *newMessage)
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Trying to send message");
    currentUser.GetConnectionStream() << QString("SEND MESSAGE") << RequestID::NextID() << newMessage->GetChat()->GetIdChat() << newMessage->GetMessageText();
    currentUser.SendData();
}

void ApplicationContext::SendRequest_REGISTER(User *newUser, QString password)
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Trying to register user");
    currentUser.GetConnectionStream() << QString("REGISTER") << RequestID::NextID()
                                      << newUser->GetLogin()
                                      << password
                                      << newUser->GetSurname()
                                      << newUser->GetUserName();
    currentUser.SendData();
}

void ApplicationContext::SendRequest_CHAT_CREATE(QString login)
{
    GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ". Trying to create personal chat");
    currentUser.GetConnectionStream() << QString("CHAT CREATE")
                                      << RequestID::NextID()
                                      << qint32(1)
                                      << QString("PERSONAL CHAT DEFAULT NAME")
                                      << quint32(1)
                                      << login;
    currentUser.SendData();
}



///////////////////////////////////Обработчики сетевых событий/////////////////////////////////////////



void ApplicationContext::ServerAnswer_AUTHENTICATE::Handle(quint32 requestId, UserConnection* user, QDataStream& params)
{
    user->ClearConnectionStream();  //Отправка ответного сообщения не требуется

    QString authenticationResult;
    params >> authenticationResult;
    if (authenticationResult == QString("SUCCESS"))
    {
        qint32 idUser;
        QString surname;
        QString name;
        params >> idUser >> surname >> name;

        user->GetUserInfo().SetIdUser(idUser);
        user->GetUserInfo().SetSurname(surname);
        user->GetUserInfo().SetUserName(name);
        user->GetUserInfo().SetAuthenticatedFlag(true);

        GeneralLog::GetLog()->Write("The user is successfully autheticated");
        ApplicationContext::GetContext()->SendRequest_CHAT_LIST();
        ApplicationContext::GetContext()->ShowMessengerMainForm();
    }
    else
    {
        GeneralLog::GetLog()->Write("Authetication is failed");
        QMessageBox::critical(ApplicationContext::GetContext()->identificationWindow, "Ошибка", "Неверный логин и/или пароль");
    }
}

void ApplicationContext::Server_Answer_CHAT_LIST::Handle(quint32 requestId, UserConnection* user, QDataStream& params)
{
    user->ClearConnectionStream();  //Отправка ответного сообщения не требуется
    quint32 chatCount;
    ApplicationContext* context = ApplicationContext::GetContext();

    params >> chatCount;
    GeneralLog::GetLog()->Write("Received " + QString::number(chatCount) + " chat(s)");
    for (quint32 i = 0; i < chatCount; i++)
    {
        qint32 idChat;
        qint32 idChatType;
        QString chatName;

        QString buffer;
        params >> buffer;
        idChat = buffer.toInt();
        params >> buffer;
        idChatType = buffer.toInt();
        params >> chatName;

        //Проверяем, существует ли уже такой чат. Если нет, то создаем
        Chat* currentChat = context->chats[idChat];
        if (currentChat == nullptr)
        {
            currentChat = new Chat();
            context->chats[idChat] = currentChat;
        }

        currentChat->SetIdChat(idChat);
        currentChat->SetIdChatType(idChatType);
        currentChat->SetChatName(chatName);
    }

    context->messengerMainWindow->ViewChatList();
}

void ApplicationContext::Server_Answer_CHAT_MESSAGES::Handle(quint32 requestId, UserConnection* user, QDataStream& params)
{
    user->ClearConnectionStream();                          //Отправка ответного сообщения не требуется

    Chat* currentChat = this->requestList[requestId];       //Достаем чат, для которого пришли сообщения
    if (currentChat == nullptr)
        GeneralLog::GetLog()->Write("Chat in request isn't found. Next messages will be skipped");
    this->requestList[requestId] = nullptr;

    //Получаем участников чата
    quint32 membersCount;
    params >> membersCount;
    GeneralLog::GetLog()->Write("Received " + QString::number(membersCount) + " members(s)");

    for (quint32 i = 0; i < membersCount; i++)
    {
        qint32 idUser;
        QString surname;
        QString userName;

        QString buffer;
        params >> buffer;
        idUser = buffer.toInt();
        params >> surname >> userName;

        if (currentChat)
        {
            //Проверяем, есть ли уже такой пользователь. Если нет, то создаем
            User* currentUser = ApplicationContext::GetContext()->GetUsers()[idUser];
            if (!currentUser)
            {
                currentUser = new User();
                ApplicationContext::GetContext()->GetUsers()[idUser] = currentUser;
            }
            currentChat->GetChatMembers()[idUser] = currentUser;

            currentUser->SetIdUser(idUser);
            currentUser->SetSurname(surname);
            currentUser->SetUserName(userName);
        }
    }
    if (currentChat)
        currentChat->SetMembersDownloadedFlag(true);

    //Получаем сообщения чата
    quint32 messageCount;
    params >> messageCount;
    GeneralLog::GetLog()->Write("Received " + QString::number(messageCount) + " message(s)");
    for (quint32 i = 0; i < messageCount; i++)
    {
        qint32 idMessage;
        QString messageTime;
        qint32 idUser;
        QString messageText;

        QString buffer;
        params >> buffer;
        idMessage = buffer.toInt();
        params >> messageTime;
        params >> buffer;
        idUser = buffer.toInt();
        params >> messageText;

        if (currentChat)
        {
            //Проверяем, есть ли уже такое сообщение. Если нет, то создаем
            Message* currentMessage = currentChat->GetMessages()[idMessage];
            if (!currentMessage)
            {
                currentMessage = new Message();
                currentChat->GetMessages()[idMessage] = currentMessage;
            }

            currentMessage->SetIdMessage(idMessage);
            currentMessage->SetMessageTime(messageTime);
            currentMessage->SetSender(ApplicationContext::GetContext()->users[idUser]);
            currentMessage->SetMessageText(messageText);
        }
    }
    if (currentChat)
    {
        currentChat->SetMessagesDownloadedFlag(true);
        ApplicationContext::GetContext()->messengerMainWindow->ViewChatMessages(currentChat);
    }
}

void ApplicationContext::Server_Answer_CHAT_MESSAGES::AddRequest(quint32 requestId, Chat *requiredChat)
{
    requestList[requestId] = requiredChat;
}

void ApplicationContext::Server_Answer_SEND_MESSAGE::Handle(quint32 requestId, UserConnection* user, QDataStream& params)
{
    user->ClearConnectionStream();                          //Отправка ответного сообщения не требуется

    QString status;
    params >> status;
    if (status == "ERROR")
    {
        GeneralLog::GetLog()->Write("The message wasn't sent");
        QMessageBox::critical(ApplicationContext::GetContext()->messengerMainWindow, "Ошибка", "Ошибка при отправке сообщения");
    }
    else
        GeneralLog::GetLog()->Write("The message was sent");
}


void ApplicationContext::Server_Answer_Unrecognized_Command::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    //Ничего не делать при неопознанной команде

}

void ApplicationContext::Server_Answer_CHAT_UPDATE_MESSAGES::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    user->ClearConnectionStream();                          //Отправка ответного сообщения не требуется

    quint32 messageCount;
    params >> messageCount;
    GeneralLog::GetLog()->Write("Received " + QString::number(messageCount) + " message(s)");
    for (quint32 i = 0; i < messageCount; i++)
    {
        qint32 idChat;
        qint32 idMessage;
        QString messageTime;
        qint32 idUser;
        QString messageText;

        QString buffer;
        params >> buffer;
        idChat = buffer.toInt();
        params >> buffer;
        idMessage = buffer.toInt();
        params >> messageTime;
        params >> buffer;
        idUser = buffer.toInt();
        params >> messageText;

        Chat* currentChat = ApplicationContext::GetContext()->GetChats()[idChat];
        if (!currentChat)
        {
            GeneralLog::GetLog()->Write("Message " + QString::number(idMessage) +
                                        ": can't find chat " + QString::number(idChat));
            continue;
        }

        //Проверяем, есть ли уже такое сообщение. Если нет, то создаем
        Message* currentMessage = currentChat->GetMessages()[idMessage];
        if (!currentMessage)
        {
            currentMessage = new Message();
            currentChat->GetMessages()[idMessage] = currentMessage;
        }

        currentMessage->SetIdMessage(idMessage);
        currentMessage->SetMessageTime(messageTime);
        currentMessage->SetSender(ApplicationContext::GetContext()->users[idUser]);
        currentMessage->SetMessageText(messageText);

        if (currentChat == ApplicationContext::GetContext()->selectedChat)
            ApplicationContext::GetContext()->messengerMainWindow->ViewMessage(currentMessage);
    }
}

void ApplicationContext::Server_Answer_REGISTER::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    user->ClearConnectionStream();

    QString result;
    params >> result;
    if (result == "SUCCESS")
    {
        GeneralLog::GetLog()->Write("Registration is successfull");
        QMessageBox::information(ApplicationContext::GetContext()->registrationWindow, "Новый пользователь", "Новый пользователь успешно зарегистрирован");
        ApplicationContext::GetContext()->registrationWindow->hide();
    }
    if (result == "ERROR")
    {
        QString errorStr;
        params >> errorStr;
        GeneralLog::GetLog()->Write("Registration is failed. Server error: " + errorStr);
        if (errorStr == "LOGIN EXISTS")
            QMessageBox::information(ApplicationContext::GetContext()->registrationWindow, "Новый пользователь", "Указанный логин уже существует");
        else if (errorStr == "ANOTHER")
            QMessageBox::information(ApplicationContext::GetContext()->registrationWindow, "Новый пользователь", "Отказано в создании нового пользователя");
        ApplicationContext::GetContext()->registrationWindow->EnableRegisterButton();
    }
}

void ApplicationContext::Server_Answer_CHAT_CREATE::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    user->ClearConnectionStream();

    QString status;
    params >> status;
    if (status == "ERROR")
    {
        GeneralLog::GetLog()->Write("The chat wasn't created");
        QWidget* parentWindow = nullptr;
        if (this->isPersonalChat)
            parentWindow = ApplicationContext::GetContext()->startPersonalChatWindow;

        QString reason;
        params >> reason;
        if (reason == "USER NOT EXIST")
        {
            QString login;
            params >> login;

            GeneralLog::GetLog()->Write("User '" + login + "' isn't detected");
            QMessageBox::critical(parentWindow, "Ошибка", "Пользователь '" + login + "' не обнаружен");
        }
        else if (reason == "PERSONAL CHAT EXISTS")
        {
            GeneralLog::GetLog()->Write("Personal chat already exists");
            QMessageBox::critical(parentWindow, "Ошибка", "Чат с этим пользователем уже существует");
        }
        else
        {
            GeneralLog::GetLog()->Write("Unknown reason");
            QMessageBox::critical(parentWindow, "Ошибка", "Отказано в создании чата");
        }
    }
    else
    {
        GeneralLog::GetLog()->Write("The chat was created");
        ApplicationContext::GetContext()->HideCreatingChatWindows();
    }

}

void ApplicationContext::Server_Answer_CHAT_CREATE::SetPersonalChatFlag(bool flag)
{
    isPersonalChat = flag;
}

void ApplicationContext::Server_Answer_CHAT_UPDATE_LIST::Handle(quint32 requestId, UserConnection *user, QDataStream &params)
{
    user->ClearConnectionStream();                          //Отправка ответного сообщения не требуется

    ApplicationContext* context = ApplicationContext::GetContext();

    qint32 idChat;
    qint32 idChatType;
    QString chatName;

    params >> idChat >> idChatType>> chatName;

    //Проверяем, существует ли уже такой чат. Если нет, то создаем
    Chat* currentChat = context->chats[idChat];
    if (currentChat == nullptr)
    {
        currentChat = new Chat();
        context->chats[idChat] = currentChat;
    }

    currentChat->SetIdChat(idChat);
    currentChat->SetIdChatType(idChatType);
    currentChat->SetChatName(chatName);

    context->messengerMainWindow->ViewChat(currentChat);
}
