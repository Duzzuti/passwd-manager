#include "chainhash_modes.h"
#include "rng.h"
#include "app.h"
#include "utility.h"

std::string ChainHashModes::getInfo(unsigned char const chainhash_mode){
    std::stringstream msg{}; 
    msg << "The chainhash mode " << +chainhash_mode << " corresponds to a chainhash ";
    switch(chainhash_mode){
        case 1: //normal chainhash 
            msg << "that hashes the old hash again";
            break;
        case 2: //constant salt
            msg << "that hashes the old hash + a constant salt again";
            break;
        case 3: //count salt
            msg << "that hashes the old hash + an incrementing salt again";
            break;
        case 4: //count salt and constant salt
            msg << "that hashes the old hash + a constant AND an incrementing salt again";
            break;
        case 5: //quadratic salt
            msg << "that hashes the old hash + quadratic value derived from an incrementing salt and 3 coefficients";
            break;
        default:
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::askForSaltString(Bytes bytes, std::string msg, unsigned char const max_len) noexcept
{
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do{
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);
        std::cout << std::endl;
    }while(inp.length() <= max_len);
    if(inp.empty()){    //user wants to generate it randomly
        Bytes tmpb = Bytes();
        tmpb.setBytes(RNG::get_random_bytes(255));
        bytes.addBytes(tmpb);
        return bytes;
    }
    for(unsigned char i=0; i < inp.length(); i++){
        bytes.addByte(inp[i]);    //implicit char -> unsigned char
    }
    return bytes;
}

Bytes ChainHashModes::askForSaltNumber(Bytes bytes, std::string msg, unsigned char const max_len) noexcept{
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do{
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);
        std::cout << std::endl;
    }while(App::isValidNumber(inp, true));
    if(inp.empty()){    //user wants to generate it randomly
        Bytes tmpb = Bytes();
        tmpb.setBytes(RNG::get_random_bytes(8));
        bytes.addBytes(tmpb);
        return bytes;
    }
    unsigned long inp_int = std::stoul(inp);
    Bytes tmpb = Bytes();
    tmpb.setBytes(LongToCharVec(inp_int));
    bytes.addBytes(tmpb);
    return bytes;
}

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
    case 5: //count salt begin needed (8 Bytes for quadratic count salt, and 3*8 Bytes for a,b,c)
        if(datablock.getLen() != 4*8){
        return false;   //datablock has an invalid length
        }
        return true;
    default:
        return false;  //chainhash mode not valid
    }
}

Bytes ChainHashModes::askForData(unsigned char const chainhash_mode){
    std::string inp{};
    std::string blank = " or leave this field blank to generate it randomly: ";
    Bytes ret = Bytes();
    unsigned long inp_int = 0;
    switch(chainhash_mode){
        case 1: //normal chainhash (no data needed)
            return Bytes();
        case 2: //constant salt
            return ChainHashModes::askForSaltString(ret, "Please enter the salt that should be mixed into the chainhash", 255);
        case 3: //count salt
            return ChainHashModes::askForSaltNumber(ret, "Please enter the start number for the count salt");
        case 4: //count salt and constant salt
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the start number for the count salt");
            return ChainHashModes::askForSaltString(ret, "Please enter the salt that should be mixed into the chainhash", 247);
        case 5: //quadratic salt
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the start number for the count salt");
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the value of quadratic part a");
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the value of linear part b");
            return ChainHashModes::askForSaltNumber(ret, "Please enter the value of constant part c");
        default:
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::performChainHash(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock, Hash* hash, Bytes data){
    switch (chainhash_mode){
    case 1: //normal chainhash
        return Bytes();
    case 2: //constant salt
        return Bytes();
    case 3: //count salt
        return Bytes();
    case 4: //constant + count salt
        return Bytes();
    case 5: //Quadratic count salt
        return Bytes();
    default:
        throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::performChainHash(unsigned char const chainhash_mode, unsigned long iters, Bytes datablock, Hash* hash, std::string data){
    switch (chainhash_mode){
    case 1: //normal chainhash
        return Bytes();
    case 2: //constant salt
        return Bytes();
    case 3: //count salt
        return Bytes();
    case 4: //constant + count salt
        return Bytes();
    case 5: //quadratic count salt
        return Bytes();
    default:
        throw std::invalid_argument("chainhash mode does not exist");
    }
}
