#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "httpmanager.h"
#include <QMessageBox>
ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    setupUI();
    initHttpHandlers();
    connect(ui->verification_code, &VerificationCodeEdit::requestCodeClicked, this, &ResetDialog::handleRequestCode);
    connect(ui->confirm_btn, &QPushButton::clicked, this, &ResetDialog::handleRequestReset);
    connect(ui->cancel_btn, &QPushButton::clicked, this, &ResetDialog::switchLogin);
    connect(HttpManager::getInstance().get(), &HttpManager::sigResetModFinish,
            this, &ResetDialog::handleResetFinish);

}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::loadStyleSheet(const QString &file_path)
{
    QFile file(file_path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet); // Apply stylesheet ONLY to this dialog
        file.close();
    }
}


void ResetDialog::setupUI()
{
    ui->account_name->setPlaceholderText("账户名");
    ui->password->setPlaceholderText("密码");
    ui->email->setPlaceholderText("邮箱");
    ui->verification_code->setPlaceholderText("验证码");
    // 各个validate
    ui->account_name->setValidator([](const QString& text) {
        if (text.isEmpty()) {
            return "账户名不能为空";
        }
        if (text.length() < 6 || text.length() > 20) {
            return "账户名长度必须在6~20之间";
        }
        return "";
    });

    ui->password->setValidator([](const QString& text) {

        if (text.isEmpty()) {
            return "密码不能为空";
        }
        if (text.length() < 6) {
            return "密码长度不能少于6个字符";
        }
        bool has_letter = false, has_digit = false, has_symbol = false;
        for (const QChar& ch : text) {
            if (ch.isLetter()) has_letter = true;
            else if (ch.isDigit()) has_digit = true;
            else if (ch.isSymbol()) has_symbol = true;
        }
        int type_cnt = (has_symbol ? 1 :0) + (has_digit ? 1 : 0) + (has_letter ? 1 : 0);
        if (type_cnt < 2) {
            return "密码必须包含字母, 数字, 符号中的至少2种";
        }
        return "";
    });

    ui->email->setValidator([](const QString& text) {
        if (text.isEmpty()) {
            return "邮箱不能为空";
        }

        static QRegularExpression regex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
        if (!regex.match(text).hasMatch()) {
            return "请输入有效的邮箱格式";
        }
        return "";
    });
    // 验证码输入框不需要验证器
    ui->verification_code->setValidator([](const QString& text) {
        if (text.isEmpty()) {
            return "验证码不能为空";
        }
        return "";
    });

    // loading_widget, 一开始设置为false
    ui->loading_widget->setRoundness(50.0);
    ui->loading_widget->setMinimumTrailOpacity(15.0);
    ui->loading_widget->setTrailFadePercentage(70.0);
    ui->loading_widget->setNumberOfLines(10);
    ui->loading_widget->setLineLength(10);
    ui->loading_widget->setLineLength(3);
    ui->loading_widget->setInnerRadius(8);
    ui->loading_widget->setRevolutionsPerSecond(1);
    ui->loading_widget->setColor(QColor(0, 153, 255));
    ui->loading_widget->stop();
    ui->loading_widget->hide();
}

void ResetDialog::initHttpHandlers()
{
    handlers_.insert(ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject& json_obj) {

        int error = json_obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            // showTip(tr("参数错误"), false);
            showTipMessageBox("错误提示", "参数错误", 5000);
            return;
        }
        auto email = json_obj["email"].toString();
        // showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug() << "email: " << email;
    });
    handlers_.insert(ReqId::ID_RESET_PWD, [this](const QJsonObject& json_obj) {

        int error = json_obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            // showTip(tr("参数错误"), false);
            showTipMessageBox("错误提示", "参数错误", 5000);
            return;
        }
        auto email = json_obj["email"].toString();
        // showTip(tr("注册成功"), true);
        showTipMessageBox("", "注册成功, 5s后返回登录界面", 5000);
        qDebug() << "email: " << email << "succussful register";
        emit switchLogin();
    });
}

void ResetDialog::showTipMessageBox(const QString &title, const QString &text, int timeout)
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


void ResetDialog::handleRequestCode()
{
    // 验证email栏有无问题
    ui->email->validate();
    if (!ui->email->isValid()) {
        return;
    }
    // 验证成功就倒计时同时发送验证码
    ui->verification_code->startTimer();
    if (ui->email->isValid()) {
        // showTip(tr("请求发送中..."), true);
        QJsonObject post_json;
        post_json["email"] = ui->email->text();
        HttpManager::getInstance()->postHttpReq(QUrl(gate_server_prefix + "/post_verifycode"),
                                                post_json, ReqId::ID_GET_VERIFY_CODE, Modules::RESETPWDMOD);
    }
    // 否则email栏会给出提示
}

void ResetDialog::handleRequestReset()
{
    ui->account_name->validate();
    ui->password->validate();
    ui->email->validate();
    ui->verification_code->validate();
    if (!ui->account_name->isValid()) {
        return;
    }

    if (!ui->password->isValid()) {
        return;
    }

    if (!ui->email->isValid()) {
        return;
    }

    if (!ui->verification_code->isValid()) {
        return;
    }
    // showTip(tr("注册验证中..."),  true);
    // 动画模拟请求转圈效果
    ui->loading_widget->show();
    ui->loading_widget->start();
    // 发送注册请求
    QJsonObject json_obj;
    json_obj["user"] = ui->account_name->text();
    json_obj["email"] = ui->email->text();
    json_obj["pass"] = ui->password->text();
    json_obj["verify_code"] = ui->verification_code->text();
    HttpManager::getInstance()->postHttpReq(QUrl(gate_server_prefix + "/user_resetpwd"),
                                            json_obj, ReqId::ID_RESET_PWD, Modules::RESETPWDMOD);
}

void ResetDialog::handleResetFinish(ReqId id, QString res, ErrorCodes err)
{
    ui->loading_widget->stop();
    ui->loading_widget->hide();
    if (err != ErrorCodes::SUCCESS) {
        // showTip(tr("网络请求错误"), false);
        showTipMessageBox("错误提示", "网络请求错误", 5000);
        return;
    }

    // 解析json字符串, res转为QByteArray
    QJsonDocument json_doc = QJsonDocument::fromJson(res.toUtf8());
    if (json_doc.isNull() || !json_doc.isObject()) {
        // showTip(tr("json解析失败"), false);
        showTipMessageBox("错误提示", "json解析失败", 5000);
        return;
    }
    // 具体的处理函数中处理
    handlers_[id](json_doc.object());
    return;
}
