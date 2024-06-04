#ifndef HELLOFORM_H
#define HELLOFORM_H

#include <QDialog>

namespace Ui {
class HelloForm;
}

class HelloForm : public QDialog
{
    Q_OBJECT

public:
    explicit HelloForm(QWidget *parent = nullptr);
    ~HelloForm();


private slots:
    void on_loginButton_clicked();

    void on_registrationButton_clicked();

private:
    Ui::HelloForm *ui;
};

#endif // HELLOFORM_H
