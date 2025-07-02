//
// Created by baikaishui on 2025/7/2.
//

#include "header/mysql_manager.h"

#include "header/config.h"
#include "header/mysql_dao.h"

MySQLManager::MySQLManager() {
    auto config = Config::getInstance();
    std::string host = (*config)["MySQL"]["Host"].asString();
    int port = (*config)["MySQL"]["Port"].asInt();
    std::string user = (*config)["MySQL"]["User"].asString();
    std::string password = (*config)["MySQL"]["Pass"].asString();
    std::string schema = (*config)["MySQL"]["Schema"].asString();
    std::string url = "tcp://" + host + ":" + std::to_string(port);
    dao_ = std::make_unique<MySQLDAO>(url, user, password, schema, 12);
}

MySQLManager::~MySQLManager() {

}

std::pair<ErrorCodes, int> MySQLManager::registerUser(const std::string &name, const std::string &email, const std::string &password) {
    return dao_->userRegister(name, email, password);
}
