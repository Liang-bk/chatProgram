cmake_minimum_required(VERSION 3.30)
project(server)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/src)
include(${CMAKE_SOURCE_DIR}/cmake/third_party.cmake)



add_executable(server
        ${JSONCPP_SRC}
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
)

# target_link_libraries(server ${Boost_LIBRARIES})

target_link_directories(server PRIVATE
        ${BOOST_LIB_DIR}
        ${GRPC_LIB_DIR}
)
target_link_libraries(server PRIVATE
        ${GRPC_LIB_NAME}
)