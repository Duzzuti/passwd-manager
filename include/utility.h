/*
this file contains some useful global used functions
if a functionality is used in different files or scenarios, it should be defined here
*/
#pragma once

#include <iostream>
#include <vector>

#include "bytes.h"

// checks if a string ends with a string
bool endsWith(const std::string& fullString, const std::string& ending) noexcept;
// transforms a string into a bytes object
Bytes stringToBytes(const std::string str) noexcept;
// adds a string to a bytes object
void addStringToBytes(const std::string& str, Bytes& bytes);
// gets the byte length of a long number
unsigned int getLongLen(const u_int64_t& num) noexcept;
// checks if a string is a valid number
bool isValidNumber(const std::string& number, const bool accept_blank = false, const u_int64_t& lower_bound = 0, const u_int64_t& upper_bound = -1) noexcept;