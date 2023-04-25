cmake_minimum_required(VERSION 3.0.0)
project(passwd-manager VERSION 0.1.0)
################################
# GTest
################################
find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/tests)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(${SRC_DIR})
add_subdirectory(${TEST_DIR})


