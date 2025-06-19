//
// Created by baikaishui on 2025/6/12.
//

#include "header/verify_grpc_client.h"
VerifyGrpcClient::VerifyGrpcClient() {
    std::shared_ptr<Channel> channel = grpc::CreateChannel("0.0.0.0:50051",
        grpc::InsecureChannelCredentials());
    stub_ = VerifyService::NewStub(channel);
}

VerifyGrpcClient::~VerifyGrpcClient() {

}


GetVerifyResponse VerifyGrpcClient::GetVerifyCode(std::string email) {
    ClientContext context;
    GetVerifyResponse reply;
    GetVerifyRequest request;
    request.set_email(email);

    Status status = stub_->GetVerifyCode(&context, request, &reply);
    if (status.ok()) {
        return reply;
    } else {
        reply.set_error(ErrorCodes::RPC_FAILED);
        return reply;
    }
}
