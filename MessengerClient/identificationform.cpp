#include "identificationform.h"
#include "ui_identificationform.h"
#include "applicationcontext.h"

#include <QMessageBox>

IdentificationForm::IdentificationForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::IdentificationForm)
{
    ui->setupUi(this);
    ui->passwordLine->setEchoMode(QLineEdit::Password);
}

IdentificationForm::~IdentificationForm()
{
    delete ui;
}

void IdentificationForm::ClearFields()
{
    ui->loginLine->setText("");
    ui->passwordLine->setText("");
}

void IdentificationForm::on_loginButton_clicked()
{
    //Избавляемся от пробелов в начале и в конце строк
    ui->loginLine->setText(ui->loginLine->text().trimmed());
    ui->passwordLine->setText(ui->passwordLine->text().trimmed());

    if (ui->loginLine->text() == "" || ui->passwordLine->text() == "")
    {
        QMessageBox::information(this, "Ошибка", "Введен некорректный логин и/или пароль");
        return;
    }
    ApplicationContext::GetContext()->SendRequest_AUTHETICATE(ui->loginLine->text(), ui->passwordLine->text());
}

