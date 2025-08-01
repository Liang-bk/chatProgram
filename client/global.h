#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QRegularExpression>
#include "QStyle"
#include <memory>
#include <iostream>
#include <mutex>
#include <QByteArray>
#include <QDir>
#include <QSettings>
/**
 * @brief repolish 重新刷新qss
 */
extern std::function<void(QWidget*)> repolish;

extern QString gate_server_prefix;
/**
 * @brief The ReqId enum
 * 要实现的请求功能
 */
enum ReqId {
    ID_GET_VERIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,         // 用户注册
    ID_RESET_PWD = 1003,
};
/**
 * @brief The ErrorCodes enum
 * 错误码
 */
enum ErrorCodes {
    SUCCESS = 0,        // 成功
    ERR_JSON = 1,       // json解析失败
    ERR_NETWORK = 2     // 网络错误
};

/**
 * @brief The Modules enum
 * 功能隶属于的模块
 */
enum Modules {
    REGISTERMOD = 0,    // 注册模块
    RESETPWDMOD = 1,
};

#endif // GLOBAL_H
