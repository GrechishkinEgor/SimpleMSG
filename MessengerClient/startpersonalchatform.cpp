#include "startpersonalchatform.h"
#include "ui_startpersonalchatform.h"
#include "applicationcontext.h"
#include <QMessageBox>

StartPersonalChatForm::StartPersonalChatForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartPersonalChatForm)
{
    ui->setupUi(this);
}

StartPersonalChatForm::~StartPersonalChatForm()
{
    delete ui;
}

void StartPersonalChatForm::ClearFields()
{
    ui->loginLine->setText("");
}

void StartPersonalChatForm::on_startChatButton_clicked()
{
    ui->loginLine->setText(ui->loginLine->text().trimmed());
    if (ui->loginLine->text() == "")
    {
        QMessageBox::information(this, "Ошибка", "Введите логин пользователя");
        return;
    }
    ApplicationContext::GetContext()->SendRequest_CHAT_CREATE(ui->loginLine->text());
}

