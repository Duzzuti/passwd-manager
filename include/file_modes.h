#pragma once
#ifndef FILEMODES_H
#define FILEMODES_H

#include <iostream>

class FileModes{
public:
    static std::string getInfo(unsigned char const file_mode) noexcept;
    static bool isModeValid(unsigned char const file_mode) noexcept;
};


#endif //FILEMODES_H