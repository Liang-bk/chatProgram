cmake_minimum_required(VERSION 3.30)
project(server)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/src) # 包含src目录

add_subdirectory(common)
add_subdirectory(database)
add_subdirectory(gateserver)
add_subdirectory(net)
add_subdirectory(rpc)
add_subdirectory(utils)
