//
// Created by baikaishui on 2025/7/1.
//

#include "mysql_dao.h"
#include "utils/defer.h"
#include "common/constant.h"
/**
 * 初始化sql连接池
 * @param url 数据库地址
 * @param user 数据库用户
 * @param pass 用户对应的密码
 * @param schema 使用的数据库
 * @param pool_size 池大小
 */
SQLConnPool::SQLConnPool(const std::string &url, const std::string &user, const std::string &pass,
    const std::string &schema, size_t pool_size): url_(url), user_(user), pass_(pass),
    schema_(schema), pool_size_(pool_size), stop_(false) {
    try {
        for (size_t i = 0; i < pool_size; i++) {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
            auto* conn = driver->connect(url_, user_, pass_);
            conn->setSchema(schema_);
            // 初始化连接
            auto current_time = std::chrono::system_clock::now().time_since_epoch();
            long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(current_time).count();
            sql_pool_.push(std::make_unique<SQLConn>(conn, time_stamp));
        }
        // 启动监管线程, 防止连接长时间不操作自动退出
        check_thread_ = std::thread([this]() {
            while (!stop_) {
                checkConnection();
                std::this_thread::sleep_for(std::chrono::seconds(120));
            }
        });
        check_thread_.detach();
    } catch (sql::SQLException& e) {
        stop_ = true;
        std::cout << "Initialized MySQL pool error: " << e.what() << std::endl;
    }
}

SQLConnPool::~SQLConnPool() {
    close();
}

std::unique_ptr<SQLConn> SQLConnPool::getSQLConn() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond.wait(lock, [this]() {
        return stop_ || !sql_pool_.empty();
    });
    if (stop_) {
        return nullptr;
    }
    auto conn = std::move(sql_pool_.front());
    sql_pool_.pop();
    return conn;
}

void SQLConnPool::returnSQLConn(std::unique_ptr<SQLConn> conn) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (stop_) {
        return;
    }
    sql_pool_.push(std::move(conn));
    cond.notify_one();
}

/**
 * 固定时间做查询, 并重新连接数据库内因超时而掉线的连接
 */
void SQLConnPool::checkConnection() {
    size_t target_size = 0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        target_size = sql_pool_.size();
    }
    size_t process_cnt = 0;
    while (process_cnt < target_size) {
        std::unique_ptr<SQLConn> conn;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (sql_pool_.empty()) {
                break;
            }
            conn = std::move(sql_pool_.front());
            sql_pool_.pop();
        }
        // check
        bool healthy = true;
        auto now = std::chrono::system_clock::now().time_since_epoch();
        long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(now).count();
        // 一个连接10s未做操作
        if (timestamp - conn->last_op_time_ > 10) {
            try {
                std::unique_ptr<sql::Statement> stmt(conn->conn->createStatement());
                stmt->execute("SELECT 1");
                conn->last_op_time_ = timestamp;
            } catch (sql::SQLException& e) {
                std::cout << "MySQL connection keep-alive failed: " << e.what() << std::endl;
                healthy = false;
                ++failed_cnt;
            }
        }
        // 该连接未掉线, 放回连接池
        if (healthy) {
            std::lock_guard<std::mutex> lock(mutex_);
            sql_pool_.push(std::move(conn));
            cond.notify_one();
        }
        process_cnt++;
    }

    // 已掉线的连接需要重新连接回来
    while (failed_cnt > 0) {
        bool res = reconnect();
        if (res) {
            --failed_cnt;
        } else {
            break;
        }
    }
}
bool SQLConnPool::reconnect() {
    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
        auto* conn = driver->connect(url_, user_, pass_);
        conn->setSchema(schema_);
        auto current_time = std::chrono::system_clock::now().time_since_epoch();
        long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(current_time).count();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            sql_pool_.push(std::make_unique<SQLConn>(conn, time_stamp));
        }
        return true;
    } catch (sql::SQLException& e) {
        std::cout << "MysqlConnPool: Try to reconnect Mysql-server, but failed: "<< e.what() << std::endl;
        return false;
    }
}

void SQLConnPool::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        while (!sql_pool_.empty()) {
            auto conn = std::move(sql_pool_.front());
            sql_pool_.pop();
        }
    }
    cond.notify_all();
}

MySQLDAO::MySQLDAO(const std::string &url, const std::string &user, const std::string &pass,
    const std::string &schema, size_t pool_size) {
    conn_pool_ = std::make_unique<SQLConnPool>(url, user, pass, schema, pool_size);
}


MySQLDAO::~MySQLDAO() {
    conn_pool_->close();
}

ErrorCodes MySQLDAO::userExist(const std::string &name) {
    auto conn = conn_pool_->getSQLConn();
    Defer defer([this, &conn]() {
        conn_pool_->returnSQLConn(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return SQL_ERROR;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->conn->prepareStatement("SELECT COUNT(*) AS cnt FROM users WHERE uid = ?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            int count = res->getInt("cnt");
            if (count > 0) {
                return USER_EXIST;
            } else {
                return USER_NOT_EXIST;
            }
        }
        return SQL_ERROR;
    } catch (sql::SQLException& e) {
        std::cout << "MySQL try to judge " << name << " is exist , but failed: " << e.what() << std::endl;
        return SQL_ERROR;
    }

}

ErrorCodes MySQLDAO::checkUserMatchEmail(const std::string &name, const std::string &email) {
    auto conn = conn_pool_->getSQLConn();
    Defer defer([this, &conn]() {
        conn_pool_->returnSQLConn(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return SQL_ERROR;
        }
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->conn->prepareStatement(
                "SELECT email FROM users WHERE uid = ?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // 账户名唯一, 最多只有一个结果
        if (res->next()) {
            if (email != res->getString("email")) {
                return USER_MAIL_NOT_MATCH;
            } else {
                return SUCCESS;
            }
        }
        // 没有这个账户名, 也不匹配
        return USER_MAIL_NOT_MATCH;
    } catch (sql::SQLException& e) {
        std::cout << "MySQL try to check " << name << ' ' << email << " is matched, but failed: " << e.what() << std::endl;
        return SQL_ERROR;
    }
}

ErrorCodes MySQLDAO::resetPassword(const std::string &name, const std::string &password) {
    auto conn = conn_pool_->getSQLConn();
    Defer defer([this, &conn]() {
        conn_pool_->returnSQLConn(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return SQL_ERROR;
        }
        // 更新语句
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->conn->prepareStatement("UPDATE users SET password_hash = ? WHERE uid = ?"));
        // 设置参数
        pstmt->setString(1, password);
        pstmt->setString(2, name);
        int update_count = pstmt->executeUpdate();
        std::cout << "resetPassword: update rows " << update_count << std::endl;
        return SUCCESS;
    } catch (sql::SQLException& e) {
        std::cout << "MySQL try to reset " << name << "'s password , but failed: " << e.what() << std::endl;
        return SQL_ERROR;
    }
}

/**
 * 用户注册功能, 调用数据库过程进行注册, 并返回错误结果
 * @param name 账户名, 唯一标识
 * @param email 邮箱
 * @param password 密码
 * @return ErrorCodes
 */
ErrorCodes MySQLDAO::userRegister(const std::string &name, const std::string &email, const std::string &password) {
    auto conn = conn_pool_->getSQLConn();
    Defer defer([this, &conn]() {
        conn_pool_->returnSQLConn(std::move(conn));
    });
    try {
        if (conn == nullptr) {
            return SQL_ERROR;
        }
        size_t email_loc = email.find('@');
        std::string username = email.substr(0, email_loc);
        // 调用已写好的存储过程
        std::unique_ptr<sql::Statement> stmt_init(conn->conn->createStatement());
        // 提前设置会话变量
        if (stmt_init->execute("SET @p_status = 0, @p_id = 0")) {
            std::unique_ptr<sql::ResultSet> _(stmt_init->getResultSet());
        }

        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->conn->prepareStatement("CALL register_user(?, ?, ?, ?, @p_status, @p_id);")
            );
        // 设置存储过程的输出参数
        stmt->setString(1, name);
        stmt->setString(2, username);
        stmt->setString(3, email);
        stmt->setString(4, password);
        // 执行
        if (stmt->execute()) {
            std::unique_ptr<sql::ResultSet> _(stmt->getResultSet());
        }

        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
        std::unique_ptr<sql::Statement> stmt_result(conn->conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt_result->executeQuery("SELECT @p_status AS status, @p_id AS id;"));

        if (res->next()) {
            int status = res->getInt("status");
            int id = res->getInt("id");
            switch (status) {
                case 0: return SUCCESS;
                case 1: return MAIL_MAX_ERROR;
                case 2: return USER_EXIST;
                case 3: return SQL_ERROR;
                default: return SQL_ERROR;
            }
        }
        return SQL_ERROR;
    } catch (sql::SQLException& e) {
        std::cout << "MySQL try to register a user, but failed: " << e.what() << std::endl;
        return SQL_ERROR;
    }
}
