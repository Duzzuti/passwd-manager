/*
please comment out the test modes you dont use
this module is providing some constants that are used as test settings
you can set the settings to fast or slow (but more test iterations)

DOCUMENTATION:
the easy mode is documented.
All modes have obviously the same variables and semantics.
It is not neccessary to document all modes
before the variable there is a string in the format: {<charactersA>; <charactersB>}
characterA describes the consequences of incrementing this number in terms of time usage
characterB describes the consequences of incrementing this number in terms of functionality

A:
    F - runs faster
    N - neutral
    S - runs slower
    T<num> - runs slower until num
B:
    T<num> - more testcases are tested but it is unneccessary; but should be greater or equal to num
    M - more testcases are tested; but it is not strictly unneccessary
    A - more testcases are tested; the accuracy is strictly increasing
    D<char><var> - depends on var; inverse if char == i; proportional if char == p
    B - functionality or accuracy is declining
    G - functionality or accuracy is increasing
    R<num1>|<num2> - value should be between these nums
    N<num> - better accuracy until num;
*/

#include <iostream>

const constexpr bool TEST_VERBOSE = false;  // should the tests print information?

//####################### EASY MODE (tests are running fast) ######################################
//+++++++++++++++++hashing functions+++++++++++++++++++++++
//{S; T256}
const constexpr int TEST_HASH_MAX_LEN = 512;  // up to how many characters should we test hashing
//{S; M}
const constexpr u_int64_t TEST_HASH_ITERS = 5;  // how many times should we test one string length
//+++++++++++++++++rng+++++++++++++++++++++++++++++++++++++
//{S; A}
const constexpr u_int64_t TEST_RNG_ITERS_ENTROPY = 1000000;  // num of bytes for the entropy bytes test
//{N; DiTEST_RNG_ITERS_ENTROPY, B}
const constexpr double TEST_RNG_ENTROPY_ERROR = 0.001;  // max accepted error for the entropy bytes test
//{S; M}
const constexpr u_int64_t TEST_RNG_ITERS_IO = 100000;  // number of calls made to test the input/output of the method
//{N; R1|8, G}
const constexpr unsigned char TEST_RNG_BYTE_BUFFER_SIZE = 8;  // buffer size for entropy byte test
//{S; A}
const constexpr u_int64_t TEST_RNG_ITERS_BYTE_ENTROPY = 10000;  // num of bytes for the entropy byte test with one setting
//{S; T2}
const constexpr unsigned char TEST_RNG_VALUES_PER_RANGE = 2;  // num of tested settings per range (lower, upper bound)
//{N; DiTEST_RNG_ITERS_BYTE_ENTROPY, B}
const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR = 0.025;  // max accepted error for the entropy byte test with big buffer
//{S; A}
const constexpr u_int64_t TEST_RNG_ITERS_BYTE_BUFFER_ENTROPY = 10000;  // num of bytes for the entropy buffer test for one range and buffer
//{N; DiTEST_RNG_ITERS_BYTE_ENTROPY, B}
const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR_1BUFFER = 0.2;  // max accepted error for the entropy byte test with 1 byte buffer
//{N; DiTEST_RNG_ITERS_BYTE_ENTROPY, B}
const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR_2BUFFER = 0.04;  // max accepted error for the entropy byte test with 2 byte buffer
//+++++++++++++++++pwfunc++++++++++++++++++++++++++++++++++
//{T256^2; N256^2}
const constexpr int TEST_MAX_PW_ITERS = 10000;  // number of iterations for the chainhash functions
//{S; T16}
const constexpr int TEST_MAX_PW_LEN = 20;  // up to which password length should be tested
//+++++++++++++++++dataheader++++++++++++++++++++++++++++++
//{S; A}
const constexpr u_int64_t TEST_DH_ITERS = 10000;  // number of dataheaders tested
//{S; T1000}
const constexpr u_int64_t TEST_DH_MAX_PW_ITERS = 10000;  // number of iterations for the chainhash functions in dataheader
//{S; A}
const constexpr u_int64_t TEST_DH_CALC_ITERS = 15;  // number of iterations for the calculation test of the dataheader

// //####################### MEDIUM MODE (tests are running normally) #############################
// const constexpr int TEST_HASH_MAX_LEN = 512;
// const constexpr u_int64_t TEST_HASH_ITERS = 10;
// const constexpr u_int64_t TEST_RNG_ITERS_ENTROPY = 5000000;
// const constexpr double TEST_RNG_ENTROPY_ERROR = 0.0005;
// const constexpr u_int64_t TEST_RNG_ITERS_IO = 1000000;
// const constexpr unsigned char TEST_RNG_BYTE_BUFFER_SIZE = 8;
// const constexpr u_int64_t TEST_RNG_ITERS_BYTE_ENTROPY = 20000;
// const constexpr unsigned char TEST_RNG_VALUES_PER_RANGE = 2;
// const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR = 0.015;
// const constexpr u_int64_t TEST_RNG_ITERS_BYTE_BUFFER_ENTROPY = 20000;
// const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR_1BUFFER = 0.15;
// const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR_2BUFFER = 0.02;

// const constexpr int TEST_MAX_PW_ITERS = 65000;
// const constexpr int TEST_MAX_PW_LEN = 20;

// const constexpr u_int64_t TEST_DH_ITERS = 20000;
// const constexpr u_int64_t TEST_DH_MAX_PW_ITERS = 20000;
// const constexpr u_int64_t TEST_DH_CALC_ITERS = 30;

// //####################### HARD MODE (tests are running slow) ###################################
// const constexpr int TEST_HASH_MAX_LEN = 512;
// const constexpr u_int64_t TEST_HASH_ITERS = 20;

// const constexpr u_int64_t TEST_RNG_ITERS_ENTROPY = 10000000;
// const constexpr double TEST_RNG_ENTROPY_ERROR = 0.0001;
// const constexpr u_int64_t TEST_RNG_ITERS_IO = 5000000;
// const constexpr unsigned char TEST_RNG_BYTE_BUFFER_SIZE = 8;
// const constexpr u_int64_t TEST_RNG_ITERS_BYTE_ENTROPY = 50000;
// const constexpr unsigned char TEST_RNG_VALUES_PER_RANGE = 2;
// const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR = 0.01;
// const constexpr u_int64_t TEST_RNG_ITERS_BYTE_BUFFER_ENTROPY = 30000;
// const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR_1BUFFER = 0.1;
// const constexpr double TEST_RNG_BYTE_ENTROPY_ERROR_2BUFFER = 0.01;

// const constexpr u_int64_t TEST_MAX_PW_ITERS = 65000;
// const constexpr int TEST_MAX_PW_LEN = 20;

// const constexpr u_int64_t TEST_DH_ITERS = 50000;
// const constexpr u_int64_t TEST_DH_MAX_PW_ITERS = 20000;
// const constexpr u_int64_t TEST_DH_CALC_ITERS = 50;
