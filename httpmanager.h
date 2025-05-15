#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>
#include <QJsonObject>
#include <QJsonDocument>
#include "singleton.h"

/******************************************************************************
 *
 * @file       httpmanager.h
 * @brief
 *
 * @author     白开水
 * @date       2025/05/13
 * @history
 *****************************************************************************/
/* CRTP
 *
 */
class HttpManager : public QObject, public Singleton<HttpManager>,
                public std::enable_shared_from_this<HttpManager>
{
    Q_OBJECT
public:
    ~HttpManager();
    void postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
private:
    // 使基类能够new出子类的对象
    friend class Singleton<HttpManager>;
    explicit HttpManager(QObject *parent = nullptr);
    QNetworkAccessManager manager_;

private slots:
    void slotHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod);
signals:
    void sigHttpFinish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sigRegisterModFinish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMANAGER_H
