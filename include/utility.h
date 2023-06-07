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
// transforms a u_int64_t into a byte array
std::vector<unsigned char> LongToCharVec(const u_int64_t a) noexcept;
// transforms a byte array into a string
std::string charVecToString(const std::vector<unsigned char> v) noexcept;
// transforms a string into a byts object
Bytes stringToBytes(const std::string str) noexcept;
// gets the byte length of a long number
unsigned int getLongLen(const u_int64_t num) noexcept;
