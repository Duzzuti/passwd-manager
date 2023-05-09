#pragma once 
#ifndef SETTINGS_H
#define SETTINGS_H
#include <iostream>

const constexpr int MIN_PASS_LEN = 8;
const std::string VALID_PASS_CHARSET = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!§$%&/()=?{}[]@<>#*+~-_.:,;";
const constexpr unsigned char MAX_MODE_NUMBER = 6;
const constexpr unsigned char STANDARD_ENC_MODE = 6;
const constexpr long STANDARD_PASS_VAL_ITERATIONS = 1000;    //we should test how many we need
const constexpr long MIN_ITERATIONS = 1;
const constexpr long MAX_ITERATIONS = 1000000000;

#endif //SETTINGS_H