cmake_minimum_required(VERSION 3.0.0)
project(passwd-manager VERSION 0.1.0)

################################
# GTest
################################
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
enable_testing()

include(CTest)

#tests
add_executable(passwd_manager_test_bytes main_test.cpp bytes_unittest.cpp bytes.cpp)
target_link_libraries(passwd_manager_test_bytes ${GTEST_LIBRARIES} pthread)

add_executable(passwd_manager_test_block main_test.cpp block_unittest.cpp block.cpp bytes.cpp)
target_link_libraries(passwd_manager_test_block ${GTEST_LIBRARIES} pthread)


#executable
add_executable(passwd_manager main.cpp bytes.cpp block.cpp blockchain.cpp)

add_test(bytes passwd_manager_test_bytes)
add_test(block passwd_manager_test_block)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)