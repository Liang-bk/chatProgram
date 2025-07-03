cmake_minimum_required(VERSION 3.30)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()
add_library(rpc
        message.pb.cc
        message.grpc.pb.cc
        verify_grpc_client.cpp
)
target_link_directories(rpc PUBLIC
        ${GRPC_LIB_DIR}
)
target_link_libraries(rpc common ${GRPC_LIB_NAME})