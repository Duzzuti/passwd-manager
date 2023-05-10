#pragma once
#ifndef CHAINHASHMODES_H
#define CHAINHASHMODES_H

#include "hash.h"
#include "settings.h"

class ChainHashModes{
public:
    static bool isModeValid(unsigned char const chainhash_mode) noexcept;
    static bool isChainHashValid(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock) noexcept;
    static Bytes performChainHash(unsigned char const chainhash_mode, unsigned long iters, Hash* hash, Bytes data);
    static Bytes performChainHash(unsigned char const chainhash_mode, unsigned long iters, Hash* hash, std::string data);
};


#endif //HASHMODES_H