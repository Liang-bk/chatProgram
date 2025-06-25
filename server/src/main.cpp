#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "header/cserver.h"
#include <cctype>
#include "header/constant.h"
#include "header/config.h"
#include "hiredis.h"
void testRedis() {
    // redisConnect: 连接数据库
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        std::cout << "Connect to redis server failed: " << c->errstr << std::endl;
        redisFree(c);
        return;
    }
    std::cout << "Connect to redis server success" << std::endl;
    std::string redis_pass = "123456";
    // redisCommand(context, cmd): 在context中执行cmd命令
    redisReply *r = static_cast<redisReply *>(redisCommand(c, "auth %s", redis_pass.c_str()));
    if (r->type == REDIS_REPLY_ERROR) {
        std::cout << "redis auth failed!" << std::endl;
    } else {
        std::cout << "redis auth success" << std::endl;
    }
    // 命令1: 为redis设置key-value
    const char *cmd1 = "set stest1 value1";
    r = static_cast<redisReply *>(redisCommand(c, cmd1));

    if (r == NULL) {
        std::cout << "execute cmd1 failed!" << std::endl;
        redisFree(c);
        return;
    }
    // cmd执行失败, 释放连接
    if (!(r->type == REDIS_REPLY_STATUS && strcmp(r->str, "OK") == 0 || strcmp(r->str, "ok") == 0)) {
        std::cout << "failed to execute cmd[" << cmd1 << "]" << std::endl;
        freeReplyObject(r);
        redisFree(c);
        return;
    }

    freeReplyObject(r);
    std::cout << "execute cmd[" << cmd1 << "] success" << std::endl;
    // 命令2: 获取之前的key的长度
    const char *cmd2 = "strlen stest1";
    r = static_cast<redisReply *>(redisCommand(c, cmd2));

    if (r->type != REDIS_REPLY_INTEGER) {
        std::cout << "failed to execute cmd[" << cmd2 << "]" << std::endl;
        freeReplyObject(r);
        redisFree(c);
        return;
    }

    // 获取字符串长度
    int length = r->integer;
    freeReplyObject(r);
    std::cout << "The length of 'stest1' is " << length << std::endl;
    std::cout << "execute cmd[" << cmd2 << "] success" << std::endl;
    // 命令3: 获取之前key的value
    const char *cmd3 = "get stest1";
    r = static_cast<redisReply *>(redisCommand(c, cmd3));
    if (r->type != REDIS_REPLY_STRING) {
        std::cout << "failed to execute cmd[" << cmd3 << "]" << std::endl;
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    std::cout << "value of stest1 is :" << r->str << std::endl;
    freeReplyObject(r);
    std::cout << "execute cmd[" << cmd3 << "] success" << std::endl;

    redisFree(c);
}
int main() {
    testRedis();
    auto gateserver_config = Config::getInstance()->getValue("GateServer");
    std::string gate_port_str = gateserver_config["Port"].asString();
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try
    {
        unsigned short port = static_cast<unsigned short>(8000);
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_num) {
            if (error) {
                return;
            }
            ioc.stop();

        });
        std::make_shared<CServer>(ioc, port)->start();
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
