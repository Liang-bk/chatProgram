//
// Created by baikaishui on 2025/6/5.
//

#include "header/cserver.h"
#include "header/httpconnection.h"
#include "header/asio_iocontext_pool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port)
    :ioc_(ioc),
    acceptor_(ioc, tcp::endpoint(tcp::v4(), port)) {
    // io_context无拷贝构造和拷贝赋值
}

void CServer::start() {
    auto self = shared_from_this();
    // 接收连接时, 将新连接的读写事件交由另外的io_context处理
    auto &context = AsioIOContextPool::getInstance()->getIOContext();
    // 将连接的socket注册到对应线程的io_context上
    std::shared_ptr<HttpConnection> conn = std::make_shared<HttpConnection>(context);
    acceptor_.async_accept(conn->getSocket(), [self, conn](beast::error_code ec) {
            try {
                // 出错就放弃该连接, 继续监听其他连接
                if (ec) {
                    self->start();
                    return;
                }
                // 在另一个线程的io_context上开启这个连接的读写
                conn->start();
                // 主线程继续监听
                self->start();
            } catch (const std::exception&) {

            }
        });
}