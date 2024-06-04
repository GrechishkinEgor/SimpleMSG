#ifndef CHAT_H
#define CHAT_H

#include <QMap>
#include "message.h"
#include "user.h"

/*Чат мессенджера*/
class Chat
{
private:
    int idChat;
    QString chatName;
    int idChatType;

    bool messagesDownloadedFlag;        //показывает, скачивались ли сообщения с сервера (запрос CHAT MESSAGES)
    bool membersDownloadedFlag;         //показывает, скачивались ли участники чата с сервера (запрос CHAT MEMBERS)
    QMap<qint32, Message*> messages;
    QMap<qint32, User*> chatMembers;
public:
    Chat();

    int GetIdChat() { return idChat; }
    QString GetChatName() { return chatName; }
    int GetIdChatType() { return idChatType; }
    QMap<qint32, Message*>& GetMessages() { return messages; }
    QMap<qint32, User*>& GetChatMembers() { return chatMembers; }
    bool GetMessagesDownloadedFlag() { return messagesDownloadedFlag; }
    bool GetMembersDownloadedFlag() { return membersDownloadedFlag; }

    void SetIdChat(int idChat) { this->idChat = idChat; };
    void SetChatName(QString chatName) { this->chatName = chatName; };
    void SetIdChatType(int idChatType) { this->idChatType = idChatType; }
    void SetMessagesDownloadedFlag(bool flag) { this->messagesDownloadedFlag = flag; }
    void SetMembersDownloadedFlag(bool flag) { this->membersDownloadedFlag = flag; }
};

#endif // CHAT_H
