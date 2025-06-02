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

/**
 * @brief repolish 重新刷新qss
 */
extern std::function<void(QWidget*)> repolish;

/**
 * @brief The ReqId enum
 * 要实现的请求功能
 */
enum ReqId {
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,         // 用户注册
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
};

#endif // GLOBAL_H
