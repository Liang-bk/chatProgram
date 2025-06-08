//
// Created by baikaishui on 2025/6/5.
//

#include "header/cserver.h"
#include "header/httpconnection.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port)
    :ioc_(ioc),
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
    socket_(ioc) {
    // io_context无拷贝构造和拷贝赋值
}

void CServer::start() {
    auto self = shared_from_this();
    acceptor_.async_accept(socket_, [self](beast::error_code ec) {
            try {
                // 出错就放弃该连接, 继续监听其他连接
                if (ec) {
                    self->start();
                    return;
                }
                // 接受了新连接, 将socket交给具体的管理类去管理
                std::make_shared<HttpConnection>(std::move(self->socket_))->start();
                // start();
                // HttpConnection(std::move(socket_));
                // 继续监听
                self->start();
            } catch (const std::exception&) {

            }
        });
}