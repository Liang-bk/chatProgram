const config_module = require('./config')
const Redis = require("ioredis")

// 创建redis客户端
const RedisCli = new Redis({
    host: config_module.redis_host, // Redis server host
    port: config_module.redis_port, // Redis server port
    password: config_module.redis_passwd,
});

// 监听错误信息, 比如connect error
RedisCli.on("error", function(err) {
    console.log("Redis client connect error!");
    RedisCli.quit();
});

// redis get
async function RedisGet(key) {
    try{
        const result = await RedisCli.get(key);
        if (result === null) {
            console.log("Redis-cli: try to get <", key, "> from Redis, but not found");
            return null;
        }
        console.log("Redis-cli: get <", key, "> success, value is ", result);
        return result;
    } catch(error) {
        console.log("Redis-cli get error: ", error);
        return null;
    }
}
// redis exists
async function RedisExists(key) {
    try{
        const result = await RedisCli.exists(key);
        if (result === 0) {
            console.log("Redis-cli: ", key, "is not exist");
            return null;
        }
        console.log("Redis-cli: <", key, "> exists");
        return result;
    } catch(error) {
        console.log("Redis-cli exists error: ", error);
        return null;
    }
}
// redis set
async function RedisSetWithTime(key, value, expireTime) {
    try{
        await RedisCli.set(key, value);
        await RedisCli.expire(key, expireTime);
        console.log("Redis-cli: set <", key, "> success, timer has ", expireTime, " left");
        return true;
    } catch(error) {
        console.log("Redis-cli set error: ", error);
        return false;
    }
}

function Quit() {
    RedisCli.quit();
}

module.exports = {RedisGet, RedisExists, RedisSetWithTime, Quit}