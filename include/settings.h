#pragma once 
#ifndef SETTINGS_H
#define SETTINGS_H
#include <iostream>

const constexpr unsigned int MIN_PASS_LEN = 8;
const std::string VALID_PASS_CHARSET = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!§$%&/()=?{}[]@<>#*+~-_.:,;";
const constexpr unsigned char MAX_HASHMODE_NUMBER = 3;
const constexpr unsigned char STANDARD_HASHMODE = 3;
const constexpr unsigned char MAX_CHAINHASHMODE_NUMBER = 4;
const constexpr unsigned char STANDARD_CHAINHASHMODE = 4;
const constexpr unsigned long STANDARD_PASS_VAL_ITERATIONS = 1000;    //we should test how many we need
const constexpr unsigned long MIN_ITERATIONS = 1;
const constexpr unsigned long MAX_ITERATIONS = 1000000000;

#endif //SETTINGS_H