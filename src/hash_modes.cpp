/*
this file contains implementations of HashModes class
*/
#include "hash_modes.h"
#include "settings.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"

bool HashModes::isModeValid(const unsigned char hash_mode) noexcept{
    //checks whether the given hash mode is in the accepted range
    return (1 <= hash_mode && hash_mode <= MAX_HASHMODE_NUMBER);
}

Hash* HashModes::getHash(const HModes hash_mode){
    //gets the concrete hash sub class
    switch(hash_mode){
        case HASHMODE_SHA256: //sha256
            return new sha256();
        case HASHMODE_SHA384: //sha384
            return new sha384();
        case HASHMODE_SHA512: //sha512
            return new sha512();
        default:    //hash mode is out of range
            throw std::invalid_argument("hash mode does not exist");
    }
}

std::string HashModes::getInfo(const HModes hash_mode){
    //gets some information about this hash mode (hash function)
    std::stringstream msg{}; 
    //start with a base string
    msg << "The hash mode " << +hash_mode << " corresponds to hash function: ";
    //add the information based on the mode
    switch (hash_mode){
        case HASHMODE_SHA256: //sha256
            msg << "Sha256";
            break;
        case HASHMODE_SHA384: //sha384
            msg << "Sha384";
            break;
        case HASHMODE_SHA512: //sha512
            msg << "Sha512";
            break;
        default:    //hash mode is out of range
            throw std::invalid_argument("hash mode does not exist");
    }
    return msg.str();
}
