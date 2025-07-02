#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "header/cserver.h"
#include <cctype>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#include <sw/redis++/redis.h>
#include <sw/redis++/connection_pool.h>
#include "header/constant.h"
#include "header/config.h"

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


    auto gateserver_config = Config::getInstance()->getValue("GateServer");
    std::string gate_port_str = gateserver_config["Port"].asString();
    unsigned short gate_port = atoi(gate_port_str.c_str());
    try
    {
        unsigned short port = static_cast<unsigned short>(8000);
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
