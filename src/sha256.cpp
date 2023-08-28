#include "sha256.h"

Bytes sha256::hash(const Bytes& bytes) const {
    Bytes ret(this->getHashSize());         // output buffer with hashsize length
    SHA256(bytes.getBytes(), bytes.getLen(), ret.getBytes());  // performs the hash
    ret.setLen(this->getHashSize());         // sets the length of the output buffer to the hashsize
    return ret;
}

Bytes sha256::hash(const std::string& str) const {
    const unsigned char* bytesin = reinterpret_cast<const unsigned char*>(str.c_str());  // input buffer with length of the input
    Bytes ret(this->getHashSize());                                         // output buffer with hashsize length
    SHA256(bytesin, str.length(), ret.getBytes());                                             // performs the hash
    ret.setLen(this->getHashSize());                                         // sets the length of the output buffer to the hashsize
    return ret;
}
