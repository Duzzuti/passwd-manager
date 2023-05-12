#pragma once
#ifndef HASHMODES_H
#define HASHMODES_H

#include <sstream>
#include "settings.h"
#include "hash.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"

class HashModes{
public:
    static bool isModeValid(unsigned char const hash_mode) noexcept;
    static Hash* getHash(unsigned char const hash_mode);
    static std::string getInfo(unsigned char const hash_mode) noexcept;
};


#endif //HASHMODES_H