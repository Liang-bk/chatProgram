//
// Created by baikaishui on 2025/6/26.
//

#ifndef REDIS_MANAGER_H
#define REDIS_MANAGER_H

#include <hiredis.h>
#include <queue>
#include <sw/redis++/redis.h>

#include "singleton.h"

class RedisManager : public Singleton<RedisManager> {
    friend class Singleton<RedisManager>;

public:
    ~RedisManager();
    sw::redis::Redis& getRedis();
private:
    RedisManager();
    std::unique_ptr<sw::redis::Redis> redis_conn;
};



#endif //REDIS_MANAGER_H
