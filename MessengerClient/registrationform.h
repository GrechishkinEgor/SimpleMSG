#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QDialog>

namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);
    ~RegistrationForm();

    void ClearFields();
    void EnableRegisterButton();

private slots:
    void on_registerButton_clicked();

private:
    Ui::RegistrationForm *ui;
};

#endif // REGISTRATIONFORM_H
