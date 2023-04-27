#pragma once
#ifndef SHA512_H
#define SHA512_H
#include <openssl/sha.h>
#include "hash.h"

class sha512 : Hash{
public:
    int getHashSize() const noexcept{return SHA512_DIGEST_LENGTH;}
    Bytes hash(Bytes bytes);
    Bytes hash(std::string str);
};

#endif //SHA512_H