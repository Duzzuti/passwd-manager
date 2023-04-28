include(CTest)
set_property(TEST APPEND PROPERTY ENVIRONMENT "CTEST_DIRECTORY=${PROJECT_SOURCE_DIR}/build/tests")

set(TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/tests/include)

#find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

#tests
add_executable(passwd_manager_test_bytes main_test.cpp bytes_unittest.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_bytes gtest_main)
target_link_libraries(passwd_manager_test_bytes ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_bytes PUBLIC ${INCLUDE_DIR})

add_executable(passwd_manager_test_block main_test.cpp block_unittest.cpp ${SRC_DIR}/block.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_block gtest_main)
target_link_libraries(passwd_manager_test_block ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_block PUBLIC ${INCLUDE_DIR})

add_executable(passwd_manager_test_sha256 main_test.cpp sha256_unittest.cpp test_utils.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_sha256 gtest_main)
target_link_libraries(passwd_manager_test_sha256 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_sha256 PUBLIC ${INCLUDE_DIR})
target_include_directories(passwd_manager_test_sha256 PUBLIC ${TEST_INCLUDE_DIR})

add_executable(passwd_manager_test_sha384 main_test.cpp sha384_unittest.cpp test_utils.cpp ${SRC_DIR}/sha384.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_sha384 gtest_main)
target_link_libraries(passwd_manager_test_sha384 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_sha384 PUBLIC ${INCLUDE_DIR})
target_include_directories(passwd_manager_test_sha384 PUBLIC ${TEST_INCLUDE_DIR})

add_executable(passwd_manager_test_sha512 main_test.cpp sha512_unittest.cpp test_utils.cpp ${SRC_DIR}/sha512.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_sha512 gtest_main)
target_link_libraries(passwd_manager_test_sha512 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_sha512 PUBLIC ${INCLUDE_DIR})
target_include_directories(passwd_manager_test_sha512 PUBLIC ${TEST_INCLUDE_DIR})

add_executable(passwd_manager_test_rng main_test.cpp rng_unittest.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_rng gtest_main)
target_link_libraries(passwd_manager_test_rng ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_rng PUBLIC ${INCLUDE_DIR})

add_executable(passwd_manager_test_pwfunc main_test.cpp test_utils.cpp pwfunc_unittest.cpp ${SRC_DIR}/pwfunc.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(passwd_manager_test_pwfunc gtest_main)
target_link_libraries(passwd_manager_test_pwfunc ${OPENSSL_LIBRARIES} pthread)
target_include_directories(passwd_manager_test_pwfunc PUBLIC ${TEST_INCLUDE_DIR})
target_include_directories(passwd_manager_test_pwfunc PUBLIC ${INCLUDE_DIR})


add_test(bytes passwd_manager_test_bytes)
add_test(block passwd_manager_test_block)
add_test(sha256 passwd_manager_test_sha256)
add_test(sha384 passwd_manager_test_sha384)
add_test(sha512 passwd_manager_test_sha512)
add_test(rng passwd_manager_test_rng)
add_test(pwfunc passwd_manager_test_pwfunc)