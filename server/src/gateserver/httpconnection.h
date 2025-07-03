//
// Created by baikaishui on 2025/6/5.
//

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include "common/constant.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;
public:
    HttpConnection(boost::asio::io_context& io_context);
    void start();
    boost::asio::ip::tcp::socket& getSocket();
private:
    void checkDeadLine();
    void writeResponse();
    void handleRequest();

    void parseGetParams();
    boost::asio::ip::tcp::socket socket_;
    beast::flat_buffer buffer_{ 8192 };
    http::request<http::dynamic_body> request_;
    http::response<http::dynamic_body> response_;

    std::string get_url_;   // 带/不带参数的get方法的请求url
    std::unordered_map<std::string, std::string> get_params_;// get_url参数列表

    boost::asio::steady_timer deadline_{
        socket_.get_executor(),		// 将定时器绑定到socket_的调度器上(这里就是iocontext)
        std::chrono::seconds(60)
    };
};





#endif //HTTPCONNECTION_H
