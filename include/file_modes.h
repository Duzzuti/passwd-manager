#pragma once
#ifndef FILEMODES_H
#define FILEMODES_H

#include <iostream>

class FileModes{
public:
    static std::string getInfo(const unsigned char file_mode) noexcept;
    static bool isModeValid(const unsigned char file_mode) noexcept;
};


#endif //FILEMODES_H