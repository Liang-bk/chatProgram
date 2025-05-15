#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dlg_ = new LoginDialog(this);
    setCentralWidget(login_dlg_);
    // login_dlg_->show();
    // 创建和注册消息连接: 点击login的注册按钮, 跳转到注册界面
    connect(login_dlg_, &LoginDialog::switchRegister, this, &MainWindow::slotSwitchRegister);
    register_dlg_ = new RegisterDialog(this);


    login_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    register_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    register_dlg_->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    // if (login_dlg_) {
    //     delete login_dlg_;
    //     login_dlg_ = nullptr;
    // }
    // if (register_dlg_) {
    //     delete register_dlg_;
    //     register_dlg_ = nullptr;
    // }
}

void MainWindow::slotSwitchRegister()
{
    setCentralWidget(register_dlg_);
    // 隐藏登录界面
    login_dlg_->hide();
    // 显示注册界面
    register_dlg_->show();

}
