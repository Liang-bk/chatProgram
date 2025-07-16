#include "logindialog.h"
#include "ui_logindialog.h"
#include <QFile>
#include <QTimer>
#include <QMessageBox>
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->register_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    connect(ui->forget_btn, &QPushButton::clicked, this, &LoginDialog::switchReset);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::loadStyleSheet(const QString &file_path)
{
    QFile file(file_path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet); // Apply stylesheet ONLY to this dialog
        file.close();
    }
}

void LoginDialog::showTipMessageBox(const QString &title, const QString &text, int timeout)
{
    QMessageBox msgBox(this);

    // 【核心】设置唯一的对象名，让QSS选择器能够精确定位
    msgBox.setObjectName("AutoClosingMsgBox");
    QFile file(":/style/msgbox.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        msgBox.setStyleSheet(styleSheet);
        file.close();
    }

    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);

    QPushButton *okButton = static_cast<QPushButton*>(msgBox.button(QMessageBox::Ok));
    int secondsLeft = timeout / 1000;
    okButton->setText(QString("确定 (%1)").arg(secondsLeft));

    QTimer timer(&msgBox);
    timer.setInterval(1000);

    QObject::connect(&timer, &QTimer::timeout, &msgBox, [&]() {
        secondsLeft--;
        okButton->setText(QString("确定 (%1)").arg(secondsLeft));

        if (secondsLeft <= 0) {
            timer.stop();
            msgBox.accept();
        }
    });

    timer.start();
    msgBox.exec();
}
