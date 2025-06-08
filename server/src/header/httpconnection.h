//
// Created by baikaishui on 2025/6/5.
//

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "header/constant.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    friend class LogicSystem;
    HttpConnection(tcp::socket socket);
    void start();
private:
    void checkDeadLine();
    void writeResponse();
    void handleRequest();

    void parseGetParams();
    tcp::socket socket_;
    beast::flat_buffer buffer_{ 8192 };
    http::request<http::dynamic_body> request_;
    http::response<http::dynamic_body> response_;

    std::string get_url_;   // 带/不带参数的get方法的请求url
    std::unordered_map<std::string, std::string> get_params_;// get_url参数列表

    net::steady_timer deadline_{
        socket_.get_executor(),		// 将定时器绑定到socket_的调度器上(这里就是iocontext)
        std::chrono::seconds(60)
    };
};





#endif //HTTPCONNECTION_H
