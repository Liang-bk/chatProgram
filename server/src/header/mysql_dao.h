//
// Created by 86183 on 2025/7/1.
//

#ifndef MYSQL_DAO_H
#define MYSQL_DAO_H

#include <mutex>
#include <queue>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>


enum ErrorCodes : int; // 前向声明

// SQLConn封装sql::Connection, 并记录上一次该连接进行操作的时间
struct SQLConn {
    SQLConn(sql::Connection *conn, int64_t last_time) :conn(conn), last_op_time_(last_time) {}
    std::unique_ptr<sql::Connection> conn;
    int64_t last_op_time_;
};
class SQLConnPool {
public:
    SQLConnPool(const std::string& url, const std::string& user, const std::string& pass,
        const std::string& schema, size_t pool_size);
    ~SQLConnPool();
    std::unique_ptr<SQLConn> getSQLConn();
    void returnSQLConn(std::unique_ptr<SQLConn> conn);
    void close();
private:
    void checkConnection();
    bool reconnect();
    std::string url_;
    std::string user_;
    std::string pass_;
    std::string schema_;    // 对应数据库的表

    std::mutex mutex_;
    std::queue<std::unique_ptr<SQLConn>> sql_pool_;
    size_t pool_size_;
    std::atomic<bool> stop_;
    std::condition_variable cond;
    std::atomic<int> failed_cnt;
    std::thread check_thread_;
};

/**
 * DAO: Data Access Object, 将业务逻辑代码和数据访问分离
 */
class MySQLDAO {
public:
    MySQLDAO(const std::string& url, const std::string& user, const std::string& pass,
        const std::string& schema, size_t pool_size);
    ~MySQLDAO();
    std::pair<ErrorCodes, int> userRegister(const std::string &name, const std::string& email, const std::string &password);
private:
    std::unique_ptr<SQLConnPool> conn_pool_;
};



#endif //MYSQL_DAO_H
