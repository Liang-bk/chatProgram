#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadStyleSheet(const QString& file_path);
public slots:
    void handleSwitchRegister();
    void handleSwitchLogin();
    void handleSwitchResetPwd();
private:
    Ui::MainWindow *ui;
    LoginDialog *login_dlg_;
    RegisterDialog *register_dlg_;
    ResetDialog *reset_dlg_;

};
#endif // MAINWINDOW_H
