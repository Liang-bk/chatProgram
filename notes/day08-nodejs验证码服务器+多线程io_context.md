## npm安装

nodejs下载：[Node.js — Run JavaScript Everywhere](https://nodejs.org/en)

新建一个verify_server文件夹，并初始化邮箱验证服务要用到的nodejs库配置文件：

`npm init`

然后安装grpc-js包：

`npm install @grpc/grpc-js`

安装proto-loader用来解析.proto文件：

`npm install @grpc/proto-loader`

安装处理email邮件的库：

`npm install nodemailer`

## 邮箱配置

注册一个163邮箱

![image-20250626104415147](img/163mail.png)

在`设置`里的`POP3/SMTP/IMAP`页面中开启`SMTP`服务，并记录对应的设备授权码，之后要用到

## js代码

### proto.js

该文件用于解析proto文件

```js
const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname, 'message.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
    keepCase: true,
    longs: String,
    enums: String,
    defaults: true,
    oneofs: true
})

const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)
const message_proto = protoDescriptor.message

module.exports = message_proto
```

1. `require`：相当于其他语言的`import`，用于引入对应的库
2. `protoLoader.loadSync`：加载对应的proto文件
   - `keepCase`:如果为true，则保留.proto文件中字段名原始大小写，否则将所有字段名转换为驼峰式
   - `longs`:控制如何表示 Protocol Buffers 中的 long 类型。如果设置为 String，则长整数会被转换为字符串，以避免 JavaScript 中的整数溢出问题
   - `enums`:控制如何表示 Protocol Buffers 中的枚举类型。如果设置为 String，则枚举值会被转换为字符串
   - `defaults`:如果为 true，则为未明确设置的字段提供默认值
   - `oneofs`:如果为 true，则支持 Protocol Buffers 中的 oneof 特性
3. `module.exports`：设置proto.js文件向外暴露的接口，在这里是`message_proto`

### config.json

该文件用于配置verfiy_server中可能用到的参数：

```json
{
    "email": {
        "user": "", //换成自己的邮箱和授权码
        "pass": ""
    },
    "mysql": {
        "host": "",
        "port": 3306,
        "passwd": 123456
    },
    "redis": {
        "host": "",
        "port": 6380,
        "passwd": 123456
    }
}
```

1. `email`:163邮箱的地址和用于通过验证的授权码
2. `mysql/redis`:访问对应数据库的地址和对应的密码

### config.js

该文件用于读取json文件中的配置信息：

```js
const fs = require('fs');
let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
let email_user = config.email.user;
let email_pass = config.email.pass;
let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;
let code_prefix = "code_";
module.exports = { email_pass, email_user, mysql_host, mysql_port, redis_host, redis_port, redis_passwd, code_prefix }
```

### const.js

该文件用于定义一些枚举常量和其他全局变量：

```js
let code_prefix = "code_"

const Errors = {
    SUCCESS: 0,
    REDISERR: 1,
    EXCEPTION: 2,
};
module.exports = { code_prefix, Errors }
```

### email.js

该文件封装了发送邮件的函数`SendMail`，

```js
const nodemailer = require('nodemailer')
const config_module = require('./config')

let transport = nodemailer.createTransport({
    host: 'smtp.163.com', 
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user, // 发送方邮箱地址
        pass: config_module.email_pass // 发送方对应授权码
    }
});

function SendMail(mailOptions) {
    return new Promise(function (resolve, reject) {
        transport.sendMail(mailOptions, function (error, info) {
            if (error) {
                console.log(error);
                reject(error);
            } else {
                console.log('邮件已成功发送: ' + info.response);
                resolve(info.response)
            }
        });
    })
}

module.exports.SendMail = SendMail;
```

`transport`是发送邮件的代理，`transport.SendMail`是一个异步调用没法直接同步使用，所以需要使用`Promise`封装来提供对外的同步接口，再交给外部使用`await`或`then catch`的方式处理

### server.js

该文件用于启动grpc server

```js
const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid')
const emailModule = require('./email')


async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try {
        uniqueId = uuidv4();
        console.log("uniqueId is ", uniqueId)
        let text_str = '您的验证码为' + uniqueId + '请三分钟内完成注册'
        //发送邮件
        let mailOptions = {
            from: 'xquank@163.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success
        });
    } catch (error) {
        console.log("catch error is ", error)
        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')
    })
}

main()
```

修改package.json 的main和script:

```jso
"main": "server.js",
"scripts": {
	"serve": "node server.js"
}
```

启动grpc-js server：

`npm run serve`

## IO_CONTEXT池

`gateserver`中，只在`main`函数中开启一个`io_context`，之后`CServer`的监听已经新的`HttpConnection`连接的读写都注册在这一个`io_context`中，虽然异步调用不会向同步调用一样阻塞在读写中，但单线程在面对高并发时仍显得力不从心

因此需要创建多个`io_context`来跑在多个线程中，使得主`io_context`只负责管理`CServer`的接收连接事件，其他的`io_context`负责处理连接后的读写和逻辑处理

池构建：为了简单使用池，只在全局设置一个`IO_CONTEXTPOOL`，然后让池内的每一个`io_context`都跑在不同的线程中，更复杂的运用可以参考muduo中的`EventLoopThreadPool`，不是作为单例而是作为一个对象来使用，不管是哪一种，基本遵从Reactor模式，主Reactor用于监听CServer的IO连接事件，各从Reactor负责监听各连接的IO读写事件

```c++
//
// Created by baikaishui on 2025/6/23.
//

#ifndef ASIO_IOCONTEXT_POOL_H
#define ASIO_IOCONTEXT_POOL_H
#include "singleton.h"


class AsioIOContextPool : public Singleton<AsioIOContextPool> {
    friend class Singleton<AsioIOContextPool>;
public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::executor_work_guard<IOService::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;
    ~AsioIOContextPool();
    // 禁用拷贝构造
    AsioIOContextPool(const AsioIOContextPool&) = delete;
    // 禁用赋值
    AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
    boost::asio::io_context& getIOContext();
    void stop();
private:
    AsioIOContextPool(std::size_t size = 2);
    std::vector<IOService> io_services_; // size个io_context
    std::vector<WorkPtr> works_;		// 对应的size个work	
    std::vector<std::thread> threads_;	// 对应要开启的size个线程
    std::size_t next_io_service_;		// 轮询分配给下一个连接的io_context
};



#endif //ASIO_IOCONTEXT_POOL_H

//
// Created by baikaishui on 2025/6/23.
//

#include <memory>

#include "header/asio_iocontext_pool.h"

AsioIOContextPool::AsioIOContextPool(std::size_t size) :
    io_services_(size), works_(size), next_io_service_(0) {
    // work的作用是让空的io_context去等待, 否则io_context.run()发现没有事件监听会直接退出
    for (std::size_t i = 0; i < size; ++i) {
        works_[i] = std::make_unique<Work>(io_services_[i].get_executor());
    }
    // 多线程
    for (std::size_t i = 0; i < io_services_.size(); ++i) {
        threads_.emplace_back([this, i]() {
            io_services_[i].run();
        });
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    stop();
    std::cout << "asio_io_context_pool.cpp: AsioIOContextPool destrcution" << std::endl;
}

boost::asio::io_context &AsioIOContextPool::getIOContext() {
    auto &service = io_services_[next_io_service_];
    next_io_service_ = (next_io_service_ + 1) % io_services_.size();
    return service;
}

void AsioIOContextPool::stop() {
    // work.reset()并不能将io_context停止运行
    // 需要手动停止
    for (size_t i = 0; i < io_services_.size(); ++i) {
        io_services_[i].stop();
        works_[i].reset();
    }
    // 等待所有线程结束
    for (auto &thread : threads_) {
        thread.join();
    }
}
```

### CServer逻辑修改

对应的在`CServer`中，首先创建一个对等连接，并将其绑定在非主`io_context`的`io_context`中，然后在接收连接中，使用对等连接的`socket`来接收，`conn`在`start`后，`conn`的`socket`属于的`io_context`就会监听该`socket`的读写请求，从而实现主从Reactor模式

```c++
void CServer::start() {
    auto self = shared_from_this();
    // 接收连接时, 将新连接的读写事件交由另外的io_context处理
    auto &context = AsioIOContextPool::getInstance()->getIOContext();
    // 将连接的socket注册到对应线程的io_context上
    std::shared_ptr<HttpConnection> conn = std::make_shared<HttpConnection>(context);
    acceptor_.async_accept(conn->getSocket(), [self, conn](beast::error_code ec) {
            try {
                // 出错就放弃该连接, 继续监听其他连接
                if (ec) {
                    self->start();
                    return;
                }
                // 在另一个线程的io_context上开启这个连接的读写
                conn->start();
                // 主线程继续监听
                self->start();
            } catch (const std::exception&) {

            }
        });
}
```



## RPC池

每一个HttpConnetion在处理逻辑时实际调用了LogicSystem中的函数，进而进行rpc调用，当一定时间内的HttpConnection突然过多的时候并同时访问了某一个服务，需要创建同等多的rpc连接，但这些rpc请求又比较短，所以会出现短时间内的资源的创建和销毁，这对cpu资源和端口是一个很大的消耗

为了避免这些情况，需要创建连接池，提前创建一定数量的连接，每次从连接池中取出一个连接，使用完毕后再放回，如果连接池为空就让对应的请求暂时等待，来达到一个缓存的效果

### verify_grpc_client

```c++
//
// Created by 86183 on 2025/6/12.
//

#ifndef VARIRFY_GRPC_CLIENT_H
#define VARIRFY_GRPC_CLIENT_H
#pragma once

#include <queue>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "constant.h"
#include "singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyRequest;
using message::GetVerifyResponse;
using message::VerifyService;

class RPCPool {
public:
    RPCPool(std::size_t pool_size, std::string host, std::string port);
    ~RPCPool();
    std::unique_ptr<VerifyService::Stub> getOneConn();
    void returnOneConn(std::unique_ptr<VerifyService::Stub> stub);
    void close();
private:
    std::atomic<bool> stop_;
    std::size_t pool_size_;
    std::string remote_host_;	// rpc服务器的地址
    std::string remote_port_;
    std::mutex mutex_;			// 互斥锁每次只从队列中取出一条
    std::condition_variable cond_;	// 生产者-消费者条件变量
    std::queue<std::unique_ptr<VerifyService::Stub>> queue_;	// 池队列, 存储创建好的rpc连接
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;
public:
    ~VerifyGrpcClient();
    GetVerifyResponse GetVerifyCode(std::string email);
private:
    VerifyGrpcClient();
    std::unique_ptr<RPCPool> rpc_pool_;
};



#endif //VARIRFY_GRPC_CLIENT_H

//
// Created by baikaishui on 2025/6/12.
//

#include "header/verify_grpc_client.h"

#include "header/config.h"

RPCPool::RPCPool(std::size_t pool_size, std::string host, std::string port) :
    pool_size_(pool_size), remote_host_(host), remote_port_(port), stop_(false) {
    for (std::size_t i = 0; i < pool_size_; i++) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
            grpc::InsecureChannelCredentials());
        queue_.push(VerifyService::NewStub(channel));
    }
}

RPCPool::~RPCPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    close();
    // 析构应该会自动释放queue_, 理论上下面这段并不是必须的
    while (!queue_.empty()) {
        queue_.pop();
    }
}

std::unique_ptr<VerifyService::Stub> RPCPool::getOneConn() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]() {
        if (stop_) {
            return true;
        }
        return !queue_.empty();
    });
    if (stop_) {
        return nullptr;
    } else {
        auto stub = std::move(queue_.front());
        queue_.pop();
        return stub;
    }
}

void RPCPool::returnOneConn(std::unique_ptr<VerifyService::Stub> stub) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (stop_) {
        return;
    }
    queue_.push(std::move(stub));
    cond_.notify_one();
}

void RPCPool::close() {
    stop_ = true;
    cond_.notify_all();
}

VerifyGrpcClient::VerifyGrpcClient() {
    auto config = Config::getInstance();
    std::string remote_host = (*config)["VerifyServer"]["Host"].asString();
    std::string remote_port = (*config)["VerifyServer"]["Port"].asString();
    rpc_pool_ = std::make_unique<RPCPool>(200, remote_host, remote_port);
}

VerifyGrpcClient::~VerifyGrpcClient() {

}


GetVerifyResponse VerifyGrpcClient::GetVerifyCode(std::string email) {
    ClientContext context;
    GetVerifyResponse reply;
    GetVerifyRequest request;
    request.set_email(email);
    // 从连接池中获取一个rpc连接
    std::unique_ptr<VerifyService::Stub> stub = rpc_pool_->getOneConn();
    Status status = stub->GetVerifyCode(&context, request, &reply);
    // 通信完毕后归还
    if (status.ok()) {
        rpc_pool_->returnOneConn(std::move(stub));
        return reply;
    } else {
        rpc_pool_->returnOneConn(std::move(stub));
        reply.set_error(ErrorCodes::RPC_FAILED);
        return reply;
    }
}
```

