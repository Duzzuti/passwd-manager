include(CTest)
set_property(TEST APPEND PROPERTY ENVIRONMENT "CTEST_DIRECTORY=${CMAKE_CURRENT_SOURCE_DIR}/build/tests")

#set(CTEST_BINARY_DIRECTORY "${PROJECT_SOURCE_DIR}/build/tests")
#message("LOL: ${CTEST_SOURCE_DIRECTORY}")

find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

#tests
add_executable(passwd_manager_test_bytes main_test.cpp bytes_unittest.cpp ${SRC_DIR}/bytes.cpp)
target_link_libraries(passwd_manager_test_bytes ${GTEST_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_bytes PUBLIC ${INCLUDE_DIR})

add_executable(passwd_manager_test_block main_test.cpp block_unittest.cpp ${SRC_DIR}/block.cpp ${SRC_DIR}/bytes.cpp)
target_link_libraries(passwd_manager_test_block ${GTEST_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_block PUBLIC ${INCLUDE_DIR})

add_executable(passwd_manager_test_sha256 main_test.cpp sha256_unittest.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/bytes.cpp)
target_link_libraries(passwd_manager_test_sha256 ${GTEST_LIBRARIES} pthread)
target_link_libraries(passwd_manager_test_sha256 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_sha256 PUBLIC ${INCLUDE_DIR})

add_test(bytes passwd_manager_test_bytes)
add_test(block passwd_manager_test_block)
add_test(sha256 passwd_manager_test_sha256)