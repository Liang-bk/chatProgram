//
// Created by baikaishui on 2025/6/5.
//
#pragma once
#ifndef CONSTANT_H
#define CONSTANT_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <memory>
#include <map>
#include <unordered_map>
#include <iostream>
#include "header/singleton.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// 错误码和错误类型
enum ErrorCodes {
    SUCCESS = 0,
    ERROR_JSON = 1001,  // JSON解析失败
    RPC_FAILED = 1002,  // RPC请求失败
};

#endif //CONSTANT_H
