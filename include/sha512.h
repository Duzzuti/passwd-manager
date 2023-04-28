#pragma once
#ifndef SHA512_H
#define SHA512_H
#include <openssl/sha.h>
#include "hash.h"

class sha512 : public Hash{
public:
    int getHashSize() const noexcept{return SHA512_DIGEST_LENGTH;}
    Bytes hash(const Bytes bytes) const;
    Bytes hash(const std::string str) const;
};

#endif //SHA512_H