# jsoncpp
include_directories(${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/include)
set(JSONCPP_SRC
        ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/src/lib_json/json_reader.cpp
        ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/src/lib_json/json_value.cpp
        ${CMAKE_SOURCE_DIR}/third_party/jsoncpp-1.9.6/src/lib_json/json_writer.cpp
)


# boost
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

# grpc
set(GRPC_ROOT "D:/MinGW/grpc")
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