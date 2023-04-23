cmake_minimum_required(VERSION 3.0.0)
project(passwd-manager VERSION 0.1.0)

################################
# GTest
################################
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
enable_testing()

include(CTest)

add_executable(passwd-manager-test main_test.cpp bytes_unittest.cpp bytes.cpp)
target_link_libraries(passwd-manager-test ${GTEST_LIBRARIES} pthread)
add_executable(passwd-manager main.cpp bytes.cpp)

add_test(bytes passwd-manager-test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)