/*
this file stores some global used settings
*/
#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

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

#endif  // SETTINGS_H
