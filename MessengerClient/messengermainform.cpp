#include "messengermainform.h"
#include "ui_messengermainform.h"
#include "applicationcontext.h"
#include "userconnection.h"
#include "chat.h"

MessengerMainForm::MessengerMainForm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MessengerMainForm)
{
    ui->setupUi(this);
    UserConnection& currentUser = ApplicationContext::GetContext()->GetUserConnection();
    ui->userInfo->setText("Пользователь: " + currentUser.GetUserInfo().GetSurname() + " " + currentUser.GetUserInfo().GetUserName());
}

MessengerMainForm::~MessengerMainForm()
{
    delete ui;
}

void MessengerMainForm::ViewChatList()
{
    QMap<qint32, Chat*>& chats = ApplicationContext::GetContext()->GetChats();
    ui->chatList->clear();
    this->chatList.clear();

    for (auto i = chats.begin(); i != chats.end(); i++)
    {
        ui->chatList->addItem((*i)->GetChatName());
        this->chatList.push_back(*i);
    }
}

void MessengerMainForm::ViewChatMessages(Chat* currentChat)
{
    selectedChat = currentChat;
    QMap<qint32, Message*>& messages = currentChat->GetMessages();
    ui->chatMessages->clear();

    for (auto i = messages.begin(); i != messages.end(); i++)
        ui->chatMessages->append((*i)->GetMessageTime() + ". " +
                                 (*i)->GetSender()->GetSurname() + " " + (*i)->GetSender()->GetUserName() +
                                 ": " + (*i)->GetMessageText());

    ui->chatInfo->setText("Чат: " + currentChat->GetChatName());
    ui->sendButton->setEnabled(true);
    ui->newMessage->setEnabled(true);
}

void MessengerMainForm::ViewMessage(Message *currentMessage)
{
    ui->chatMessages->append(currentMessage->GetMessageTime() + ". " +
                             currentMessage->GetSender()->GetSurname() + " " + currentMessage->GetSender()->GetUserName() +
                             ": " + currentMessage->GetMessageText());
}

void MessengerMainForm::ViewChat(Chat *currentChat)
{
    ui->chatList->addItem(currentChat->GetChatName());
    this->chatList.push_back(currentChat);
}

void MessengerMainForm::FocusOnChat(Chat *currentChat)
{
    for (int i = 0; i < this->chatList.size(); i++)
        if (currentChat == this->chatList[i])
        {
            ui->chatList->setCurrentRow(i);
            break;
        }
}

void MessengerMainForm::on_action_triggered()
{
    ApplicationContext::GetContext()->ShowStartPersonalChatWindow();
}


void MessengerMainForm::on_chatList_clicked(const QModelIndex &index)
{
    ApplicationContext::GetContext()->SendRequest_CHAT_MESSAGES(this->chatList[ui->chatList->currentRow()]);
}


void MessengerMainForm::on_sendButton_clicked()
{
    if (ui->newMessage->text().trimmed() == "")
        return;
    Message newMessage;
    newMessage.SetChat(selectedChat);
    newMessage.SetMessageText(ui->newMessage->text());
    ApplicationContext::GetContext()->SendRequest_SEND_MESSAGE(&newMessage);
    ui->newMessage->setText("");
}

