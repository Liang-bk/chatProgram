//
// Created by baikaishui on 2025/7/2.
//

#include "mysql_manager.h"
#include "mysql_dao.h"

MySQLManager::MySQLManager(): inited_(false) {

}

MySQLManager::~MySQLManager() {

}

void MySQLManager::init(const std::string &url, const std::string &user,
    const std::string &pass, const std::string &schema, size_t pool_size) {
    dao_ = std::make_unique<MySQLDAO>(url, user, pass, schema, pool_size);
    inited_ = true;
}

ErrorCodes MySQLManager::registerUser(const std::string &name, const std::string &email, const std::string &password) {
    return dao_->userRegister(name, email, password);
}
