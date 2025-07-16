#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dlg_ = new LoginDialog();
    login_dlg_->loadStyleSheet(":/style/login.qss");
    // setCentralWidget(login_dlg_);
    register_dlg_ = new RegisterDialog();
    register_dlg_->loadStyleSheet(":/style/register.qss");
    reset_dlg_ = new ResetDialog();
    reset_dlg_->loadStyleSheet(":/style/reset.qss");
    login_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    register_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    reset_dlg_->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    ui->stackedWidget->addWidget(login_dlg_);
    ui->stackedWidget->addWidget(register_dlg_);
    ui->stackedWidget->addWidget(reset_dlg_);
    ui->stackedWidget->setCurrentWidget(login_dlg_);
    // login_dlg_->show();
    // 创建和注册消息连接: 点击login的注册按钮, 跳转到注册界面
    connect(login_dlg_, &LoginDialog::switchRegister, this, &MainWindow::handleSwitchRegister);
    connect(login_dlg_, &LoginDialog::switchReset, this, &MainWindow::handleSwitchResetPwd);
    connect(register_dlg_, &RegisterDialog::switchLogin, this, &MainWindow::handleSwitchLogin);
    connect(reset_dlg_, &ResetDialog::switchLogin, this, &MainWindow::handleSwitchLogin);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadStyleSheet(const QString &file_path)
{
    QFile file(file_path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet); // Apply stylesheet ONLY to this dialog
        file.close();
    }
}

void MainWindow::handleSwitchRegister()
{
    ui->stackedWidget->setCurrentWidget(register_dlg_);

}

void MainWindow::handleSwitchLogin()
{
    ui->stackedWidget->setCurrentWidget(login_dlg_);
}

void MainWindow::handleSwitchResetPwd()
{
    ui->stackedWidget->setCurrentWidget(reset_dlg_);
}
