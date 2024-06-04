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

private:
    Ui::RegistrationForm *ui;
};

#endif // REGISTRATIONFORM_H
