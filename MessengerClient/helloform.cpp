#include "helloform.h"
#include "ui_helloform.h"
#include "applicationcontext.h"

HelloForm::HelloForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HelloForm)
{
    ui->setupUi(this);
}

HelloForm::~HelloForm()
{
    delete ui;
}

void HelloForm::on_loginButton_clicked()
{
    ApplicationContext::GetContext()->ShowIdentificationForm();
}


void HelloForm::on_registrationButton_clicked()
{
    ApplicationContext::GetContext()->ShowRegistrationForm();
}

