#include "authenticationform.h"
#include "ui_authenticationform.h"

AuthenticationForm::AuthenticationForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuthenticationForm)
{
    ui->setupUi(this);
}

AuthenticationForm::~AuthenticationForm()
{
    delete ui;
}
