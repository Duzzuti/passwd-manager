/*
contains the implementations for the chainhashModes class
*/
#include "chainhash_modes.h"
#include "rng.h"
#include "app.h"
#include "utility.h"
#include "pwfunc.h"
#include "chainhash_data.h"

std::string ChainHashModes::getInfo(const CHModes chainhash_mode){
    //gets some information about the given chainhash mode
    std::stringstream msg{}; 
    msg << "The chainhash mode " << +chainhash_mode << " corresponds to a chainhash ";
    switch(chainhash_mode){
        case CHAINHASH_NORMAL: //normal chainhash 
            msg << "that hashes the old hash again";
            break;
        case CHAINHASH_CONSTANT_SALT: //constant salt
            msg << "that hashes the old hash + a constant salt again";
            break;
        case CHAINHASH_COUNT_SALT: //count salt
            msg << "that hashes the old hash + an incrementing salt again";
            break;
        case CHAINHASH_CONSTANT_COUNT_SALT: //count salt and constant salt
            msg << "that hashes the old hash + a constant AND an incrementing salt again";
            break;
        case CHAINHASH_QUADRATIC: //quadratic salt
            msg << "that hashes the old hash + quadratic value derived from an incrementing salt and 3 coefficients";
            break;
        default: //invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
    return msg.str();
}

Bytes ChainHashModes::askForSaltString(const std::string msg, const unsigned char max_len){
    //asks the user for a string that is used as a salt and returns the string bytes
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
    Bytes bytes = Bytes();
    if(inp.empty()){    //user wants to generate it randomly if nothing was entered
        bytes.setBytes(RNG::get_random_bytes(max_len));  //generate random bytes
        return bytes;
    }
    //loops over the string and add each char as a byte to the Bytes object
    for(unsigned char i=0; i < inp.length(); i++){
        bytes.addByte(inp[i]);    //implicit char -> unsigned char
    }
    return bytes;
}

Bytes ChainHashModes::askForSaltNumber(const std::string msg, const unsigned char max_len){
    //asks the user for a number that is used as a salt and return the number bytes
    if(max_len > 8 || max_len < 1){
        //if max len is zero or greater than 8 (u_int64_t) it means that this function is useless, throw an error to prohibit bugs
        throw std::runtime_error("maximum length of the number from the user does not make sense");
    }
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do{
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);     //gets the user input
    }while(!App::isValidNumber(inp, true)); //stops if the entered string is a valid number
    Bytes bytes = Bytes();
    if(inp.empty()){    //user wants to generate it randomly
        bytes.setBytes(RNG::get_random_bytes(max_len));    //generate Bytes to represent a random number
        return bytes;
    }
    u_int64_t inp_int = std::stoul(inp);
    bytes.setBytes(LongToCharVec(inp_int)); //convert the long to a bytes object
    return bytes;
}

bool ChainHashModes::isModeValid(const unsigned char chainhash_mode) noexcept{
    //checks whether the chainhash mode is in the range of valid modes
    return (1 <= chainhash_mode && chainhash_mode <= MAX_CHAINHASHMODE_NUMBER);
}

bool ChainHashModes::isChainHashValid(const CHModes chainhash_mode, const u_int64_t iters, const ChainHashData datablock) noexcept{
    //checks if the chainhash is valid
    if(!(iters > 0 && iters <= MAX_ITERATIONS)){
        return false;   //iteration number is not valid
    }
    if(datablock.getDataBlock().getLen() > 255){
        return false;   //datablock is too long
    }
    switch (chainhash_mode){
    case CHAINHASH_NORMAL: //no datablock needed
        return true;
    case CHAINHASH_CONSTANT_SALT: //datablock is constant salt (can be everything)
        return true;
    case CHAINHASH_COUNT_SALT: //count salt begin needed (8 Bytes)
        if(datablock.isCompletedFormat("8B SN")){
            return false;   //datablock has an invalid format
        }
        return true;
    case CHAINHASH_CONSTANT_COUNT_SALT: //count salt begin needed (8 Bytes + constant salt Bytes)
        if(datablock.isCompletedFormat("8B")){
            return false;   //datablock too short
        }
        return true;
    case CHAINHASH_QUADRATIC: //count salt begin needed (8 Bytes for quadratic count salt, and 3*8 Bytes for a,b,c)
        if(datablock.getDataBlock().getLen() != 4*8){
        return false;   //datablock has an invalid length
        }
        return true;
    default:
        return false;  //chainhash mode not valid
    }
}

ChainHashData ChainHashModes::askForData(const CHModes chainhash_mode){
    //gets data from the user based on the mode
    ChainHashData chd{""};
    switch(chainhash_mode){
        case CHAINHASH_NORMAL: //normal chainhash (no data needed)
            break;
        case CHAINHASH_CONSTANT_SALT: //constant salt, we need a salt string
            chd = ChainHashData("*B S");
            chd.addBytes(ChainHashModes::askForSaltString("Please enter the salt that should be mixed into the chainhash", 255));
            break;
        case CHAINHASH_COUNT_SALT: //count salt, we need a start number for the count salt
            chd = ChainHashData("8B SN");
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the start number for the count salt"));
            break;
        case CHAINHASH_CONSTANT_COUNT_SALT: //count salt and constant salt, we need a start number for the count salt and a salt string 
            chd = ChainHashData("8B SN *B S");
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the start number for the count salt"));
            chd.addBytes(ChainHashModes::askForSaltString("Please enter the salt that should be mixed into the chainhash", 247));
            break;
        case CHAINHASH_QUADRATIC: //quadratic salt, we need the start salt number and 3 numbers for a,b,c (a*a*salt + b*salt + c)
            chd = ChainHashData("8B SN 8B A 8B B 8B C");
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the start number for the count salt"));
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the value of quadratic part a"));
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the value of linear part b"));
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the value of constant part c"));
            break;
        default:    //invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
    return chd;
}

Bytes ChainHashModes::performChainHash(const CHModes chainhash_mode, const u_int64_t iters, ChainHashData datablock, const Hash* hash, const Bytes data){
    //performs a chainhash on bytes
    if(!ChainHashModes::isChainHashValid(chainhash_mode, iters, datablock)){
        throw std::invalid_argument("ChainHash arguments are not valid");   //chainhash is not valid
    }
    PwFunc pwf = PwFunc(hash);      //init the pwfunc object with the given hash function
    std::string constant_salt{};    //init all variables we might need, because in the switch statement no variables can be declared
    u_int64_t count_salt{};
    u_int64_t a{};
    u_int64_t b{};
    u_int64_t c{};
    switch (chainhash_mode){
    case CHAINHASH_NORMAL: //normal chainhash
        return pwf.chainhash(data, iters);  //just use the iterations
    case CHAINHASH_CONSTANT_SALT: //constant salt
        constant_salt = charVecToString(datablock.getPart("S").getBytes());  //get the salt from the datablock
        return pwf.chainhashWithConstantSalt(data, iters, constant_salt);   //use the iterations and the constant salt
    case CHAINHASH_COUNT_SALT: //count salt
        count_salt = toLong(datablock.getPart("SN"));    //get the start number of the salt
        return pwf.chainhashWithCountSalt(data, iters, count_salt); //use the iterations and the count salt
    case CHAINHASH_CONSTANT_COUNT_SALT: //constant + count salt
        count_salt = toLong(datablock.getPart("SN"));    //get the start number of the salt
        constant_salt = charVecToString(datablock.getPart("S").getBytes());      //get the salt from the datablock
        return pwf.chainhashWithCountAndConstantSalt(data, iters, count_salt, constant_salt);   //use the count and constant salt
    case CHAINHASH_QUADRATIC: //Quadratic count salt
        count_salt = toLong(datablock.getPart("SN"));    //get the count salt (start number)
        a = toLong(datablock.getPart("A"));             //get the a number 
        b = toLong(datablock.getPart("B"));             //get the b number 
        c = toLong(datablock.getPart("C"));             //get the c number 
        return pwf.chainhashWithQuadraticCountSalt(data, iters, count_salt, a, b, c);   //use the count salt and a,b,c
    default:    //invalid chainhash mode
        throw std::invalid_argument("chainhash mode does not exist");
    }
}

Bytes ChainHashModes::performChainHash(const CHModes chainhash_mode, const u_int64_t iters, ChainHashData datablock, const Hash* hash, const std::string data){
    //performs a chainhash on a string
    if(!ChainHashModes::isChainHashValid(chainhash_mode, iters, datablock)){
        throw std::invalid_argument("ChainHash arguments are not valid");   //chainhash is not valid
    }
    PwFunc pwf = PwFunc(hash);      //init the pwfunc object with the given hash function
    std::string constant_salt{};    //init all variables we might need, because in the switch statement no variables can be declared
    u_int64_t count_salt{};
    u_int64_t a{};
    u_int64_t b{};
    u_int64_t c{};
    switch (chainhash_mode){
    case CHAINHASH_NORMAL: //normal chainhash
        return pwf.chainhash(data, iters);      //just use the iterations
    case CHAINHASH_CONSTANT_SALT: //constant salt
        constant_salt = charVecToString(datablock.getPart("S").getBytes());      //get the salt from the datablock
        return pwf.chainhashWithConstantSalt(data, iters, constant_salt);   //use the iterations and the constant salt
    case CHAINHASH_COUNT_SALT: //count salt
        count_salt = toLong(datablock.getPart("SN"));    //get the start number of the salt
        return pwf.chainhashWithCountSalt(data, iters, count_salt); //use the iterations and the count salt
    case CHAINHASH_CONSTANT_COUNT_SALT: //constant + count salt
        count_salt = toLong(datablock.getPart("SN"));    //get the start number of the salt
        constant_salt = charVecToString(datablock.getPart("S").getBytes());      //get the salt from the datablock
        return pwf.chainhashWithCountAndConstantSalt(data, iters, count_salt, constant_salt);   //use the count and constant salt
    case CHAINHASH_QUADRATIC: //Quadratic count salt
        count_salt = toLong(datablock.getPart("SN"));    //get the start number of the count salt
        a = toLong(datablock.getPart("A"));             //get the a number 
        b = toLong(datablock.getPart("B"));             //get the b number 
        c = toLong(datablock.getPart("C"));             //get the c number 
        return pwf.chainhashWithQuadraticCountSalt(data, iters, count_salt, a, b, c);   //use the count salt and a,b,c
    default:    //invalid chainhash mode
        throw std::invalid_argument("chainhash mode does not exist");
    }
}
