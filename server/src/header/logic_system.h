//
// Created by 86183 on 2025/6/5.
//

#ifndef LOGIC_SYSTEM_H
#define LOGIC_SYSTEM_H

#include "header/constant.h"
// 前向声明: 解决相互引用
class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    // path -> 处理哪个连接
    bool handlePost(std::string path, std::shared_ptr<HttpConnection> conn);
    // path -> 处理哪个连接
    bool handleGet(std::string path, std::shared_ptr<HttpConnection> conn);
    // 注册get请求的处理逻辑 : url -> get请求如何处理
    void registerGet(std::string url, HttpHandler handler);
    // 注册post请求的处理逻辑: url -> post请求如何处理
    void registerPost(std::string url, HttpHandler handler);
private:
    LogicSystem();
    std::map<std::string, HttpHandler> post_handler_;
    std::map<std::string, HttpHandler> get_handler_;
};


#endif //LOGIC_SYSTEM_H
