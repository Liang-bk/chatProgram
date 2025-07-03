cmake_minimum_required(VERSION 3.30)
project(gate_server)

add_executable(gate_server
        config.h
        config.cpp
        cserver.h
        cserver.cpp
        httpconnection.h
        httpconnection.cpp
        logic_system.h
        logic_system.cpp
        main.cpp
)
target_compile_definitions(gate_server PRIVATE JSON_DLL) # 正确链接jsoncpp_lib
target_link_libraries(gate_server jsoncpp_lib net rpc db_control utils common)