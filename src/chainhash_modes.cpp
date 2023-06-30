/*
contains the implementations for the chainhashModes class
*/
#include "chainhash_modes.h"

#include "app.h"
#include "chainhash_data.h"
#include "pwfunc.h"
#include "rng.h"
#include "settings.h"
#include "utility.h"

std::string ChainHashModes::getInfo(const CHModes chainhash_mode) {
    // gets some information about the given chainhash mode
    std::stringstream msg{};
    msg << "The chainhash mode " << +chainhash_mode << " corresponds to a chainhash ";
    switch (chainhash_mode) {
        case CHAINHASH_NORMAL:  // normal chainhash
            msg << "that hashes the old hash again";
            break;
        case CHAINHASH_CONSTANT_SALT:  // constant salt
            msg << "that hashes the old hash + a constant salt again";
            break;
        case CHAINHASH_COUNT_SALT:  // count salt
            msg << "that hashes the old hash + an incrementing salt again";
            break;
        case CHAINHASH_CONSTANT_COUNT_SALT:  // count salt and constant salt
            msg << "that hashes the old hash + a constant AND an incrementing salt again";
            break;
        case CHAINHASH_QUADRATIC:  // quadratic salt
            msg << "that hashes the old hash + quadratic value derived from an incrementing salt and 3 coefficients";
            break;
        default:  // invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
    return msg.str();
}

Bytes ChainHashModes::askForSaltString(const std::string msg, const unsigned char max_len) {
    // asks the user for a string that is used as a salt and returns the string bytes
    if (max_len < 1) {
        // if max len is zero it means that this function is useless, throw an error to prohibit bugs
        throw std::runtime_error("maximum length of the string that should be got from the user does not make sense");
    }
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do {
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);        // gets the user input
    } while (inp.length() > max_len);  // checks if the user input is not too long
    Bytes bytes = Bytes();
    if (inp.empty()) {                                   // user wants to generate it randomly if nothing was entered
        bytes.setBytes(RNG::get_random_bytes(max_len));  // generate random bytes
        return bytes;
    }
    // loops over the string and add each char as a byte to the Bytes object
    for (size_t i = 0; i < inp.length(); i++) {
        bytes.addByte(inp[i]);  // implicit char -> unsigned char
    }
    return bytes;
}

Bytes ChainHashModes::askForSaltNumber(const std::string msg, const unsigned char max_len) {
    // asks the user for a number that is used as a salt and return the number bytes
    if (max_len > 8 || max_len < 1) {
        // if max len is zero or greater than 8 (u_int64_t) it means that this function is useless, throw an error to prohibit bugs
        throw std::runtime_error("maximum length of the number from the user does not make sense");
    }
    std::string blank = " or leave this field blank to generate it randomly: ";
    std::string inp{};
    do {
        std::cout << msg << " (you do not have to remember this, maximum length = " << +max_len << " Bytes)" << blank;
        getline(std::cin, inp);                // gets the user input
    } while (!App::isValidNumber(inp, true));  // stops if the entered string is a valid number
    Bytes bytes = Bytes();
    if (inp.empty()) {                                   // user wants to generate it randomly
        bytes.setBytes(RNG::get_random_bytes(max_len));  // generate Bytes to represent a random number
        return bytes;
    }
    u_int64_t inp_int = std::stoul(inp);
    bytes.setBytes(LongToCharVec(inp_int));  // convert the long to a bytes object
    return bytes;
}

bool ChainHashModes::isModeValid(const CHModes chainhash_mode) noexcept {
    // checks whether the chainhash mode is in the range of valid modes
    return (1 <= chainhash_mode && chainhash_mode <= MAX_CHAINHASHMODE_NUMBER);
}

ErrorStruct<bool> ChainHashModes::isChainHashValid(const ChainHash chainh) noexcept {
    // checks if the chainhash is valid
    ErrorStruct<bool> es;
    if (!ChainHashModes::isModeValid(chainh.mode)) {
        es.errorCode = ERR_CHAINHASH_MODE_INVALID;
        es.success = FAIL;
        return es;  // chainhash mode is not valid
    }
    if (!(chainh.iters > 0 && chainh.iters <= MAX_ITERATIONS)) {
        es.errorCode = ERR_ITERATIONS_INVALID;
        es.success = FAIL;
        return es;  // iteration number is not valid
    }
    if (!chainh.datablock.isCompletedFormat(Format(chainh.mode))) {
        // checks if the datablock is already completed
        es.errorCode = ERR_DATABLOCK_NOT_COMPLETED;
        es.success = FAIL;
        return es;
    }
    if (chainh.datablock.getLen() > 255) {
        es.errorCode = ERR_DATABLOCK_TOO_LONG;
        es.success = FAIL;
        return es;  // datablock is too long
    }
    es.success = SUCCESS;
    return es;
}

ChainHashData ChainHashModes::askForData(const CHModes chainhash_mode) {
    // gets data from the user based on the mode
    ChainHashData chd{Format(chainhash_mode)};
    switch (chainhash_mode) {
        case CHAINHASH_NORMAL:  // normal chainhash (no data needed)
            break;
        case CHAINHASH_CONSTANT_SALT:  // constant salt, we need a salt string
            chd.addBytes(ChainHashModes::askForSaltString("Please enter the salt that should be mixed into the chainhash", 255));
            break;
        case CHAINHASH_COUNT_SALT:  // count salt, we need a start number for the count salt
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the start number for the count salt"));
            break;
        case CHAINHASH_CONSTANT_COUNT_SALT:  // count salt and constant salt, we need a start number for the count salt and a salt string
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the start number for the count salt"));
            chd.addBytes(ChainHashModes::askForSaltString("Please enter the salt that should be mixed into the chainhash", 247));
            break;
        case CHAINHASH_QUADRATIC:  // quadratic salt, we need the start salt number and 3 numbers for a,b,c (a*a*salt + b*salt + c)
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the start number for the count salt"));
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the value of quadratic part a"));
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the value of linear part b"));
            chd.addBytes(ChainHashModes::askForSaltNumber("Please enter the value of constant part c"));
            break;
        default:  // invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
    return chd;
}

ErrorStruct<Bytes> ChainHashModes::performChainHash(const ChainHash chainh, const Hash* hash, const Bytes data, const u_int64_t timeout) {
    // performs a chainhash on bytes
    ErrorStruct err = ChainHashModes::isChainHashValid(chainh);  // check if the chainhash is valid
    if (err.success != SUCCESS) {
        throw std::invalid_argument(getErrorMessage(err));  // chainhash is not valid
    }
    PwFunc pwf = PwFunc(hash);    // init the pwfunc object with the given hash function
    std::string constant_salt{};  // init all variables we might need, because in the switch statement no variables can be declared
    u_int64_t count_salt{};
    u_int64_t a{};
    u_int64_t b{};
    u_int64_t c{};
    switch (chainh.mode) {
        case CHAINHASH_NORMAL:                                                                                     // normal chainhash
            return pwf.chainhash(data, chainh.iters, timeout);                                                     // just use the iterations
        case CHAINHASH_CONSTANT_SALT:                                                                              // constant salt
            constant_salt = charVecToString(chainh.datablock.getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithConstantSalt(data, chainh.iters, constant_salt, timeout);                      // use the iterations and the constant salt
        case CHAINHASH_COUNT_SALT:                                                                                 // count salt
            count_salt = toLong(chainh.datablock.getPart("SN"));                                                   // get the start number of the salt
            return pwf.chainhashWithCountSalt(data, chainh.iters, count_salt, timeout);                            // use the iterations and the count salt
        case CHAINHASH_CONSTANT_COUNT_SALT:                                                                        // constant + count salt
            count_salt = toLong(chainh.datablock.getPart("SN"));                                                   // get the start number of the salt
            constant_salt = charVecToString(chainh.datablock.getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithCountAndConstantSalt(data, chainh.iters, count_salt, constant_salt, timeout);  // use the count and constant salt
        case CHAINHASH_QUADRATIC:                                                                                  // Quadratic count salt
            count_salt = toLong(chainh.datablock.getPart("SN"));                                                   // get the count salt (start number)
            a = toLong(chainh.datablock.getPart("A"));                                                             // get the a number
            b = toLong(chainh.datablock.getPart("B"));                                                             // get the b number
            c = toLong(chainh.datablock.getPart("C"));                                                             // get the c number
            return pwf.chainhashWithQuadraticCountSalt(data, chainh.iters, count_salt, a, b, c, timeout);          // use the count salt and a,b,c
        default:                                                                                                   // invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

ErrorStruct<Bytes> ChainHashModes::performChainHash(const ChainHash chainh, const Hash* hash, const std::string data, const u_int64_t timeout) {
    // performs a chainhash on a string
    ErrorStruct err = ChainHashModes::isChainHashValid(chainh);  // check if the chainhash is valid
    if (err.success != SUCCESS) {
        throw std::invalid_argument(getErrorMessage(err));  // chainhash is not valid
    }
    PwFunc pwf = PwFunc(hash);    // init the pwfunc object with the given hash function
    std::string constant_salt{};  // init all variables we might need, because in the switch statement no variables can be declared
    u_int64_t count_salt{};
    u_int64_t a{};
    u_int64_t b{};
    u_int64_t c{};
    switch (chainh.mode) {
        case CHAINHASH_NORMAL:                                                                                     // normal chainhash
            return pwf.chainhash(data, chainh.iters, timeout);                                                     // just use the iterations
        case CHAINHASH_CONSTANT_SALT:                                                                              // constant salt
            constant_salt = charVecToString(chainh.datablock.getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithConstantSalt(data, chainh.iters, constant_salt, timeout);                      // use the iterations and the constant salt
        case CHAINHASH_COUNT_SALT:                                                                                 // count salt
            count_salt = toLong(chainh.datablock.getPart("SN"));                                                   // get the start number of the salt
            return pwf.chainhashWithCountSalt(data, chainh.iters, count_salt, timeout);                            // use the iterations and the count salt
        case CHAINHASH_CONSTANT_COUNT_SALT:                                                                        // constant + count salt
            count_salt = toLong(chainh.datablock.getPart("SN"));                                                   // get the start number of the salt
            constant_salt = charVecToString(chainh.datablock.getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithCountAndConstantSalt(data, chainh.iters, count_salt, constant_salt, timeout);  // use the count and constant salt
        case CHAINHASH_QUADRATIC:                                                                                  // Quadratic count salt
            count_salt = toLong(chainh.datablock.getPart("SN"));                                                   // get the start number of the count salt
            a = toLong(chainh.datablock.getPart("A"));                                                             // get the a number
            b = toLong(chainh.datablock.getPart("B"));                                                             // get the b number
            c = toLong(chainh.datablock.getPart("C"));                                                             // get the c number
            return pwf.chainhashWithQuadraticCountSalt(data, chainh.iters, count_salt, a, b, c, timeout);          // use the count salt and a,b,c
        default:                                                                                                   // invalid chainhash mode
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

ErrorStruct<ChainHashResult> ChainHashModes::performChainHash(const ChainHashTimed chainh, const Hash* hash, const Bytes data) { 
    // WORK
    return ErrorStruct<ChainHashResult>();
}

ErrorStruct<ChainHashResult> ChainHashModes::performChainHash(const ChainHashTimed chainh, const Hash* hash, const std::string data) { 
    // WORK
    return ErrorStruct<ChainHashResult>();
}
