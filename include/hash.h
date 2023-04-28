#pragma once
#ifndef HASH_H
#define HASH_H
#include "bytes.h"
class Hash{
public:
    Hash() = default;
    virtual int getHashSize() const noexcept = 0;
    virtual Bytes hash(const Bytes bytes) const = 0;
    virtual Bytes hash(const std::string str) const = 0;
    virtual ~Hash() {};
};

#endif //HASH_H