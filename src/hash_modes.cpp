/*
this file contains implementations of HashModes class
*/
#include "hash_modes.h"

#include "logger.h"
#include "settings.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"

bool HashModes::isModeValid(const HModes& hash_mode) noexcept {
    // checks whether the given hash mode is in the accepted range
    return (1 <= hash_mode && hash_mode <= MAX_HASHMODE_NUMBER);
}

std::unique_ptr<Hash> HashModes::getHash(const HModes& hash_mode) {
    // gets the concrete hash sub class
    PLOG_VERBOSE << "getting hash function for hash mode: " << +hash_mode;
    switch (hash_mode) {
        case HASHMODE_SHA256:  // sha256
            return std::make_unique<sha256>();
        case HASHMODE_SHA384:  // sha384
            return std::make_unique<sha384>();
        case HASHMODE_SHA512:  // sha512
            return std::make_unique<sha512>();
        default:  // hash mode is out of range
            PLOG_ERROR << "invalid hash mode passed to getHash (hash mode: " << +hash_mode << ")";
            throw std::invalid_argument("hash mode does not exist");
    }
}

std::string HashModes::getInfo(const HModes& hash_mode, const bool only_name) {
    // gets some information about this hash mode (hash function)
    std::stringstream msg{};
    // start with a base string
    if (!only_name) msg << "The hash mode " << +hash_mode << " corresponds to hash function: ";
    // add the information based on the mode
    switch (hash_mode) {
        case HASHMODE_SHA256:  // sha256
            msg << "SHA256";
            break;
        case HASHMODE_SHA384:  // sha384
            msg << "SHA384";
            break;
        case HASHMODE_SHA512:  // sha512
            msg << "SHA512";
            break;
        default:  // hash mode is out of range
            PLOG_ERROR << "invalid hash mode passed to getInfo (hash mode: " << +hash_mode << ")";
            throw std::invalid_argument("hash mode does not exist");
    }
    return msg.str();
}
