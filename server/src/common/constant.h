//
// Created by baikaishui on 2025/6/5.
//
#pragma once
#ifndef CONSTANT_H
#define CONSTANT_H

#include <memory>
#include <map>
#include <unordered_map>
#include <iostream>

#define CODE_PREFIX "code_"
// 错误码和错误类型
enum ErrorCodes {
    SUCCESS = 0,
    JSON_ERROR = 1001,  // JSON解析失败
    RPC_FAILED = 1002,  // RPC请求失败
    SQL_ERROR = 1003,   // sql数据库错误
    VCODE_ERROR = 1004, // 验证码错误
    USER_EXIST = 1006, // 账户已存在
    USER_NOT_EXIST = 1007, // 账户不存在
    MAIL_MAX_ERROR = 1008, // 邮箱超限
    USER_MAIL_NOT_MATCH = 1009, //账户和邮箱不匹配
    PWD_UPDATE_FAILED = 1010, // 密码更新失败

};

#endif //CONSTANT_H
