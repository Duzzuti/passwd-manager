#pragma once

#include "bytes.h"

class Hash {
    /*
    abstract hash class
    if you wanna add a new hash function it has to implement this class in order to work with the other classes
    */
   public:
    Hash() = default;                                      // it needs a default constructor
    virtual int getHashSize() const noexcept = 0;          // a getter for the byte len of the hash
    // extra_space is adding more space to the returned Bytes object
    virtual Bytes hash(const Bytes& bytes, const u_int32_t extra_space = 0) const = 0;      // a hash function that takes a Bytes object
    virtual Bytes hash(const std::string& str, const u_int32_t extra_space = 0) const = 0;  // a second hash function that takes a string
    virtual ~Hash(){};
};
