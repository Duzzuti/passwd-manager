include(CTest)

set_property(TEST APPEND PROPERTY ENVIRONMENT "CTEST_DIRECTORY=${PROJECT_SOURCE_DIR}/build/benchmarks")

#find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

#tests

add_executable(pman_bench_chainhash main_bench.cpp chainhash_bench.cpp 
    ${SRC_DIR}/chainhash_modes.cpp ${SRC_DIR}/bytes.cpp ${SRC_DIR}/timer.cpp ${SRC_DIR}/chainhash_data.cpp ${SRC_DIR}/format.cpp
    ${SRC_DIR}/pwfunc.cpp ${SRC_DIR}/utility.cpp ${SRC_DIR}/hash_modes.cpp ${SRC_DIR}/rng.cpp ${SRC_DIR}/sha256.cpp ${SRC_DIR}/sha384.cpp ${SRC_DIR}/sha512.cpp)
target_link_libraries(pman_bench_chainhash gtest_main)
target_link_libraries(pman_bench_chainhash ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_bench_chainhash PUBLIC ${INCLUDE_DIR})

add_test(BENCH_Chainhash pman_bench_chainhash)