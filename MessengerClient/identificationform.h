#ifndef IDENTIFICATIONFORM_H
#define IDENTIFICATIONFORM_H

#include <QDialog>

namespace Ui {
class IdentificationForm;
}

class IdentificationForm : public QDialog
{
    Q_OBJECT

public:
    explicit IdentificationForm(QWidget *parent = nullptr);
    ~IdentificationForm();

    void ClearFields();
private slots:
    void on_loginButton_clicked();

private:
    Ui::IdentificationForm *ui;
};

#endif // IDENTIFICATIONFORM_H
