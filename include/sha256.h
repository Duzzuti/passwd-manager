#pragma once

#include <openssl/sha.h>

#include "hash.h"

class sha256 : public Hash {
    /*
    this class is a hash function that implements the abstract class Hash
    it uses the openssl library to perform a sha256
    */
   public:
    int getHashSize() const noexcept override { return SHA256_DIGEST_LENGTH; }  // returns the length of the sha256 (32 byte)
    Bytes hash(const Bytes& bytes) const override;                              // performs the sha256 on a Bytes object
    Bytes hash(const Bytes& bytes) const override;                        // performs the sha256 on a Bytes object
    Bytes hash(const std::string& str) const override;                          // performs the sha256 on a string
};
