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
    std::string remote_host_;
    std::string remote_port_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<std::unique_ptr<VerifyService::Stub>> queue_;
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
