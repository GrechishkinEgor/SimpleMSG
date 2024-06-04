#include "searchuserform.h"
#include "ui_searchuserform.h"

SearchUserForm::SearchUserForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchUserForm)
{
    ui->setupUi(this);
}

SearchUserForm::~SearchUserForm()
{
    delete ui;
}

void SearchUserForm::on_searchButton_clicked()
{

}

