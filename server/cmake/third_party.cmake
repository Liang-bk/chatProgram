# jsoncpp
include_directories(${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/include)
set(JSONCPP_SRC
        ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/src/lib_json/json_reader.cpp
        ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/src/lib_json/json_value.cpp
        ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/src/lib_json/json_writer.cpp
)