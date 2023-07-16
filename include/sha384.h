#pragma once

#include <openssl/sha.h>

#include "hash.h"

class sha384 : public Hash {
    /*
    this class is a hash function that implements the abstract class Hash
    it uses the openssl library to perform a sha384
    */
   public:
    int getHashSize() const noexcept override { return SHA384_DIGEST_LENGTH; }  // returns the length of the sha384 (48 byte)
    Bytes hash(const Bytes bytes) const override;                               // performs the sha384 on a Bytes object
    Bytes hash(const std::string str) const override;                           // performs the sha384 on a string
};
