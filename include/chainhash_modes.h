#pragma once

#include "base.h"
#include "chainhash_data.h"
#include "error.h"
#include "hash.h"
#include "settings.h"

// ChainHash struct holds all components that are needed to describe a chainhash
struct ChainHash {
    CHModes mode = CHModes(0);                                         // the mode
    u_int64_t iters;                                                   // the iters
    ChainHashData datablock{Format{CHModes(STANDARD_CHAINHASHMODE)}};  // the data corresponding to the chainhash
};

// ChainHashTimed struct holds all components that are needed to describe a chainhash
// this chainhash does not contain iterations but a runtime to get the iterations
struct ChainHashTimed {
    CHModes mode = CHModes(0);                                         // the mode
    u_int64_t run_time;                                                // the run time for this chainhash
    ChainHashData datablock{Format{CHModes(STANDARD_CHAINHASHMODE)}};  // the data corresponding to the chainhash
};

// ChainHashResult struct holds the actual chainhash that was used and the result of the chainhash
// this is returned if a timed chainhash was performed
struct ChainHashResult {
    ChainHash chainhash;  // the actual chainhash
    Bytes result;         // the result of the chainhash
};

class ChainHashModes {
    /*
    this class provides static methods all around chainhashes
    you can perform a chainhash or check if a chainhash mode is valid etc.
    A chainhash is a hash function that is applied multiple times
    Each chainhash mode has an own way of applying the hash function
    We can add some salt before we hash again and this salt is calculated differently in each mode
    */
   public:
    static std::string getInfo(const CHModes chainhash_mode);  // gets a string that conatins information about this chainhash mode
    // ask the user for a string with a given message and max length and returns the bytes of the string
    static Bytes askForSaltString(const std::string msg, const unsigned char max_len);
    // ask the user for a number with a given message and max byte len (8 for ul) and returns the bytes of the number
    static Bytes askForSaltNumber(const std::string msg, const unsigned char max_len = 8);
    static bool isModeValid(const CHModes chainhash_mode) noexcept;  // checks if the given chain hash mode is valid
    // checks if the given chainhash is valid (with the iterations and datablock which contains data that is used by the chainhash)
    static ErrorStruct<bool> isChainHashValid(const ChainHash chainh) noexcept;
    static ChainHashData askForData(const CHModes chainhash_mode);  // gets data from the user that is needed for this chainhash mode
    // two methods for actually performing the chainhash, one for Bytes input and one for string input
    // expensive methods, you can set an timeout (in ms). 0 means no timeout.
    static ErrorStruct<Bytes> performChainHash(const ChainHash chainh, const Hash* hash, const Bytes data, const u_int64_t timeout = 0);
    static ErrorStruct<Bytes> performChainHash(const ChainHash chainh, const Hash* hash, const std::string data, const u_int64_t timeout = 0);
    // two other methods for actually performing the chainhash, one for Bytes input and one for string input
    // these methods use a runtime instead of iterations and are returning a ChainHash
    static ErrorStruct<ChainHashResult> performChainHash(const ChainHashTimed chainh, const Hash* hash, const Bytes data);
    static ErrorStruct<ChainHashResult> performChainHash(const ChainHashTimed chainh, const Hash* hash, const std::string data);
};
