#include "base_attacker.h"
#include "chainhash_modes.h"
#include "hash_modes.h"

class BrutePwAttacker : public BaseAttacker {
    // attacker that is trying every possible password combination
    AttackerReturn attack(DataHeaderParts dhp, Bytes data, std::string decrypted_content) const noexcept override {
        AttackerReturn ret;                              // return struct
        u_int64_t tries = 1;                             // number of tries
        Hash* hash = HashModes::getHash(dhp.hash_mode);  // get the hash function used
        // tries the empty string first, hashes to a password hash
        Bytes ch1 = ChainHashModes::performChainHash(dhp.chainhash1_mode, dhp.chainhash1_iters, dhp.chainhash1_datablock, hash, "");
        // hashes that password hash to another hash (to check if the password is correct)
        Bytes ch2 = ChainHashModes::performChainHash(dhp.chainhash2_mode, dhp.chainhash2_iters, dhp.chainhash2_datablock, hash, ch1);
        if (ch2 == dhp.valid_passwordhash) {
            // passwordhash is correct
            delete hash;  // free that hash ptr
            // got a success with that passwordhash
            ret.success_type = SUCCESS;
            ret.progress = 0;
            ret.tries = tries;
            ret.passwordhash = ch2;
            return ret;
        }
        for (int l = 1;; l++) {
            // tries all possible combinations, with length l
            u_int64_t numCombinations = 1;  // Number of combinations at each length
            for (int i = 0; i < l; ++i) {
                // we got charset.length() ^ length  possibilities
                numCombinations *= this->CHAR_SET.length();
            }
            for (u_int64_t i = 0; i < numCombinations; ++i) {
                // loops for the number of combinations
                u_int64_t index = i;      // get the current combination number as an index
                std::string combination;  // next combination

                for (int j = 0; j < l; ++j) {
                    // calculate the combination with index i
                    combination += this->CHAR_SET[index % this->CHAR_SET.length()];
                    index /= this->CHAR_SET.length();
                }
                tries++;  // one more try needed
                // hashes the combination to a passwordhash
                Bytes ch1 = ChainHashModes::performChainHash(dhp.chainhash1_mode, dhp.chainhash1_iters, dhp.chainhash1_datablock, hash, combination);
                // hashes the passwordhash to a passwordhash validator hash
                Bytes ch2 = ChainHashModes::performChainHash(dhp.chainhash2_mode, dhp.chainhash2_iters, dhp.chainhash2_datablock, hash, ch1);
                if (ch2 == dhp.valid_passwordhash) {
                    // got the right password hash
                    delete hash;
                    ret.success_type = SUCCESS;
                    ret.progress = l;  // progress is the length of the combination
                    ret.tries = tries;
                    ret.passwordhash = ch2;
                    return ret;
                }
                // WORK check for timeout
                if (false) {
                    // attacker timeout (did not found the password in time)
                    ret.success_type = TIMEOUT;
                    ret.progress = l;
                    ret.tries = tries;
                    break;
                }
            }
            if (ret.success_type == TIMEOUT) {
                // cancel the infinite loop if a timeout got called
                break;
            }
        }
        delete hash;
        return ret;
    }
};