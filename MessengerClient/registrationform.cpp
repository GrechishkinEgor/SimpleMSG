#include "registrationform.h"
#include "ui_registrationform.h"

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
