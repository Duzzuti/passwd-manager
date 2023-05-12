#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

bool endsWith(std::string const &fullString, std::string const &ending);
std::vector<unsigned char> LongToCharVec(unsigned long a);

#endif //UTILITY_H