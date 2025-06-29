//
// Created by 86183 on 2025/6/5.
//

#include "header/logic_system.h"

#include "header/httpconnection.h"
#include "header/message.pb.h"
#include "header/verify_grpc_client.h"
LogicSystem::LogicSystem() {
    registerGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->response_.body()) << "receive get_test request\r\n";
        int i = 0;
        for (auto &item : connection->get_params_) {
            i++;
            std::cout << "key: " << item.first << " value: " << item.second << std::endl;
            beast::ostream(connection->response_.body()) << "receive key " <<
                item.first << ", value " << item.second << "\r\n";
        }
    });
    registerPost("/post_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        // 将body转为string类型
        auto body_string = boost::beast::buffers_to_string(connection->request_.body().data());
        std::cout << "post_verifycode received: " << body_string << std::endl;
        // 回复JSON数据
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value request_json, response_json;
        Json::Reader reader;
        bool parse_success = reader.parse(body_string, request_json);
        // 将body格式化为json放入request_json中, 检查成功和email是否是其成员
        if (!parse_success || !request_json.isMember("email")) {
            std::cout << "Failed to parse request json!" << std::endl;
            response_json["error"] = ErrorCodes::ERROR_JSON;
            std::string response_string = response_json.toStyledString();
            beast::ostream(connection->response_.body()) << response_string;
            return true;
        }
        // asString将json的某一键的值转为字符串
        // toStyleString转换整个json到字符串
        auto email = request_json["email"].asString();
        std::cout << "post request: email is " << email << std::endl;
        // 调用grpc向verifyServer请求验证
        GetVerifyResponse verify_response = VerifyGrpcClient::getInstance()->GetVerifyCode(email);
        response_json["error"] = verify_response.error();
        response_json["email"] = request_json["email"];
        std::string response_string = response_json.toStyledString();
        beast::ostream(connection->response_.body()) << response_string;
        return true;
    });
}

LogicSystem::~LogicSystem() {

}

void LogicSystem::registerGet(std::string url, HttpHandler handler) {
    get_handler_.insert(make_pair(url, handler));
}

void LogicSystem::registerPost(std::string url, HttpHandler handler) {
    post_handler_.insert(std::make_pair(url, handler));
}

bool LogicSystem::handleGet(std::string path, std::shared_ptr<HttpConnection> conn) {
    if (get_handler_.find(path) == get_handler_.end()) {
        return false;
    }
    get_handler_[path](conn);
    return true;
}

bool LogicSystem::handlePost(std::string path, std::shared_ptr<HttpConnection> conn) {
    // 没有对应的路径
    if (post_handler_.find(path) == post_handler_.end()) {
        return false;
    }

    post_handler_[path](conn);
    return true;
}
