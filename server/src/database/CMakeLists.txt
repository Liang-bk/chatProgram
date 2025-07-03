cmake_minimum_required(VERSION 3.20)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()
add_library(db_control
        mysql_dao.cpp
        mysql_manager.cpp
        redis_manager.cpp
)

target_link_directories(db_control PUBLIC
        ${Mysql_connector_LIB_DIRS}
)
target_link_libraries(db_control
        ${Mysql_connector_LIB_NAME}
        common utils
        hiredis::hiredis
        redis++::redis++
)