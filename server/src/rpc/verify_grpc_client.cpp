//
// Created by baikaishui on 2025/6/12.
//

#include "verify_grpc_client.h"


RPCPool::RPCPool(const std::string &host, int port, std::size_t pool_size) :
    pool_size_(pool_size), remote_host_(host), remote_port_(port), stop_(false) {
    for (std::size_t i = 0; i < pool_size_; i++) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + std::to_string(port),
            grpc::InsecureChannelCredentials());
        queue_.push(VerifyService::NewStub(channel));
    }
}

RPCPool::~RPCPool() {
    close();
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
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        // 析构应该会自动释放queue_, 理论上下面这段并不是必须的
        // while (!queue_.empty()) {
        //     auto conn = std::move(queue_.front());
        //     queue_.pop();
        // }
    }
    cond_.notify_all();
}

VerifyGrpcClient::VerifyGrpcClient(): inited_(false) {
}

VerifyGrpcClient::~VerifyGrpcClient() {
    // close();
}
void VerifyGrpcClient::init(const std::string &host, int port, size_t pool_size) {
    rpc_pool_ = std::make_unique<RPCPool>(host, port, pool_size);
}

void VerifyGrpcClient::close() {
    rpc_pool_->close();
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
