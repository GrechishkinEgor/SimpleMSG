#include "registrationform.h"
#include "ui_registrationform.h"
#include "user.h"
#include "applicationcontext.h"
#include <QMessageBox>

RegistrationForm::RegistrationForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
}

void RegistrationForm::ClearFields()
{
    ui->loginLine->setText("");
    ui->passwordLine->setText("");
    ui->repeatPasswordLine->setText("");
    ui->surnameLine->setText("");
    ui->nameLine->setText("");
    ui->registerButton->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);
}

void RegistrationForm::EnableRegisterButton()
{
    ui->registerButton->setEnabled(true);
    this->setCursor(Qt::ArrowCursor);
}

void RegistrationForm::on_registerButton_clicked()
{
    //Удаляем лишние пробелы
    ui->loginLine->setText(ui->loginLine->text().trimmed());
    ui->passwordLine->setText(ui->passwordLine->text().trimmed());
    ui->repeatPasswordLine->setText(ui->repeatPasswordLine->text().trimmed());
    ui->surnameLine->setText(ui->surnameLine->text().trimmed());
    ui->nameLine->setText(ui->nameLine->text().trimmed());

    if (ui->loginLine->text() == "")
    {
        QMessageBox::information(this, "Ошибка", "Введите логин");
        return;
    }
    if (ui->passwordLine->text() == "")
    {
        QMessageBox::information(this, "Ошибка", "Введите пароль");
        return;
    }
    if (ui->passwordLine->text() != ui->repeatPasswordLine->text())
    {
        QMessageBox::information(this, "Ошибка", "Введенные пароли не совпадают");
        return;
    }
    if (ui->surnameLine->text() == "")
    {
        QMessageBox::information(this, "Ошибка", "Введите фамилию");
        return;
    }
    if (ui->nameLine->text() == "")
    {
        QMessageBox::information(this, "Ошибка", "Введите имя");
        return;
    }

    ui->registerButton->setEnabled(false);
    this->setCursor(Qt::WaitCursor);
    User newUser;
    newUser.SetLogin(ui->loginLine->text());
    newUser.SetSurname(ui->surnameLine->text());
    newUser.SetUserName(ui->nameLine->text());
    ApplicationContext::GetContext()->SendRequest_REGISTER(&newUser, ui->passwordLine->text());
}

