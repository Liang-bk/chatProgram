//
// Created by baikaishui on 2025/6/5.
//
#pragma once
#ifndef CSERVER_H
#define CSERVER_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    // io_context类似于一个EventLoop, 注册 / 监听 / 回调事件
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    void start();
private:
    tcp::acceptor acceptor_;
    net::io_context& ioc_;
    tcp::socket socket_;

};



#endif //CSERVER_H
