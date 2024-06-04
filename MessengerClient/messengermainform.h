#ifndef MESSENGERMAINFORM_H
#define MESSENGERMAINFORM_H

#include <QMainWindow>
#include "message.h"
#include "chat.h"

namespace Ui {
class MessengerMainForm;
}

class MessengerMainForm : public QMainWindow
{
    Q_OBJECT
private:
    QVector<Chat*> chatList;
    Chat* selectedChat;

public:
    explicit MessengerMainForm(QWidget *parent = nullptr);
    ~MessengerMainForm();

    void ViewChatList();
    void ViewChatMessages(Chat* currentChat);
    void ViewMessage(Message* currentMessage);

private slots:
    void on_action_triggered();
    void on_chatList_clicked(const QModelIndex &index);
    void on_sendButton_clicked();

private:
    Ui::MessengerMainForm *ui;
};

#endif // MESSENGERMAINFORM_H
