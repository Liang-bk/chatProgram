cmake_minimum_required(VERSION 3.30)
project(server)

set(CMAKE_CXX_STANDARD 20)
# 将所有二进制文件生成在类似build/bin目录下(解决windows平台下三方库dll找不到的问题)
message("-- Find CMAKE_BINARY_DIR: " "${CMAKE_BINARY_DIR}")
message("-- Set binary output: " "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
message("-- CMAKE_RUNTIME_OUTPUT_DIRECTORY: " "${CMAKE_BINARY_DIR}/bin")
message("-- CMAKE_LIBRARY_OUTPUT_DIRECTORY: " "${CMAKE_BINARY_DIR}/bin")
message("-- CMAKE_ARCHIVE_OUTPUT_DIRECTORY: " "${CMAKE_BINARY_DIR}/bin")

include(cmake/third_party.cmake)


add_subdirectory(src)
add_subdirectory(example)
