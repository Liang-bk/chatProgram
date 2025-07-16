#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void loadStyleSheet(const QString& file_path);
private:
    void showTipMessageBox(const QString& title, const QString& text, int timeout = 5000);
private:
    Ui::LoginDialog *ui;
signals:
    void switchRegister();
    void switchReset();
};

#endif // LOGINDIALOG_H
