#include "hash_modes.h"

bool HashModes::isModeValid(unsigned char const hash_mode) noexcept{
    return (1 <= hash_mode <= MAX_HASHMODE_NUMBER);
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
