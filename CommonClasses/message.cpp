#include "message.h"
#include "chat.h"

Message::Message()
{
    idMessage = 0;
    messageTime = "UNDEFINED";
    messageText = "";
    sender = nullptr;
    messageChat = nullptr;
}

Chat *Message::GetChat()
{
    return messageChat;
}

void Message::SetChat(Chat *messageChat)
{
    this->messageChat = messageChat;
}
