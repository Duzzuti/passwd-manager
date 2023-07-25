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

ErrorStruct<bool> ChainHashModes::isChainHashValid(const ChainHash chainh) noexcept {
    // checks if the chainhash is valid
    ErrorStruct<bool> es;
    if (!ChainHashModes::isModeValid(chainh.mode)) {
        PLOG_WARNING << "chainhash mode is not valid (" << +chainh.mode << ")";
        es.errorCode = ERR_CHAINHASH_MODE_INVALID;
        es.success = FAIL;
        return es;  // chainhash mode is not valid
    }
    if (!(chainh.iters > 0 && chainh.iters <= MAX_ITERATIONS)) {
        PLOG_WARNING << "chainhash iterations are not valid (" << chainh.iters << ")";
        es.errorCode = ERR_ITERATIONS_INVALID;
        es.success = FAIL;
        return es;  // iteration number is not valid
    }
    if (!chainh.datablock.isCompletedFormat(Format(chainh.mode))) {
        PLOG_WARNING << "chainhash datablock is not completed or has the wrong format";
        // checks if the datablock is already completed
        es.errorCode = ERR_DATABLOCK_NOT_COMPLETED;
        es.success = FAIL;
        return es;
    }
    if (chainh.datablock.getLen() > 255) {
        PLOG_WARNING << "chainhash datablock is too long (" << chainh.datablock.getLen() << ")";
        es.errorCode = ERR_DATABLOCK_TOO_LONG;
        es.success = FAIL;
        return es;  // datablock is too long
    }
    es.success = SUCCESS;
    return es;
}

ErrorStruct<Bytes> ChainHashModes::performChainHash(const ChainHash chainh, std::shared_ptr<Hash> hash, const Bytes data, const u_int64_t timeout) {
    // performs a chainhash on bytes
    PLOG_VERBOSE << "performing chainhash (mode: " << +chainh.mode << ", iterations: " << chainh.iters << ", timeout " << timeout << ")";
    ErrorStruct err = ChainHashModes::isChainHashValid(chainh);  // check if the chainhash is valid
    if (!err.isSuccess()) {
        PLOG_ERROR << "chainhash is not valid cannot perform chainhash";
        throw std::invalid_argument(getErrorMessage(err));  // chainhash is not valid
    }
    PwFunc pwf = PwFunc(std::move(hash));  // init the pwfunc object with the given hash function
    std::string constant_salt{};           // init all variables we might need, because in the switch statement no variables can be declared
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
            PLOG_FATAL << "invalid chainhash mode provided (" << +chainh.mode << ")";
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

ErrorStruct<Bytes> ChainHashModes::performChainHash(const ChainHash chainh, std::shared_ptr<Hash> hash, const std::string data, const u_int64_t timeout) {
    // performs a chainhash on a string
    PLOG_VERBOSE << "performing chainhash (mode: " << +chainh.mode << ", iterations: " << chainh.iters << ", timeout " << timeout << ")";
    ErrorStruct err = ChainHashModes::isChainHashValid(chainh);  // check if the chainhash is valid
    if (!err.isSuccess()) {
        PLOG_ERROR << "chainhash is not valid cannot perform chainhash";
        throw std::invalid_argument(getErrorMessage(err));  // chainhash is not valid
    }
    PwFunc pwf = PwFunc(std::move(hash));  // init the pwfunc object with the given hash function
    std::string constant_salt{};           // init all variables we might need, because in the switch statement no variables can be declared
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
            PLOG_FATAL << "invalid chainhash mode provided (" << +chainh.mode << ")";
            throw std::invalid_argument("chainhash mode does not exist");
    }
}

ErrorStruct<ChainHashResult> ChainHashModes::performChainHash(const ChainHashTimed chainh, std::shared_ptr<Hash> hash, const Bytes data) {
    PLOG_VERBOSE << "performing chainhash timed (mode: " << +chainh.mode << ", runtime: " << chainh.run_time << ")";
    // WORK
    return ErrorStruct<ChainHashResult>();
}

ErrorStruct<ChainHashResult> ChainHashModes::performChainHash(const ChainHashTimed chainh, std::shared_ptr<Hash> hash, const std::string data) {
    PLOG_VERBOSE << "performing chainhash timed (mode: " << +chainh.mode << ", runtime: " << chainh.run_time << ")";
    // WORK
    return ErrorStruct<ChainHashResult>();
}
