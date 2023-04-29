/*
please comment out the test modes you dont use
this module is providing some constants that are used as test settings
you can set the settings to fast or slow (but more test iterations)
*/

//easy mode (tests are running very short)
//hashing functions
const constexpr int SET_MAX_LEN = 512;
const constexpr long SET_ITERS = 5;
//rng
const constexpr long SET_NUMBER_ENTROPY = 1000000; 
const constexpr double SET_ENTROPY_ERROR = 0.001;
//pwfunc
const constexpr int SET_PW_ITERS = 500;
const constexpr int SET_MAX_PW_LEN = 256;



// //medium mode (tests are running normally)
// //hashing functions
// const constexpr int SET_MAX_LEN = 512;
// const constexpr long SET_ITERS = 15;
// //rng
// const constexpr long SET_NUMBER_ENTROPY = 10000000; 
// const constexpr double SET_ENTROPY_ERROR = 0.0001;
//pwfunc
// const constexpr int SET_PW_ITERS = 1500;
// const constexpr int SET_MAX_PW_LEN = 256;


// //hard mode (tests are running for longer)
// //hashing functions
// const constexpr int SET_MAX_LEN = 512;
// const constexpr long SET_ITERS = 30;
// //rng
// const constexpr long SET_NUMBER_ENTROPY = 100000000; 
// const constexpr double SET_ENTROPY_ERROR = 0.00001;
//pwfunc
// const constexpr int SET_PW_ITERS = 3000;
// const constexpr int SET_MAX_PW_LEN = 256;

