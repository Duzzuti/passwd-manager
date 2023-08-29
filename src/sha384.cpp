#include "sha384.h"

Bytes sha384::hash(const Bytes& bytes, const u_int32_t extra_space) const {
    Bytes ret(this->getHashSize() + extra_space);              // output buffer with hashsize length and extra_space
    SHA384(bytes.getBytes(), bytes.getLen(), ret.getBytes());  // performs the hash
    ret.setLen(this->getHashSize());                           // sets the length of the output buffer to the hashsize
    return ret;
}

Bytes sha384::hash(const std::string& str, const u_int32_t extra_space) const {
    const unsigned char* bytesin = reinterpret_cast<const unsigned char*>(str.c_str());  // input buffer with length of the input
    Bytes ret(this->getHashSize() + extra_space);              // output buffer with hashsize length and extra_space
    SHA384(bytesin, str.length(), ret.getBytes());                                       // performs the hash
    ret.setLen(this->getHashSize());                                                     // sets the length of the output buffer to the hashsize
    return ret;
}
