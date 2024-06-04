#ifndef AUTHENTICATIONFORM_H
#define AUTHENTICATIONFORM_H

#include <QDialog>

namespace Ui {
class AuthenticationForm;
}

class AuthenticationForm : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenticationForm(QWidget *parent = nullptr);
    ~AuthenticationForm();

private:
    Ui::AuthenticationForm *ui;
};

#endif // AUTHENTICATIONFORM_H
