#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "header/cserver.h"
#include <cctype>
#include "header/constant.h"
#include "header/config.h"
int main() {
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
