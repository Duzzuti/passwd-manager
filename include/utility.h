#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

bool endsWith(std::string const &fullString, std::string const &ending) noexcept;
std::vector<unsigned char> LongToCharVec(unsigned long a) noexcept;
std::string charVecToString(std::vector<unsigned char> v) noexcept;

#endif //UTILITY_H