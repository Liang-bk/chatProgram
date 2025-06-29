cmake_minimum_required(VERSION 3.30)
project(server)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/src)
# include(jsoncpp)


add_executable(server
        # ${JSONCPP_SRC}
        main.cpp
        header/constant.h
        header/cserver.h
        header/httpconnection.h
        header/singleton.h
        header/logic_system.h
        header/message.pb.h
        header/message.grpc.pb.h
        source/cserver.cpp
        source/httpconnection.cpp
        source/logic_system.cpp
        source/message.pb.cc
        source/message.grpc.pb.cc
        source/verify_grpc_client.cpp
        header/verify_grpc_client.h
        source/config.cpp
        header/config.h
        source/asio_iocontext_pool.cpp
        header/asio_iocontext_pool.h
        source/redis_manager.cpp
        header/redis_manager.h
)

# target_link_libraries(server ${Boost_LIBRARIES})
target_compile_definitions(server PRIVATE JSON_DLL) # 正确链接动态库
target_include_directories(server PRIVATE
        ${redis_plus_plus_SOURCE_DIR}/src
)
target_link_directories(server PRIVATE
        ${BOOST_LIB_DIR}
        ${GRPC_LIB_DIR}
        # ${REDIS_LIB_DIR}
)
target_link_libraries(server PRIVATE
        ${GRPC_LIB_NAME}
        # ${REDIS_LIB_NAME}
        jsoncpp_lib

        redis++::redis++
        hiredis::hiredis

)