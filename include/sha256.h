#pragma once
#ifndef SHA256_H
#define SHA256_H
#include <openssl/sha.h>
#include "hash.h"

class sha256 : public Hash{
public:
    int getHashSize() const noexcept{return SHA256_DIGEST_LENGTH;}
    Bytes hash(const Bytes bytes) const;
    Bytes hash(const std::string str) const;
};

#endif //SHA256_H