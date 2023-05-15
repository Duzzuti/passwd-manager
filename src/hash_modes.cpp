#include "hash_modes.h"

bool HashModes::isModeValid(unsigned char const hash_mode) noexcept{
    //checks whether the given hash mode is in the accepted range
    return (1 <= hash_mode && hash_mode <= MAX_HASHMODE_NUMBER);
}

Hash* HashModes::getHash(unsigned char const hash_mode){
    switch(hash_mode){
        case 1: //sha256
            return new sha256();
        case 2: //sha384
            return new sha384();
        case 3: //sha512
            return new sha512();
        default:
            throw std::invalid_argument("hash mode does not exist");
    }
}

std::string HashModes::getInfo(unsigned char const hash_mode) noexcept{
    std::stringstream msg{}; 
    msg << "The hash mode " << +hash_mode << " corresponds to hash function: ";
    switch (hash_mode){
        case 1: //sha256
            msg << "Sha256";
            break;
        case 2: //sha384
            msg << "Sha384";
            break;
        case 3: //sha512
            msg << "Sha512";
            break;
    }
    return msg.str();
}
