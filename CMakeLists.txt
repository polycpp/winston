cmake_minimum_required(VERSION 3.20)
project(polycpp_winston VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Fetch polycpp (disable its tests when building as a dependency)
set(POLYCPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(POLYCPP_SOURCE_DIR "" CACHE PATH "Path to a local polycpp checkout")

if(POLYCPP_SOURCE_DIR)
    FetchContent_Declare(
        polycpp
        SOURCE_DIR ${POLYCPP_SOURCE_DIR}
    )
else()
    FetchContent_Declare(
        polycpp
        GIT_REPOSITORY https://github.com/enricohuang/polycpp.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
endif()
FetchContent_MakeAvailable(polycpp)

# Library target
add_library(polycpp_winston
    src/winston.cpp
)
target_link_libraries(polycpp_winston PUBLIC polycpp)
target_include_directories(polycpp_winston PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Alias for consistent naming
add_library(polycpp::winston ALIAS polycpp_winston)

# Tests
option(POLYCPP_WINSTON_BUILD_TESTS "Build tests" ON)
if(POLYCPP_WINSTON_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_winston tests/test_winston.cpp)
    target_link_libraries(test_winston PRIVATE polycpp_winston GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(test_winston)

    add_executable(test_log_info tests/test_log_info.cpp)
    target_link_libraries(test_log_info PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_log_info)

    add_executable(test_level_config tests/test_level_config.cpp)
    target_link_libraries(test_level_config PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_level_config)

    add_executable(test_format tests/test_format.cpp)
    target_link_libraries(test_format PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_format)

    add_executable(test_colorizer tests/test_colorizer.cpp)
    target_link_libraries(test_colorizer PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_colorizer)

    add_executable(test_transport tests/test_transport.cpp)
    target_link_libraries(test_transport PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_transport)

    add_executable(test_console_transport tests/test_console_transport.cpp)
    target_link_libraries(test_console_transport PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_console_transport)

    add_executable(test_format_builtins tests/test_format_builtins.cpp)
    target_link_libraries(test_format_builtins PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_format_builtins)

    add_executable(test_logger tests/test_logger.cpp)
    target_link_libraries(test_logger PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_logger)

    add_executable(test_file_transport tests/test_file_transport.cpp)
    target_link_libraries(test_file_transport PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_file_transport)

    add_executable(test_stream_transport tests/test_stream_transport.cpp)
    target_link_libraries(test_stream_transport PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_stream_transport)

    add_executable(test_container tests/test_container.cpp)
    target_link_libraries(test_container PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_container)

    add_executable(test_exception_handler tests/test_exception_handler.cpp)
    target_link_libraries(test_exception_handler PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_exception_handler)

    add_executable(test_default_logger tests/test_default_logger.cpp)
    target_link_libraries(test_default_logger PRIVATE polycpp_winston GTest::gtest_main)
    gtest_discover_tests(test_default_logger)
endif()
