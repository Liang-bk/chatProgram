//
// Created by baikaishui on 2025/6/26.
//

#ifndef REDIS_MANAGER_H
#define REDIS_MANAGER_H

#include <hiredis.h>
#include <queue>

#include "singleton.h"
struct RedisContextDeleter {
    void operator()(redisContext* c) const {
        if (c != nullptr) {
            std::cout << "try to free redis connection" << std::endl;
            redisFree(c);
        }
    }
};

// 多线程下, 在不对connection做线程安全处理的情况下, 使用单个conn会引起事务的混乱
class RedisConnPool {
public:
    using redisContextPtr = std::unique_ptr<redisContext, RedisContextDeleter>;
    RedisConnPool(size_t pool_size, std::string ip, int port, std::string pass);
    ~RedisConnPool();
    redisContextPtr getRedisContext();
    void returnRedisContext(redisContextPtr context);
    void close();
private:
    std::atomic<bool> stop_;
    size_t pool_size_;
    std::string ip_;
    int port_;
    std::queue<redisContextPtr> conns_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class RedisManager : public Singleton<RedisManager> {
    friend class Singleton<RedisManager>;
    friend class RedisConnPool;
private:
    // 避免代码中重复的释放
    struct RedisCmdRAII {
        RedisCmdRAII(redisContext *context, const char* cmd, ...) {
            reply = nullptr;
            va_list args;
            va_start(args, cmd);
            // 使用vCommand处理参数列表
            reply = static_cast<redisReply *>(redisvCommand(context, cmd, args));
            va_end(args);
        }
        ~RedisCmdRAII() {
            if (reply) {
                freeReplyObject(reply);
            }
            reply = nullptr;
        }
        redisReply *reply;
    };
public:
    ~RedisManager();
    // bool connect(const std::string& host, int port);
    bool get(const std::string& key, std::string& value);
    bool set(const std::string& key, const std::string& value);
    bool auth(const std::string& pass);
    bool lpush(const std::string& key, const std::string& value);
    bool lpop(const std::string& key, std::string& value);
    bool rpush(const std::string& key, const std::string& value);
    bool rpop(const std::string& key, std::string& value);
    bool hset(const std::string& key, const std::string & hkey, const std::string& value);
    bool hset(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string hget(const std::string &key, const std::string &hkey);
    bool del(const std::string &key);
    bool existsKey(const std::string &key);
    void close();
private:
    RedisManager();
    // redisContext *context;
    std::unique_ptr<RedisConnPool> conn_pool_;
};



#endif //REDIS_MANAGER_H
