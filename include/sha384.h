#pragma once
#ifndef SHA384_H
#define SHA384_H
#include <openssl/sha.h>
#include "hash.h"

class sha384 : public Hash{
public:
    int getHashSize() const noexcept{return SHA384_DIGEST_LENGTH;}
    Bytes hash(const Bytes bytes) const;
    Bytes hash(const std::string str) const;
};

#endif //SHA384_H