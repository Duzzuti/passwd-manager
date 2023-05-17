#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

bool endsWith(const std::string &fullString, const std::string &ending) noexcept;
std::vector<unsigned char> LongToCharVec(const unsigned long a) noexcept;
std::string charVecToString(const std::vector<unsigned char> v) noexcept;

#endif //UTILITY_H