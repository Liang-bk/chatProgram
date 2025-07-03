# jsoncpp
message("-- Loading jsoncpp...")
include(FetchContent)
FetchContent_Declare(
        jsoncpp
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/jsoncpp
)
FetchContent_MakeAvailable(jsoncpp)
message("-- jsoncpp include dir: " "${jsoncpp_SOURCE_DIR}")

# boost
message("-- Loading boost...")
set(BOOST_ROOT "D:/MinGW/boost_compile")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost REQUIRED COMPONENTS system filesystem thread)

if(Boost_FOUND)
    # 手动修正包含目录路径
    # set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include")
    set(BOOST_LIB_DIR "${BOOST_ROOT}/lib")
    message(STATUS "=== Boost Information ===")
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Original Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
    message(STATUS "Boost library dirs: ${Boost_LIBRARY_DIR}")

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

# grpc
message("-- Loading grpc...")
set(GRPC_ROOT "D:/MinGW/grpc")
message("-- grpc include dir: " "${GRPC_ROOT}/include")
include_directories(${GRPC_ROOT}/third_party/re2)
include_directories(${GRPC_ROOT}/third_party/address_sorting/include)
include_directories(${GRPC_ROOT}/third_party/abseil-cpp)
include_directories(${GRPC_ROOT}/third_party/protobuf/src)
include_directories(${GRPC_ROOT}/include)

set(GRPC_LIB_DIR
        ${GRPC_ROOT}/vsbuild/third_party/re2/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/types/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/synchronization/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/status/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/random/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/flags/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/debugging/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/container/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/hash/Debug
        ${GRPC_ROOT}/vsbuild/third_party/boringssl-with-bazel/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/numeric/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/time/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/base/Debug
        ${GRPC_ROOT}/vsbuild/third_party/abseil-cpp/absl/strings/Debug
        ${GRPC_ROOT}/vsbuild/third_party/protobuf/Debug
        ${GRPC_ROOT}/vsbuild/third_party/zlib/Debug
        ${GRPC_ROOT}/vsbuild/Debug
        ${GRPC_ROOT}/vsbuild/third_party/cares/cares/lib/Debug
)
set(GRPC_LIB_NAME
        libprotobufd
        gpr
        grpc
        grpc++
        grpc++_reflection
        address_sorting
        ws2_32
        cares
        zlibstaticd
        upb
        ssl
        crypto
        absl_bad_any_cast_impl
        absl_bad_optional_access
        absl_bad_variant_access
        absl_base
        absl_city
        absl_civil_time
        absl_cord
        absl_debugging_internal
        absl_demangle_internal
        absl_examine_stack
        absl_exponential_biased
        absl_failure_signal_handler
        absl_flags
        absl_flags_config
        absl_flags_internal
        absl_flags_marshalling
        absl_flags_parse
        absl_flags_program_name
        absl_flags_usage
        absl_flags_usage_internal
        absl_graphcycles_internal
        absl_hash
        absl_hashtablez_sampler
        absl_int128
        absl_leak_check
        absl_leak_check_disable
        absl_log_severity
        absl_malloc_internal
        absl_periodic_sampler
        absl_random_distributions
        absl_random_internal_distribution_test_util
        absl_random_internal_pool_urbg
        absl_random_internal_randen
        absl_random_internal_randen_hwaes
        absl_random_internal_randen_hwaes_impl
        absl_random_internal_randen_slow
        absl_random_internal_seed_material
        absl_random_seed_gen_exception
        absl_random_seed_sequences
        absl_raw_hash_set
        absl_raw_logging_internal
        absl_scoped_set_env
        absl_spinlock_wait
        absl_stacktrace
        absl_status
        absl_strings
        absl_strings_internal
        absl_str_format_internal
        absl_symbolize
        absl_synchronization
        absl_throw_delegate
        absl_time
        absl_time_zone
        absl_statusor
        re2
)
# The following redis project will be loaded as dynamic libraries
# Download Hiredis, upon which Redis Plus Plus depends and use local dir.
message("-- Loading hiredis...")
FetchContent_Declare(
        hiredis
        SOURCE_DIR     ${PROJECT_SOURCE_DIR}/third_party/hiredis
)
message("-- hiredis include dir: " "${hiredis_SOURCE_DIR}")
FetchContent_MakeAvailable(hiredis)

# Download the Redis binding.
message("-- Loading redis++")
FetchContent_Declare(
        redis_plus_plus
        SOURCE_DIR      ${PROJECT_SOURCE_DIR}/third_party/redis-plus-plus
)
message("-- redis++ include dir: " "${redis++_SOURCE_DIR}")
# Pre-fill the include and library paths so that Redis++ does not search for them.
set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH};${hiredis_SOURCE_DIR}")
set(hiredis_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/third_party")
set(HIREDIS_HEADER "${PROJECT_SOURCE_DIR}/third_party")
set(TEST_HIREDIS_LIB "${CMAKE_BINARY_DIR}/lib/hiredisd.lib")
set(HIREDIS_LIB "${CMAKE_BINARY_DIR}/lib/hiredisd.lib")

FetchContent_MakeAvailable(redis_plus_plus)
include_directories(${redis_plus_plus_SOURCE_DIR}/src)
# gtest
message("-- Loading GoogleTest...")
FetchContent_Declare(
        googletest
        SOURCE_DIR     ${PROJECT_SOURCE_DIR}/third_party/googletest
)
message("-- GoogleTest include dir: " "${gtest_SOURCE_DIR}")
FetchContent_MakeAvailable(googletest)

# mysql-connector
message("-- Loading mysql-connector...")
set(Mysql_connector_ROOT "D:/MinGW/mysql-connector")
set(Mysql_connector_INCLUDE_DIRS "${Mysql_connector_ROOT}/include")
set(Mysql_connector_LIB_DIRS "${Mysql_connector_ROOT}/lib64/debug/vs14")
set(Mysql_connector_LIB_NAME
        mysqlcppconn
        mysqlcppconnx
)
include_directories(${Mysql_connector_INCLUDE_DIRS})

message("-- mysql-connector include dir: " "${Mysql_connector_INCLUDE_DIRS}")
