//
// Created by baikaishui on 2025/6/5.
//

#include "header/httpconnection.h"
#include "header/logic_system.h"
// 10转16
unsigned char DecToHex(unsigned char x) {
    return x > 9 ? x + 55 : x + 48;
}
// 16转10
unsigned char HexToDec(unsigned char x) {
    unsigned char y;
    if (x >= '0' && x <= '9') y = x - '0';
    else if (x >= 'A' && x <= 'F') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'f') y = x - 'a' + 10;
    else assert(0);
    return y;
}
// http url编码, 转换空格和其他非ascii字符
std::string urlEncode(const std::string& str) {
    std::string encoded = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        // 数字和字母不变
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~')) {
            encoded += str[i];
        } else if (str[i] == ' ') {
            // space变为'+'
            encoded += '+';
        } else {
            // 其他字符转换为%(高4位16进制)(低4位16进制)
            encoded += '%';
            encoded += DecToHex((unsigned char)str[i] >> 4);
            encoded += DecToHex((unsigned char)str[i] & 0x0F);
        }
    }
    return encoded;
}
// http url解码
std::string urlDecode(const std::string& str) {
    std::string decoded = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') {
            decoded += ' ';
        } else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = HexToDec((unsigned char)str[++i]);
            unsigned char low = HexToDec((unsigned char)str[++i]);
            // unsigned char c = HexToDec(str[i + 1]);
            // c = (c << 4) + HexToDec(str[i + 2]);
            decoded += (char)(high * 16 + low);
            // i += 2;
        } else {
            decoded += str[i];
        }
    }
    return decoded;
}
HttpConnection::HttpConnection(boost::asio::io_context& io_context)
    : socket_(io_context) {

}

void HttpConnection::start() {
    auto self = shared_from_this();
    // 注册读请求
    http::async_read(socket_, buffer_, request_, [self](beast::error_code ec, std::size_t bytes_transferred) {
        try {
            if (ec) {
                std::cout << "http read err is " << ec.what() << std::endl;
                return;
            }
            boost::ignore_unused(bytes_transferred);
            // 处理http请求
            self->handleRequest();
            // 发送响应是否超时, 超时则关闭socket_
            self->checkDeadLine();
        }
        catch (const std::exception& exp) {
            std::cout << "exception is " << exp.what() << std::endl;
        }
    });
}

tcp::socket& HttpConnection::getSocket() {
    return socket_;
}

void HttpConnection::checkDeadLine() {
    auto self = shared_from_this();
    deadline_.async_wait([self](beast::error_code ec) {
        if (!ec) {
            self->socket_.close();
        }
    });
}
void HttpConnection::writeResponse() {
    auto self = shared_from_this();
    response_.content_length(response_.body().size());
    response_.prepare_payload();
    http::async_write(socket_, response_, [self](beast::error_code ec, std::size_t bytes_transferred) {
        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        self->deadline_.cancel();
    });
}
void HttpConnection::handleRequest() {
    // 设置版本
    response_.version(request_.version());
    // 设置短连接
    response_.keep_alive(false);
    // 处理不同的请求方法
    if (request_.method() == http::verb::get) {
        // 过滤uri得到url和kv
        parseGetParams();
        bool success = LogicSystem::getInstance()->handleGet(get_url_, shared_from_this());
        if (!success) {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain; charset=utf-8");
            beast::ostream(response_.body()) << "url not found\r\n";
            writeResponse();
            return;
        }
        response_.set(http::field::content_type, "text/plain; charset=utf-8");
        response_.result(http::status::ok);
        response_.set(http::field::server, "GateServer");
        writeResponse();
        return;
    } else if (request_.method() == http::verb::post) {
        bool success = LogicSystem::getInstance()->handlePost(request_.target(), shared_from_this());
        if (!success) {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "url not found\r\n";
            writeResponse();
            return;
        }
        response_.result(http::status::ok);
        response_.set(http::field::server, "GateServer");
        writeResponse();
        return;
    }
}

void HttpConnection::parseGetParams() {
    // xxx?key1=value1&key2=value2
    auto uri = request_.target();
    auto q_pos = uri.find('?');
    // 找第一个?的位置, 没找到说明没有
    if (q_pos == std::string::npos) {
        get_url_ = urlDecode(uri);
        return;
    }
    // 否则url是uri[0 ~ q_pos-1], 键值对在uri[q_pos + 1 ~ end]
    get_url_ = urlDecode(uri.substr(0, q_pos));
    std::string query = uri.substr(q_pos + 1);
    // 遍历键值对, key和value都要decode
    size_t and_pos = 0;
    while ((and_pos = query.find('&')) != std::string::npos) {
        auto kv = query.substr(0, and_pos);
        size_t equal_pos = kv.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = urlDecode(kv.substr(0, equal_pos));
            std::string value = urlDecode(kv.substr(equal_pos + 1));
            get_params_[key] = value;
        }
        query = query.substr(and_pos + 1);
    }

    if (!query.empty()) {
        size_t equal_pos = query.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = urlDecode(query.substr(0, equal_pos));
            std::string value = urlDecode(query.substr(equal_pos + 1));
            get_params_[key] = value;
        }
    }
}
