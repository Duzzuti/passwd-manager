/*
contains implementations of utility functions from utility.h
*/
#include "utility.h"

#include <cstring>
#include <vector>

#include "logger.h"

bool endsWith(const std::string& fullString, const std::string& ending) noexcept {
    // checks if a string ends with a given string
    if (fullString.length() >= ending.length()) {
        // do the check
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;  // if the ending is longer than the actual string, it cannot end with the ending
    }
}

Bytes stringToBytes(const std::string str) noexcept {
    // transforms a string into a Bytes object
    const unsigned char* str_arr = reinterpret_cast<const unsigned char*>(str.c_str());  // to get the char array from the string
    Bytes ret(str.length());
    ret.setBytes(str_arr, str.length());
    return ret;
}

std::string bytesToString(const Bytes & bytes) noexcept{
    // transforms a Bytes object into a string
    return std::string(reinterpret_cast<const char*>(bytes.getBytes()), bytes.getLen());
}

void addStringToBytes(const std::string& str, Bytes& bytes) {
    // adds a string to a Bytes object
    if (str.length() > bytes.getMaxLen() - bytes.getLen()) {
        // if the string is longer than the remaining space in the bytes object, throw an error
        PLOG_FATAL << "string is too long to be added to the bytes object";
        throw std::runtime_error("string is too long to be added to the bytes object");
    }
    bytes.addBytes(reinterpret_cast<const unsigned char*>(str.c_str()), str.length());
}

unsigned int getLongLen(const u_int64_t& num) noexcept {
    // gets the byte length of a long number
    unsigned int ret = 0;
    u_int64_t tmp = num;
    while (tmp > 0) {
        // shift the number 8 bits to the right and increment the counter
        //(this is the same as dividing by 256 (byte) and incrementing the counter)
        tmp = tmp >> 8;
        ret++;
    }
    return ret;
}

bool isValidNumber(const std::string& number, const bool accept_blank, const u_int64_t& lower_bound, const u_int64_t& upper_bound) noexcept {
    if (number.empty()) {
        // if the number string was empty return if blank is accepted
        return accept_blank;
    }
    for (char c : number) {
        // check if the number string contains only numbers
        if (!isdigit(c)) {
            return false;
        }
    }
    u_int64_t long_number;
    try {
        long_number = std::stoul(number);  // transform the number string to a number
    } catch (std::exception) {
        return false;  // number string could not be transformed into an u_int64_t
    }
    // checks the bounds of the number
    if (lower_bound <= long_number && long_number <= upper_bound) {
        return true;
    }
    return false;  // given number is not in the valid number bounds
}