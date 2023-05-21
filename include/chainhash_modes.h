#pragma once
#ifndef CHAINHASHMODES_H
#define CHAINHASHMODES_H

#include "hash.h"
#include "settings.h"
#include "chainhash_data.h"

enum CHModes : unsigned char{
    CHAINHASH_NORMAL = 1,
    CHAINHASH_CONSTANT_SALT,
    CHAINHASH_COUNT_SALT,
    CHAINHASH_CONSTANT_COUNT_SALT,
    CHAINHASH_QUADRATIC
};

class ChainHashModes{
    /*
    this class provides static methods all around chainhashes
    you can perform a chainhash or check if a chainhash mode is valid etc.
    A chainhash is a hash function that is applied multiple times
    Each chainhash mode has an own way of applying the hash function 
    We can add some salt before we hash again and this salt is calculated differently in each mode
    */
public:
    static std::string getInfo(const CHModes chainhash_mode); //gets a string that conatins information about this chainhash mode
    //ask the user for a string with a given message and max length and returns the bytes of the string
    static Bytes askForSaltString(const std::string msg, const unsigned char max_len);
    //ask the user for a number with a given message and max byte len (8 for ul) and returns the bytes of the number
    static Bytes askForSaltNumber(const std::string msg, const unsigned char max_len=8);
    static bool isModeValid(const unsigned char chainhash_mode) noexcept;   //checks if the given chain hash mode is valid
    //checks if the given chainhash is valid (with the iterations and datablock which contains data that is used by the chainhash)
    static bool isChainHashValid(const CHModes chainhash_mode, const u_int64_t iters, const ChainHashData datablock) noexcept;
    static ChainHashData askForData(const CHModes chainhash_mode);    //gets data from the user that is needed for this chainhash mode
    //two methods for actually performing the chainhash, one for Bytes input and one for string input
    static Bytes performChainHash(const CHModes chainhash_mode, const u_int64_t iters, ChainHashData datablock, const Hash* hash, const Bytes data);
    static Bytes performChainHash(const CHModes chainhash_mode, const u_int64_t iters, ChainHashData datablock, const Hash* hash, const std::string data);
};


#endif //HASHMODES_H