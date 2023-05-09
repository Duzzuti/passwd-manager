#pragma once
#ifndef MODES_H
#define MODES_H

#include "bytes.h"
#include "hash.h"

class Modes{
public:
    static const int iteration_bytes = 8;
    static int getModeBytesLen(unsigned char const mode);
    static Bytes getSalt(unsigned char const mode, std::string password, Bytes headerBytes);
    static std::string getInfo(unsigned char const mode);

private:
    static Bytes getSalt1(std::string password, Bytes headerBytes);
    static Bytes getSalt2(std::string password, Bytes headerBytes);
    static Bytes getSalt3(std::string password, Bytes headerBytes);
    static Bytes getSalt4(std::string password, Bytes headerBytes);
    static Bytes getSalt5(std::string password, Bytes headerBytes);
    static Bytes getSalt6(std::string password, Bytes headerBytes);
};

#endif //MODES_H