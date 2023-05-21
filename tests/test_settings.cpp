/*
please comment out the test modes you dont use
this module is providing some constants that are used as test settings
you can set the settings to fast or slow (but more test iterations)
*/

#include <iostream>

//easy mode (tests are running very short)
//hashing functions
const constexpr int TEST_MAX_LEN = 512;
const constexpr u_int64_t TEST_ITERS = 5;
//rng
const constexpr u_int64_t TEST_NUMBER_ENTROPY = 1000000; 
const constexpr double TEST_ENTROPY_ERROR = 0.001;
//pwfunc
const constexpr int TEST_MAX_PW_ITERS = 100;
const constexpr int TEST_MAX_PW_LEN = 20;
//dataheader
const constexpr u_int64_t TEST_DH_ITERS = 1000;



// //medium mode (tests are running normally)
// //hashing functions
// const constexpr int TEST_MAX_LEN = 512;
// const constexpr u_int64_t TEST_ITERS = 15;
// //rng
// const constexpr u_int64_t TEST_NUMBER_ENTROPY = 10000000; 
// const constexpr double TEST_ENTROPY_ERROR = 0.0001;
//pwfunc
// const constexpr int TEST_MAX_PW_ITERS = 200;
// const constexpr int TEST_MAX_PW_LEN = 32;


// //hard mode (tests are running for u_int64_ter)
// //hashing functions
// const constexpr int TEST_MAX_LEN = 512;
// const constexpr u_int64_t TEST_ITERS = 30;
// //rng
// const constexpr u_int64_t TEST_NUMBER_ENTROPY = 100000000; 
// const constexpr double TEST_ENTROPY_ERROR = 0.00001;
//pwfunc
// const constexpr int TEST_MAX_PW_ITERS = 10000;
// const constexpr int TEST_MAX_PW_LEN = 48;

