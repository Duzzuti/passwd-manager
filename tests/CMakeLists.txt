include(CTest)
set_property(TEST APPEND PROPERTY ENVIRONMENT "CTEST_DIRECTORY=${PROJECT_SOURCE_DIR}/build/tests")

set(TEST_INCLUDE_DIR ${TEST_DIR}/include)
set(ATTACKER_DIR ${TEST_DIR}/Attacker)

#find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

#tests
add_executable(pman_test_bytes main_test.cpp bytes_unittest.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_bytes gtest_main)
target_link_libraries(pman_test_bytes ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_bytes PUBLIC ${INCLUDE_DIR})

add_executable(pman_test_block main_test.cpp block_unittest.cpp ${SRC_DIR}/block.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_block gtest_main)
target_link_libraries(pman_test_block ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_block PUBLIC ${INCLUDE_DIR})

add_executable(pman_test_sha256 main_test.cpp sha256_unittest.cpp test_utils.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_sha256 gtest_main)
target_link_libraries(pman_test_sha256 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_sha256 PUBLIC ${INCLUDE_DIR})
target_include_directories(pman_test_sha256 PUBLIC ${TEST_INCLUDE_DIR})

add_executable(pman_test_sha384 main_test.cpp sha384_unittest.cpp test_utils.cpp ${SRC_DIR}/sha384.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_sha384 gtest_main)
target_link_libraries(pman_test_sha384 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_sha384 PUBLIC ${INCLUDE_DIR})
target_include_directories(pman_test_sha384 PUBLIC ${TEST_INCLUDE_DIR})

add_executable(pman_test_sha512 main_test.cpp sha512_unittest.cpp test_utils.cpp ${SRC_DIR}/sha512.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_sha512 gtest_main)
target_link_libraries(pman_test_sha512 ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_sha512 PUBLIC ${INCLUDE_DIR})
target_include_directories(pman_test_sha512 PUBLIC ${TEST_INCLUDE_DIR})

add_executable(pman_test_rng main_test.cpp rng_unittest.cpp ${SRC_DIR}/rng.cpp ${SRC_DIR}/bytes.cpp)
target_link_libraries(pman_test_rng gtest_main)
target_link_libraries(pman_test_rng ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_rng PUBLIC ${INCLUDE_DIR})

add_executable(pman_test_pwfunc main_test.cpp test_utils.cpp pwfunc_unittest.cpp ${SRC_DIR}/pwfunc.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_pwfunc gtest_main)
target_link_libraries(pman_test_pwfunc ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_pwfunc PUBLIC ${TEST_INCLUDE_DIR})
target_include_directories(pman_test_pwfunc PUBLIC ${INCLUDE_DIR})

add_executable(pman_test_utility main_test.cpp utility_unittest.cpp ${SRC_DIR}/utility.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_utility gtest_main)
target_link_libraries(pman_test_utility ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_utility PUBLIC ${TEST_INCLUDE_DIR})
target_include_directories(pman_test_utility PUBLIC ${INCLUDE_DIR})

add_executable(pman_test_dataheader 
    main_test.cpp dataheader_unittest.cpp 
    ${SRC_DIR}/password_data.cpp ${SRC_DIR}/filehandler.cpp ${SRC_DIR}/app.cpp ${SRC_DIR}/utility.cpp ${SRC_DIR}/file_modes.cpp 
    ${SRC_DIR}/hash_modes.cpp ${SRC_DIR}/pwfunc.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/sha384.cpp ${SRC_DIR}/sha512.cpp ${SRC_DIR}/format.cpp
    ${SRC_DIR}/chainhash_data.cpp ${SRC_DIR}/chainhash_modes.cpp ${SRC_DIR}/dataheader.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp)
target_link_libraries(pman_test_dataheader gtest_main)
target_link_libraries(pman_test_dataheader ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_dataheader PUBLIC ${TEST_INCLUDE_DIR})
target_include_directories(pman_test_dataheader PUBLIC ${INCLUDE_DIR})

add_executable(pman_test_attacker main_test.cpp ${ATTACKER_DIR}/attacker_unittest.cpp
    ${ATTACKER_DIR}/base_attacker.cpp ${ATTACKER_DIR}/brute_pw_attacker.cpp 
    ${SRC_DIR}/dataheader.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/rng.cpp ${SRC_DIR}/chainhash_modes.cpp ${SRC_DIR}/hash_modes.cpp
    ${SRC_DIR}/file_modes.cpp ${SRC_DIR}/chainhash_data.cpp ${SRC_DIR}/format.cpp ${SRC_DIR}/utility.cpp
    ${SRC_DIR}/pwfunc.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/sha384.cpp ${SRC_DIR}/sha512.cpp ${SRC_DIR}/app.cpp
    ${SRC_DIR}/filehandler.cpp ${SRC_DIR}/password_data.cpp)
target_link_libraries(pman_test_attacker gtest_main)
target_link_libraries(pman_test_attacker ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_test_attacker PUBLIC ${INCLUDE_DIR})
target_link_directories(pman_test_attacker PUBLIC ${ATTACKER_DIR})

add_test(bytes pman_test_bytes)
add_test(block pman_test_block)
add_test(sha256 pman_test_sha256)
add_test(sha384 pman_test_sha384)
add_test(sha512 pman_test_sha512)
add_test(rng pman_test_rng)
add_test(pwfunc pman_test_pwfunc)
add_test(utility pman_test_utility)
add_test(dataheader pman_test_dataheader)
add_test(attacker pman_test_attacker)