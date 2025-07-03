//
// Created by baikaishui on 2025/6/26.
//

#include "redis_manager.h"

RedisManager::RedisManager(): inited_(false) {
}

RedisManager::~RedisManager() {
    std::cout << "RedisManager::~RedisManager" << std::endl;
}

void RedisManager::init(const std::string &host, const int port, const std::string &pass,
    const size_t pool_size, const std::chrono::milliseconds &wait_time) {
    sw::redis::ConnectionOptions conn_options;
    conn_options.host = host;
    conn_options.port = port;
    conn_options.password = pass;

    sw::redis::ConnectionPoolOptions pool_options;
    pool_options.size = pool_size; // redis连接数
    pool_options.wait_timeout = wait_time; // 请求一个连接的超时时间
    redis_conn = std::make_unique<sw::redis::Redis>(conn_options, pool_options);
    std::cout << "Redis pool initialized with size: " << pool_options.size << std::endl;
}

sw::redis::Redis &RedisManager::getRedis() {
    return *redis_conn;
}
