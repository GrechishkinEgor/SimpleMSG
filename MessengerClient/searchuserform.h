#ifndef SEARCHUSERFORM_H
#define SEARCHUSERFORM_H

#include <QDialog>

namespace Ui {
class SearchUserForm;
}

class SearchUserForm : public QDialog
{
    Q_OBJECT

public:
    explicit SearchUserForm(QWidget *parent = nullptr);
    ~SearchUserForm();

private slots:
    void on_searchButton_clicked();

private:
    Ui::SearchUserForm *ui;
};

#endif // SEARCHUSERFORM_H
