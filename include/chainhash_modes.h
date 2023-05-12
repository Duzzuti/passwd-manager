#pragma once
#ifndef CHAINHASHMODES_H
#define CHAINHASHMODES_H

#include "hash.h"
#include "settings.h"

class ChainHashModes{
public:
    static std::string getInfo(unsigned char const chainhash_mode);
    static Bytes askForSaltString(Bytes bytes, std::string msg, unsigned char const max_len) noexcept;
    static Bytes askForSaltNumber(Bytes bytes, std::string msg, unsigned char const max_len=8) noexcept;
    static bool isModeValid(unsigned char const chainhash_mode) noexcept;
    static bool isChainHashValid(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock) noexcept;
    static Bytes askForData(unsigned char const chainhash_mode);
    static Bytes performChainHash(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock, Hash* hash, Bytes data);
    static Bytes performChainHash(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock, Hash* hash, std::string data);
};


#endif //HASHMODES_H