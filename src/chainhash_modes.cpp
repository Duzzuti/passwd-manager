/*
contains the implementations for the chainhashModes class
*/
#include "chainhash_modes.h"
#include "rng.h"
#include "app.h"
#include "utility.h"
#include "pwfunc.h"

std::string ChainHashModes::getInfo(const unsigned char chainhash_mode){
    //gets some information about the given chainhash mode
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
        default: //invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
    return msg.str();
}

Bytes ChainHashModes::askForSaltString(Bytes bytes, const std::string msg, const unsigned char max_len){
    //asks the user for a string that is used as a salt and added to the given bytes
    if(max_len < 1){
        //if max len is zero it means that this function is useless, throw an error to prohibit bugs
        throw std::runtime_error("maximum length of the string that should be got from the user does not make sense");
    }
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do{
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);     //gets the user input
    }while(inp.length() > max_len); //checks if the user input is not too long
    if(inp.empty()){    //user wants to generate it randomly if nothing was entered
        Bytes tmpb = Bytes();
        tmpb.setBytes(RNG::get_random_bytes(max_len));  //generate random bytes
        bytes.addBytes(tmpb);       //appends the new bytes
        return bytes;
    }
    //loops over the string and add each char as a byte to the Bytes object
    for(unsigned char i=0; i < inp.length(); i++){
        bytes.addByte(inp[i]);    //implicit char -> unsigned char
    }
    return bytes;
}

Bytes ChainHashModes::askForSaltNumber(Bytes bytes, const std::string msg, const unsigned char max_len){
    //asks the user for a number that is used as a salt and added to the given bytes
    if(max_len > 8 || max_len < 1){
        //if max len is zero or greater than 8 (unsigned long) it means that this function is useless, throw an error to prohibit bugs
        throw std::runtime_error("maximum length of the number from the user does not make sense");
    }
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do{
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);     //gets the user input
    }while(!App::isValidNumber(inp, true)); //stops if the entered string is a valid number
    if(inp.empty()){    //user wants to generate it randomly
        Bytes tmpb = Bytes();
        tmpb.setBytes(RNG::get_random_bytes(max_len));    //generate Bytes to represent a random number
        bytes.addBytes(tmpb);
        return bytes;
    }
    unsigned long inp_int = std::stoul(inp);
    Bytes tmpb = Bytes();
    tmpb.setBytes(LongToCharVec(inp_int));
    bytes.addBytes(tmpb);
    return bytes;
}

bool ChainHashModes::isModeValid(const unsigned char chainhash_mode) noexcept{
    //checks whether the chainhash mode is in the range of valid modes
    return (1 <= chainhash_mode && chainhash_mode <= MAX_CHAINHASHMODE_NUMBER);
}

bool ChainHashModes::isChainHashValid(const unsigned char chainhash_mode, const unsigned long iters, const Bytes datablock) noexcept{
    //checks if the chainhash is valid
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

Bytes ChainHashModes::askForData(const unsigned char chainhash_mode){
    //gets data from the user based on the mode
    std::string inp{};
    std::string blank = " or leave this field blank to generate it randomly: ";
    Bytes ret = Bytes();
    unsigned long inp_int = 0;
    switch(chainhash_mode){
        case 1: //normal chainhash (no data needed)
            return Bytes();
        case 2: //constant salt, we need a salt string
            return ChainHashModes::askForSaltString(ret, "Please enter the salt that should be mixed into the chainhash", 255);
        case 3: //count salt, we need a start number for the count salt
            return ChainHashModes::askForSaltNumber(ret, "Please enter the start number for the count salt");
        case 4: //count salt and constant salt, we need a start number for the count salt and a salt string 
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the start number for the count salt");
            return ChainHashModes::askForSaltString(ret, "Please enter the salt that should be mixed into the chainhash", 247);
        case 5: //quadratic salt, we need the start salt number and 3 numbers for a,b,c (a*a*salt + b*salt + c)
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the start number for the count salt");
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the value of quadratic part a");
            ret = ChainHashModes::askForSaltNumber(ret, "Please enter the value of linear part b");
            return ChainHashModes::askForSaltNumber(ret, "Please enter the value of constant part c");
        default:    //invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::performChainHash(const unsigned char chainhash_mode, const unsigned long iters, Bytes datablock, const Hash* hash, const Bytes data){
    //performs a chainhash on bytes
    if(!ChainHashModes::isChainHashValid(chainhash_mode, iters, datablock)){
        throw std::invalid_argument("ChainHash arguments are not valid");   //chainhash is not valid
    }
    PwFunc pwf = PwFunc(hash);      //init the pwfunc object with the given hash function
    std::string constant_salt{};    //init all variables we might need, because in the switch statement no variables can be declared
    unsigned long count_salt{};
    unsigned long a{};
    unsigned long b{};
    unsigned long c{};
    switch (chainhash_mode){
    case 1: //normal chainhash
        return pwf.chainhash(data, iters);  //just use the iterations
    case 2: //constant salt
        constant_salt = charVecToString(datablock.getBytes());  //get the salt from the datablock
        return pwf.chainhashWithConstantSalt(data, iters, constant_salt);   //use the iterations and the constant salt
    case 3: //count salt
        count_salt = toLong(datablock.popFirstBytes(8).value());    //get the start number of the salt
        return pwf.chainhashWithCountSalt(data, iters, count_salt); //use the iterations and the count salt
    case 4: //constant + count salt
        count_salt = toLong(datablock.popFirstBytes(8).value());    //get the start number of the salt
        constant_salt = charVecToString(datablock.getBytes());      //get the salt from the datablock
        return pwf.chainhashWithCountAndConstantSalt(data, iters, count_salt, constant_salt);   //use the count and constant salt
    case 5: //Quadratic count salt
        count_salt = toLong(datablock.popFirstBytes(8).value());    //get the count salt (start number)
        a = toLong(datablock.popFirstBytes(8).value());             //get the a number 
        b = toLong(datablock.popFirstBytes(8).value());             //get the b number 
        c = toLong(datablock.popFirstBytes(8).value());             //get the c number 
        return pwf.chainhashWithQuadraticCountSalt(data, iters, count_salt, a, b, c);   //use the count salt and a,b,c
    default:    //invalid chainhash mode
        throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::performChainHash(const unsigned char chainhash_mode, const unsigned long iters, Bytes datablock, const Hash* hash, const std::string data){
    //performs a chainhash on a string
    if(!ChainHashModes::isChainHashValid(chainhash_mode, iters, datablock)){
        throw std::invalid_argument("ChainHash arguments are not valid");   //chainhash is not valid
    }
    PwFunc pwf = PwFunc(hash);      //init the pwfunc object with the given hash function
    std::string constant_salt{};    //init all variables we might need, because in the switch statement no variables can be declared
    unsigned long count_salt{};
    unsigned long a{};
    unsigned long b{};
    unsigned long c{};
    switch (chainhash_mode){
    case 1: //normal chainhash
        return pwf.chainhash(data, iters);      //just use the iterations
    case 2: //constant salt
        constant_salt = charVecToString(datablock.getBytes());      //get the salt from the datablock
        return pwf.chainhashWithConstantSalt(data, iters, constant_salt);   //use the iterations and the constant salt
    case 3: //count salt
        count_salt = toLong(datablock.popFirstBytes(8).value());    //get the start number of the salt
        return pwf.chainhashWithCountSalt(data, iters, count_salt); //use the iterations and the count salt
    case 4: //constant + count salt
        count_salt = toLong(datablock.popFirstBytes(8).value());    //get the start number of the salt
        constant_salt = charVecToString(datablock.getBytes());      //get the salt from the datablock
        return pwf.chainhashWithCountAndConstantSalt(data, iters, count_salt, constant_salt);   //use the count and constant salt
    case 5: //Quadratic count salt
        count_salt = toLong(datablock.popFirstBytes(8).value());    //get the start number of the count salt
        a = toLong(datablock.popFirstBytes(8).value());             //get the a number 
        b = toLong(datablock.popFirstBytes(8).value());             //get the b number 
        c = toLong(datablock.popFirstBytes(8).value());             //get the c number 
        return pwf.chainhashWithQuadraticCountSalt(data, iters, count_salt, a, b, c);   //use the count salt and a,b,c
    default:    //invalid chainhash mode
        throw std::invalid_argument("chainhash mode does not exist");
    }
}
