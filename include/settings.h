/*
this file stores some global used settings
*/
#pragma once

#include <iostream>

//##################### PASSWORD ######################
// minimum length of a password that is accepted
const constexpr unsigned int MIN_PASS_LEN = 8;
// contains all characters which are allowed. Note that this is sorted for linear search
const std::string VALID_PASS_CHARSET = "a1eo0r2ins9lt357864mdycuhkbwgzpfvxjqAWYEZSRXMTNLVBDCOIPHUGKFJQ_.-!*@$#?%&;=~+[(/]{}):§<>,";

//##################### FILEMODE ######################
// stores the maximum valid mode, all modes from 1 to this number are valid
const constexpr unsigned char MAX_FILEMODE_NUMBER = 1;
// stores the default mode
const constexpr unsigned char STANDARD_FILEMODE = 1;

//##################### HASHMODE ######################
// stores the maximum valid mode, all modes from 1 to this number are valid
const constexpr unsigned char MAX_HASHMODE_NUMBER = 3;
// stores the default mode
const constexpr unsigned char STANDARD_HASHMODE = 3;

//##################### CHAINHASHMODE #################
// stores the maximum valid mode, all modes from 1 to this number are valid
const constexpr unsigned char MAX_CHAINHASHMODE_NUMBER = 5;
// stores the default mode
const constexpr unsigned char STANDARD_CHAINHASHMODE = 4;

//##################### ITERATIONS ####################
// stores the default value for iteration count
const constexpr u_int64_t STANDARD_ITERATIONS = 1000;  // WORK we should test how many we need
// stores the maximum accepted iterations
const constexpr u_int64_t MIN_ITERATIONS = 1;
// stores the minimum accepted iterations
const constexpr u_int64_t MAX_ITERATIONS = 1000000000;
// stores the maximum accepted runtime in ms for the chainhash
const constexpr u_int64_t MAX_RUNTIME = 1000 * 60 * 60 * 24;  // 1 day

//##################### TIMER #########################
// stores the number of iteration that can passed from the chainhash before the timeout is checked
// a higher value does increase performance but leads to more inaccurate timeout calls
const constexpr u_int64_t TIMEOUT_ITERATIONS = 1000;

//##################### HEADER ##########################
const constexpr u_int16_t MAX_HEADER_SIZE = 38 + 2 * 64 + 2 * 255;