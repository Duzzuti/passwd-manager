#include "sha512.h"

#include <openssl/sha.h>

int sha512::getHashSize() const noexcept { return SHA512_DIGEST_LENGTH; }

Bytes sha512::hash(const Bytes& bytes, const u_int32_t extra_space) const {
    Bytes ret(this->getHashSize() + extra_space);              // output buffer with hashsize length and extra_space
    SHA512(bytes.getBytes(), bytes.getLen(), ret.getBytes());  // performs the hash
    ret.setLen(this->getHashSize());                           // sets the length of the output buffer to the hashsize
    return ret;
}

Bytes sha512::hash(const std::string& str, const u_int32_t extra_space) const {
    const unsigned char* bytesin = reinterpret_cast<const unsigned char*>(str.c_str());  // input buffer with length of the input
    Bytes ret(this->getHashSize() + extra_space);                                        // output buffer with hashsize length and extra_space
    SHA512(bytesin, str.length(), ret.getBytes());                                       // performs the hash
    ret.setLen(this->getHashSize());                                                     // sets the length of the output buffer to the hashsize
    return ret;
}
