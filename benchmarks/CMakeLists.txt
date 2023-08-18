include(CTest)

set_property(TEST APPEND PROPERTY ENVIRONMENT "CTEST_DIRECTORY=${PROJECT_SOURCE_DIR}/build/benchmarks")

#find_package(GTest REQUIRED)
find_package(OpenSSL REQUIRED)
enable_testing()

#tests

add_executable(pman_bench_chainhash main_bench.cpp chainhash_bench.cpp)
target_link_libraries(pman_bench_chainhash gtest_main)
target_link_libraries(pman_bench_chainhash ${OPENSSL_LIBRARIES} pthread)
target_include_directories(pman_bench_chainhash PUBLIC ${INCLUDE_DIR})

add_test(BENCH_Chainhash pman_bench_chainhash)