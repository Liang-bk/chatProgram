cmake_minimum_required(VERSION 3.30)
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()
add_library(net
        asio_iocontext_pool.cpp
)
target_link_directories(net PUBLIC
        ${BOOST_LIB_DIR}
)
target_link_libraries(net common)