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
    VCODE_NOT_EXIST = 1004, // 验证码不存在
    VCODE_INVALID = 1005, // 验证码不匹配
    USER_EXIST = 1006, // 用户已存在
    MAIL_MAX_ERROR = 1007, // 邮箱超限
};

#endif //CONSTANT_H
