#pragma once

#include <memory>
#include <sstream>

#include "base.h"
#include "hash.h"

class HashModes {
    /*
    static class for the hash modes.
    You can get infos about a hash mode or check if it is valid
    The important thing is that you can get the corresponding Hash class for that hash mode
    */
   public:
    // checks if the hash mode exists (has to be in a defined range)
    static bool isModeValid(const HModes& hash_mode) noexcept;
    // gets the hash sub class (e.g sha256) to perform a hash (strategy pattern)
    static std::unique_ptr<Hash> getHash(const HModes& hash_mode);
    // gets some information about that hash mode (info about the hash function)
    static std::string getInfo(const HModes& hash_mode, const bool only_name);
};
