#include "httpmanager.h"


HttpManager::HttpManager(QObject *parent)
    : QObject{parent}
{
    connect(this, &HttpManager::sigHttpFinish, this, &HttpManager::slotHttpFinish);
}


HttpManager::~HttpManager()
{

}

void HttpManager::postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    // 创建一个HTTP请求, 设置请求头和请求体
    // 将json对象转为一个字节数组
    QByteArray data = QJsonDocument(json).toJson();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    auto self = shared_from_this();
    // 这里是异步发送
    QNetworkReply *reply = manager_.post(request, data);
    // 数据接收完成, 执行处理逻辑 finish信号和lambda的连接
    connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << reply->errorString();
            emit self->sigHttpFinish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        QString res = reply->readAll();

        emit self->sigHttpFinish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpManager::slotHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    // 注册模块功能处理
    if (mod == Modules::REGISTERMOD) {
        emit sigRegisterModFinish(id, res, err);
    }
}
