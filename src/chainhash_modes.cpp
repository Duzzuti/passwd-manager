/*
contains the implementations for the chainhashModes class
*/
#include "chainhash_modes.h"

#include "chainhash_data.h"
#include "logger.h"
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
            PLOG_FATAL << "invalid chainhash mode provided (" << +chainhash_mode << ")";
            throw std::invalid_argument("chainhash mode does not exist");
    }
    return msg.str();
}

bool ChainHashModes::isModeValid(const CHModes chainhash_mode) noexcept {
    // checks whether the chainhash mode is in the range of valid modes
    return (1 <= chainhash_mode && chainhash_mode <= MAX_CHAINHASHMODE_NUMBER);
}

ErrorStruct<Bytes> ChainHashModes::performChainHash(const ChainHash chainh, std::shared_ptr<Hash> hash, const Bytes data, const u_int64_t timeout) {
    // performs a chainhash on bytes
    PLOG_VERBOSE << "performing chainhash (mode: " << +chainh.getMode() << ", iterations: " << chainh.getIters() << ", timeout " << timeout << ")";
    PwFunc pwf = PwFunc(std::move(hash));  // init the pwfunc object with the given hash function
    std::string constant_salt{};           // init all variables we might need, because in the switch statement no variables can be declared
    u_int64_t count_salt{};
    u_int64_t a{};
    u_int64_t b{};
    u_int64_t c{};
    switch (chainh.getMode()) {
        case CHAINHASH_NORMAL:                                                                                     // normal chainhash
            return pwf.chainhash(data, chainh.getIters(), timeout);                                                     // just use the iterations
        case CHAINHASH_CONSTANT_SALT:                                                                              // constant salt
            constant_salt = charVecToString(chainh.getChainHashData().getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithConstantSalt(data, chainh.getIters(), constant_salt, timeout);                      // use the iterations and the constant salt
        case CHAINHASH_COUNT_SALT:                                                                                 // count salt
            count_salt = toLong(chainh.getChainHashData().getPart("SN"));                                                   // get the start number of the salt
            return pwf.chainhashWithCountSalt(data, chainh.getIters(), count_salt, timeout);                            // use the iterations and the count salt
        case CHAINHASH_CONSTANT_COUNT_SALT:                                                                        // constant + count salt
            count_salt = toLong(chainh.getChainHashData().getPart("SN"));                                                   // get the start number of the salt
            constant_salt = charVecToString(chainh.getChainHashData().getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithCountAndConstantSalt(data, chainh.getIters(), count_salt, constant_salt, timeout);  // use the count and constant salt
        case CHAINHASH_QUADRATIC:                                                                                  // Quadratic count salt
            count_salt = toLong(chainh.getChainHashData().getPart("SN"));                                                   // get the count salt (start number)
            a = toLong(chainh.getChainHashData().getPart("A"));                                                             // get the a number
            b = toLong(chainh.getChainHashData().getPart("B"));                                                             // get the b number
            c = toLong(chainh.getChainHashData().getPart("C"));                                                             // get the c number
            return pwf.chainhashWithQuadraticCountSalt(data, chainh.getIters(), count_salt, a, b, c, timeout);          // use the count salt and a,b,c
        default:                                                                                                   // invalid chainhash mode
            PLOG_FATAL << "invalid chainhash mode provided (" << +chainh.getMode() << ")";
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

ErrorStruct<Bytes> ChainHashModes::performChainHash(const ChainHash chainh, std::shared_ptr<Hash> hash, const std::string data, const u_int64_t timeout) {
    // performs a chainhash on a string
    PLOG_VERBOSE << "performing chainhash (mode: " << +chainh.getMode() << ", iterations: " << chainh.getIters() << ", timeout " << timeout << ")";
    PwFunc pwf = PwFunc(std::move(hash));  // init the pwfunc object with the given hash function
    std::string constant_salt{};           // init all variables we might need, because in the switch statement no variables can be declared
    u_int64_t count_salt{};
    u_int64_t a{};
    u_int64_t b{};
    u_int64_t c{};
    switch (chainh.getMode()) {
        case CHAINHASH_NORMAL:                                                                                     // normal chainhash
            return pwf.chainhash(data, chainh.getIters(), timeout);                                                     // just use the iterations
        case CHAINHASH_CONSTANT_SALT:                                                                              // constant salt
            constant_salt = charVecToString(chainh.getChainHashData().getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithConstantSalt(data, chainh.getIters(), constant_salt, timeout);                      // use the iterations and the constant salt
        case CHAINHASH_COUNT_SALT:                                                                                 // count salt
            count_salt = toLong(chainh.getChainHashData().getPart("SN"));                                                   // get the start number of the salt
            return pwf.chainhashWithCountSalt(data, chainh.getIters(), count_salt, timeout);                            // use the iterations and the count salt
        case CHAINHASH_CONSTANT_COUNT_SALT:                                                                        // constant + count salt
            count_salt = toLong(chainh.getChainHashData().getPart("SN"));                                                   // get the start number of the salt
            constant_salt = charVecToString(chainh.getChainHashData().getPart("S").getBytes());                             // get the salt from the datablock
            return pwf.chainhashWithCountAndConstantSalt(data, chainh.getIters(), count_salt, constant_salt, timeout);  // use the count and constant salt
        case CHAINHASH_QUADRATIC:                                                                                  // Quadratic count salt
            count_salt = toLong(chainh.getChainHashData().getPart("SN"));                                                   // get the start number of the count salt
            a = toLong(chainh.getChainHashData().getPart("A"));                                                             // get the a number
            b = toLong(chainh.getChainHashData().getPart("B"));                                                             // get the b number
            c = toLong(chainh.getChainHashData().getPart("C"));                                                             // get the c number
            return pwf.chainhashWithQuadraticCountSalt(data, chainh.getIters(), count_salt, a, b, c, timeout);          // use the count salt and a,b,c
        default:                                                                                                   // invalid chainhash mode
            PLOG_FATAL << "invalid chainhash mode provided (" << +chainh.getMode() << ")";
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

ErrorStruct<ChainHashResult> ChainHashModes::performChainHash(const ChainHashTimed chainh, std::shared_ptr<Hash> hash, const Bytes data) {
    PLOG_VERBOSE << "performing chainhash timed (mode: " << +chainh.getMode() << ", runtime: " << chainh.getRunTime() << ")";
    // WORK
    return ErrorStruct<ChainHashResult>();
}

ErrorStruct<ChainHashResult> ChainHashModes::performChainHash(const ChainHashTimed chainh, std::shared_ptr<Hash> hash, const std::string data) {
    PLOG_VERBOSE << "performing chainhash timed (mode: " << +chainh.getMode() << ", runtime: " << chainh.getRunTime() << ")";
    // WORK
    return ErrorStruct<ChainHashResult>();
}
