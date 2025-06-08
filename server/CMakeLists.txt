cmake_minimum_required(VERSION 3.30)
project(server)

set(CMAKE_CXX_STANDARD 20)

set(BOOST_ROOT "D:/MinGW/boost1_84_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost 1.84.0 REQUIRED COMPONENTS system filesystem thread)

if(Boost_FOUND)
    # 手动修正包含目录路径
    set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
    set(BOOST_LIB_DIR "${BOOST_ROOT}/lib")
    message(STATUS "=== Boost Information ===")
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Original Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
    message(STATUS "Boost library dirs: ${Boost_LIBRARY_DIRS}")

    # 验证路径是否存在
    if(EXISTS "${Boost_INCLUDE_DIRS}")
        message(STATUS "✓ Boost include directory exists")
    else()
        message(WARNING "✗ Boost include directory does not exist: ${Boost_INCLUDE_DIRS}")
    endif()

    if(EXISTS "${Boost_INCLUDE_DIRS}/boost")
        message(STATUS "✓ Boost headers found")
    else()
        message(WARNING "✗ Boost headers not found in: ${Boost_INCLUDE_DIRS}/boost")
    endif()
    # 包含目录
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_subdirectory(src)
# add_executable(server src/main.cpp)
