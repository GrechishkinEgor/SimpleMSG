#ifndef STARTPERSONALCHATFORM_H
#define STARTPERSONALCHATFORM_H

#include <QDialog>

namespace Ui {
class StartPersonalChatForm;
}

class StartPersonalChatForm : public QDialog
{
    Q_OBJECT

public:
    explicit StartPersonalChatForm(QWidget *parent = nullptr);
    ~StartPersonalChatForm();

    void ClearFields();
private slots:
    void on_startChatButton_clicked();

private:
    Ui::StartPersonalChatForm *ui;
};

#endif // STARTPERSONALCHATFORM_H
