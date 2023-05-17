#pragma once
#ifndef CHAINHASHMODES_H
#define CHAINHASHMODES_H

#include "hash.h"
#include "settings.h"

class ChainHashModes{
    /*
    this class provides static methods all around chainhashes
    you can perform a chainhash or check if a chainhash mode is valid etc.
    A chainhash is a hash function that is applied multiple times
    Each chainhash mode has an own way of applying the hash function 
    We can add some salt before we hash again and this salt is calculated differently in each mode
    */
public:
    static std::string getInfo(unsigned char const chainhash_mode); //gets a string that conatins information about this chainhash mode
    //ask the user for a string with a given message and max length and appends the bytes of the string to the given bytes and returns them
    static Bytes askForSaltString(Bytes bytes, std::string msg, unsigned char const max_len);
    //ask the user for a number with a given message and max byte len (8 for ul) and appends the bytes of the number to the given bytes and returns them
    static Bytes askForSaltNumber(Bytes bytes, std::string msg, unsigned char const max_len=8);
    static bool isModeValid(unsigned char const chainhash_mode) noexcept;   //checks if the given chain hash mode is valid
    //checks if the given chainhash is valid (with the iterations and datablock which contains data that is used by the chainhash)
    static bool isChainHashValid(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock) noexcept;
    static Bytes askForData(unsigned char const chainhash_mode);    //gets data from the user that is needed for this chainhash mode
    //two methods for actually performing the chainhash, one for Bytes input and one for string input
    static Bytes performChainHash(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock, Hash* hash, Bytes data);
    static Bytes performChainHash(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock, Hash* hash, std::string data);
};


#endif //HASHMODES_H