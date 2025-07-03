//
// Created by 86183 on 2025/7/2.
//

#ifndef MYSQL_MANAGER_H
#define MYSQL_MANAGER_H

#include "common/singleton.h"


class MySQLDAO;

class MySQLManager:public Singleton<MySQLManager> {
    friend class Singleton<MySQLManager>;
public:
    ~MySQLManager();
    void init(const std::string& url, const std::string& user, const std::string& pass,
        const std::string& schema, size_t pool_size);

    std::pair<ErrorCodes, int> registerUser(const std::string& name,
        const std::string &email, const std::string& password);
private:
    MySQLManager();
    std::unique_ptr<MySQLDAO> dao_;
    bool inited_;
};



#endif //MYSQL_MANAGER_H
