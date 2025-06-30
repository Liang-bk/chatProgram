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

    sw::redis::ConnectionOptions conn_options;
    conn_options.host = redis_host;
    conn_options.port = redis_port;
    conn_options.password = redis_pass;

    sw::redis::ConnectionPoolOptions pool_options;
    pool_options.size = 200; // redis连接数
    pool_options.wait_timeout = std::chrono::milliseconds(500); // 请求一个连接的超时时间
    redis_conn = std::make_unique<sw::redis::Redis>(conn_options, pool_options);
    std::cout << "Redis pool initialized with size: " << pool_options.size << std::endl;
}

RedisManager::~RedisManager() {
    std::cout << "RedisManager::~RedisManager" << std::endl;
}

sw::redis::Redis &RedisManager::getRedis() {
    return *redis_conn;
}
