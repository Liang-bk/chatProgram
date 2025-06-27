//
// Created by baikaishui on 2025/6/26.
//

#include "header/redis_manager.h"

#include "header/config.h"

RedisManager::RedisManager() {
    auto config = Config::getInstance();
    std::string redis_host = (*config)["Redis"]["Host"].asString();
    int redis_port = (*config)["Redis"]["Port"].asInt();
    std::string redis_pass = (*config)["Redis"]["Pass"].asString();
    conn_pool_ = std::make_unique<RedisConnPool>(10, redis_host, redis_port, redis_pass);
}
RedisManager::~RedisManager() {
    std::cout << "RedisManager::~RedisManager" << std::endl;
    close();
}
// 连接redis数据库
// bool RedisManager::connect(const std::string &host, int port) {
//     context = redisConnect(host.c_str(), port);
//     if (context == nullptr || context->err) {
//         std::cout << "redis connect error " << context->errstr << std::endl;
//         context = nullptr;
//         return false;
//     }
//     return true;
// }
// 设置一个键值对key - value
bool RedisManager::set(const std::string& key, const std::string& value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "SET %s %s", key.c_str(), value.c_str());
    if (cmd.reply == nullptr ||
        !(cmd.reply->type == REDIS_REPLY_STATUS &&
            (strcmp(cmd.reply->str, "OK") == 0) || strcmp(cmd.reply->str, "ok") == 0)) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [SET " << key << " = "<< value << "] failed" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [SET " << key << " = "<< value << "] success" << std::endl;
    return true;
}
// 取出key对应的value
bool RedisManager::get(const std::string &key, std::string &value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "GET %s", key.c_str());
    if (cmd.reply == nullptr || cmd.reply->type != REDIS_REPLY_STRING) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [GET " << key << "] failed" << std::endl;
        return false;
    }
    value = cmd.reply->str;
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [GET " << key << "] value is: " << value << std::endl;
    return true;
}
// 认证
bool RedisManager::auth(const std::string &pass) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "AUTH %s", pass.c_str());
    if (cmd.reply->type == REDIS_REPLY_ERROR) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis AUTH failed" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis AUTH success" << std::endl;
    return true;
}
// 为键为key的链表头部插入value(如果没有就创建一个空链表并插入)
bool RedisManager::lpush(const std::string &key, const std::string &value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "LPUSH %s %s", key.c_str(), value.c_str());
    if (cmd.reply == nullptr ||
        cmd.reply->type != REDIS_REPLY_INTEGER || cmd.reply->integer <= 0) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [LPUSH " << key << ' ' << value << "] failed" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [LPUSH " << key << ' ' << value << "] success" << std::endl;
    return true;
}
// 键为key的链表pop_front, 将pop出的数据存到value
bool RedisManager::lpop(const std::string &key, std::string &value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "LPOP %s", key.c_str());
    if (cmd.reply == nullptr || cmd.reply->type == REDIS_REPLY_NIL) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [LPOP " << key << "] failed" << std::endl;
        return false;
    }
    value = cmd.reply->str;
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [LPOP " << key << ' ' << value << "] success" << std::endl;
    return true;
}
// 为以键为key的链表的尾部插入value
bool RedisManager::rpush(const std::string &key, const std::string &value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "RPUSH %s %s", key.c_str(), value.c_str());
    if (cmd.reply == nullptr ||
        cmd.reply->type != REDIS_REPLY_INTEGER ||cmd.reply->integer <= 0) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [RPUSH " << key << ' ' << value << "] failed" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [RPUSH " << key << ' ' << value << "] success" << std::endl;
    return true;
}
// 键为key的链表的尾部pop_back, 将pop出的数据存到value
bool RedisManager::rpop(const std::string &key, std::string &value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "RPOP %s", key.c_str());
    if (cmd.reply == nullptr || cmd.reply->type == REDIS_REPLY_NIL) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [RPOP " << key << "] failed" << std::endl;
        return false;
    }
    value = cmd.reply->str;
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [RPOP " << key << ' ' << value << "] success" << std::endl;
    return true;
}
// hset: 设置键为key的哈希表, hash表中的数据形如hkey-hvalue
bool RedisManager::hset(const char *key, const char *hkey, const char *hvalue, size_t hvaluelen) {
    auto ctx = conn_pool_->getRedisContext();
    const char *argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;

    redisReply *reply = static_cast<redisReply *>(redisCommandArgv(ctx.get(), 4, argv, argvlen));
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [HSet " << key << "  " << hkey << "  " << hvalue << "] failed" << std::endl;
        freeReplyObject(reply);
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [HSet " << key << "  " << hkey << "  " << hvalue << "] success ! " << std::endl;
    freeReplyObject(reply);
    return true;
}
// hset: 设置键为key的哈希表, hash表中的数据形如hkey-value
bool RedisManager::hset(const std::string &key, const std::string &hkey, const std::string &value) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (cmd.reply == nullptr || cmd.reply->type != REDIS_REPLY_INTEGER) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [HSET " << key << "  " << hkey << "  " << value << "] failed" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [HSET " << key << "  " << hkey << "  " << value << "] success" << std::endl;
    return true;
}

std::string RedisManager::hget(const std::string &key, const std::string &hkey) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "HGET %s %s", key.c_str(), hkey.c_str());
    if (cmd.reply == nullptr || cmd.reply->type == REDIS_REPLY_NIL) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [HGET " << key << "  " << hkey << "] failed" << std::endl;
        return "";
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [HGET " << key << "  " << hkey << "] success" << std::endl;
    std::string value = cmd.reply->str;
    return value;
}

bool RedisManager::del(const std::string &key) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "DEL %s", key.c_str());
    if (cmd.reply == nullptr || cmd.reply->type != REDIS_REPLY_INTEGER) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [DEL " << key << "] failed" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [DEL " << key << "] success" << std::endl;
    return true;
}

bool RedisManager::existsKey(const std::string &key) {
    auto ctx = conn_pool_->getRedisContext();
    RedisCmdRAII cmd(ctx.get(), "EXISTS %s", key.c_str());
    if (cmd.reply == nullptr || cmd.reply->type != REDIS_REPLY_INTEGER ||
        cmd.reply->integer == 0) {
        conn_pool_->returnRedisContext(std::move(ctx));
        std::cout << "redis [EXISTS " << key << "], not found" << std::endl;
        return false;
    }
    conn_pool_->returnRedisContext(std::move(ctx));
    std::cout << "redis [EXISTS " << key << "], found" << std::endl;
    return true;
}

void RedisManager::close() {
    // 如果RedisManager有需要主动清理的资源
    conn_pool_->close();
}

RedisConnPool::RedisConnPool(size_t pool_size, std::string ip, int port, std::string pass) :
    pool_size_(pool_size), ip_(ip), port_(port) {
    for (size_t i = 0; i < pool_size_; i++) {

        redisContextPtr context(redisConnect(ip.c_str(), port));
        if (context == nullptr || context->err != 0) {
            continue;
        }
        RedisManager::RedisCmdRAII cmd(context.get(), "auth %s", pass.c_str());
        if (cmd.reply == nullptr || cmd.reply->type == REDIS_REPLY_ERROR) {
            std::cout << "redis pool try [AUTH " << pass << "] failed" << std::endl;
            continue;
        }
        // std::cout << "redis pool try [AUTH " << pass << "] success" << std::endl;
        conns_.push(std::move(context));
    }
}

RedisConnPool::~RedisConnPool() {
    close();
    std::cout << "RedisConnPool::~RedisConnPool" << std::endl;
}

RedisConnPool::redisContextPtr RedisConnPool::getRedisContext() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]() {
        if (stop_) {
            return true;
        }
        return !conns_.empty();
    });
    if (stop_) {
        return nullptr;
    }
    redisContextPtr context = std::move(conns_.front());
    conns_.pop();
    return context;
}

void RedisConnPool::returnRedisContext(redisContextPtr context) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (stop_) {
        return;
    }
    conns_.push(std::move(context));
    cond_.notify_one();
}

void RedisConnPool::close() {
    stop_ = true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        std::cout << conns_.size() << std::endl;
        while (!conns_.empty()) {
            auto conn = std::move(conns_.front());
            conns_.pop();
        }
    }
    cond_.notify_all();
}
