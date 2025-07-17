//
// Created by baikaishui on 2025/6/26.
//

#ifndef REDIS_MANAGER_H
#define REDIS_MANAGER_H

#include <hiredis/hiredis.h>
#include <queue>
#include <redis-plus-plus/src/sw/redis++/redis.h>

#include "common/singleton.h"

class RedisManager : public Singleton<RedisManager> {
    friend class Singleton<RedisManager>;

public:
    ~RedisManager() override;
    void init(const std::string &host, const int port, const std::string &pass,
        const size_t pool_size, const std::chrono::milliseconds& wait_time);
    sw::redis::Redis& getRedis();
private:
    RedisManager();
    std::unique_ptr<sw::redis::Redis> redis_conn;
    bool inited_;
};



#endif //REDIS_MANAGER_H
