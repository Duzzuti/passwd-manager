#include "chainhash_modes.h"

bool ChainHashModes::isModeValid(unsigned char const chainhash_mode) noexcept{
    return (1 <= chainhash_mode <= MAX_CHAINHASHMODE_NUMBER);
}

bool ChainHashModes::isChainHashValid(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock) noexcept{
    if(!(iters > 0 && iters <= MAX_ITERATIONS)){
        return false;   //iteration number is not valid
    }
    if(datablock.getLen() > 255){
        return false;   //datablock is too long
    }
    switch (chainhash_mode){
    case 1: //no datablock needed
        return true;
    case 2: //datablock is constant salt (can be everything)
        return true;
    case 3: //count salt begin needed (8 Bytes)
        if(datablock.getLen() != 8){
            return false;   //datablock has an invalid length
        }
        return true;
    case 4: //count salt begin needed (8 Bytes + constant salt Bytes)
        if(datablock.getLen() < 8){
            return false;   //datablock too short
        }
        return true;
    default:
        return false;  //chainhash mode not valid
    }
}

Bytes ChainHashModes::performChainHash(unsigned char const chainhash_mode, unsigned long iters, Hash *hash, Bytes data){
    switch (chainhash_mode){
    case 1: //normal chainhash
        return Bytes();
    case 2: //constant salt
        return Bytes();
    case 3: //count salt
        return Bytes();
    case 4: //constant + count salt
        return Bytes();
    default:
        throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::performChainHash(unsigned char const chainhash_mode, unsigned long iters, Hash *hash, std::string data){
    switch (chainhash_mode){
    case 1: //normal chainhash
        return Bytes();
    case 2: //constant salt
        return Bytes();
    case 3: //count salt
        return Bytes();
    case 4: //constant + count salt
        return Bytes();
    default:
        throw std::invalid_argument("chainhash mode does not exist");
    }
}
