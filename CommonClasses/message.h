#ifndef MESSAGE_H
#define MESSAGE_H

#include "user.h"

class Chat;

/*Содержит данные конкретного сообщения, отправленного пользователем мессенджера*/
class Message
{
    qint32 idMessage;
    QString messageTime;
    QString messageText;

    User* sender;
    Chat* messageChat;
public:
    Message();

    int GetIdMessage() { return idMessage;}
    QString GetMessageTime() { return messageTime; }
    QString GetMessageText() { return messageText; }
    User* GetSender() { return sender; }
    Chat* GetChat();

    void SetIdMessage(qint32 idMessage) { this->idMessage = idMessage; }
    void SetMessageTime(QString messageTime) { this->messageTime = messageTime; }
    void SetMessageText(QString messageText) { this->messageText = messageText; }
    void SetSender(User* sender) { this->sender = sender; }
    void SetChat(Chat* messageChat);
};

#endif // MESSAGE_H
