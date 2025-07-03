#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "gateserver/cserver.h"
#include <cctype>
#include <database/mysql_manager.h>
#include <database/redis_manager.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#include <rpc/verify_grpc_client.h>
#include <sw/redis++/redis.h>
#include <sw/redis++/connection_pool.h>
#include "gateserver/config.h"

void loadConfig() {
    auto config = Config::getInstance();
    std::string gate_server_h = (*config)["GateServer"]["Host"].asString();
    int gate_server_p = (*config)["GateServer"]["Port"].asInt();

    std::string verify_server_h = (*config)["VerifyServer"]["Host"].asString();
    int verify_server_p = (*config)["VerifyServer"]["Port"].asInt();

    std::string redis_host = (*config)["Redis"]["Host"].asString();
    int redis_port = (*config)["Redis"]["Port"].asInt();
    std::string redis_pass = (*config)["Redis"]["Pass"].asString();

    std::string mysql_host = (*config)["MySQL"]["Host"].asString();
    int mysql_port = (*config)["MySQL"]["Port"].asInt();
    std::string mysql_pass = (*config)["MySQL"]["Pass"].asString();
    std::string mysql_user = (*config)["MySQL"]["User"].asString();
    std::string mysql_schema = (*config)["MySQL"]["Schema"].asString();
    std::string mysql_url = "tcp://" + mysql_host + ":" + std::to_string(mysql_port);

    VerifyGrpcClient::getInstance()->init(verify_server_h, verify_server_p, 10);
    RedisManager::getInstance()->init(redis_host, redis_port, redis_pass, 20, std::chrono::milliseconds(500));
    MySQLManager::getInstance()->init(mysql_url, mysql_user, mysql_pass, mysql_schema, 12);
    // iocontextpool
}
int main() {
    // try {
    //     sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
    //
    //     // ✅ 修正连接地址格式
    //     std::unique_ptr<sql::Connection> conn(
    //         driver->connect("tcp://127.0.0.1:3306", "root", "123456")
    //     );
    //
    //     conn->setSchema("chat_project");
    //
    //     std::unique_ptr<sql::Statement> stmt(conn->createStatement());
    //     stmt->execute("SELECT 1;");
    //     stmt->execute("SELECT 1;");
    // } catch (sql::SQLException& e) {
    //     std::cerr << "SQL error: " << e.what() << std::endl;
    // }

    loadConfig();
    auto gateserver_config = Config::getInstance()->getValue("GateServer");
    std::string gate_port_str = gateserver_config["Port"].asString();
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try
    {
        unsigned short port = static_cast<unsigned short>(gate_port);
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_num) {
            if (error) {
                return;
            }
            ioc.stop();

        });
        std::make_shared<CServer>(ioc, port)->start();
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
