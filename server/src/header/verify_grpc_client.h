//
// Created by 86183 on 2025/6/12.
//

#ifndef VARIRFY_GRPC_CLIENT_H
#define VARIRFY_GRPC_CLIENT_H
#pragma once

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


class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
    friend class Singleton<VerifyGrpcClient>;
public:
    ~VerifyGrpcClient();
    GetVerifyResponse GetVerifyCode(std::string email);
private:
    VerifyGrpcClient();
    std::unique_ptr<VerifyService::Stub> stub_;
};



#endif //VARIRFY_GRPC_CLIENT_H
