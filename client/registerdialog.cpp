#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmanager.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    // 文本框输入不显示原始字符
    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    connect(HttpManager::getInstance().get(), &HttpManager::sigRegisterModFinish,
            this, &RegisterDialog::slotRegisterModFinish);
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_clicked()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if (match) {
        // 发送验证码
        showTip(tr("请求发送中..."), true);
        QJsonObject post_json;
        post_json["email"] = email;
        HttpManager::getInstance()->postHttpReq(QUrl(gate_server_prefix + "/post_verifycode"),
                                                post_json, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);


    } else {
        showTip(tr("邮箱地址不正确"), false);
    }
}


void RegisterDialog::showTip(QString str, bool b_ok)
{
    if (b_ok) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}


void RegisterDialog::slotRegisterModFinish(ReqId id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return;
    }

    // 解析json字符串, res转为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if (jsonDoc.isNull()) {
        showTip(tr("json解析失败"), false);
        return;
    }
    // json 转换成jsonObect失败
    if (!jsonDoc.isObject()) {
        showTip(tr("json解析失败"), false);
        return;
    }


    handlers_[id](jsonDoc.object());
    return;
}

void RegisterDialog::initHttpHandlers()
{
    handlers_.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug() << "email: " << email;
    });
}
