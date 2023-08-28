#include "sha256.h"

Bytes sha256::hash(const Bytes& bytes) const {
    unsigned char bytesout[this->getHashSize()];         // output buffer with hashsize length
    SHA256(bytes.getBytes(), bytes.getLen(), bytesout);  // performs the hash
    Bytes ret(this->getHashSize());
    ret.setBytes(bytesout, this->getHashSize());  // sets the output vector as new bytes in the Bytes object
    return ret;
}

Bytes sha256::hash(const std::string& str) const {
    const unsigned char* bytesin = reinterpret_cast<const unsigned char*>(str.c_str());         // input buffer with length of the input
    unsigned char bytesout[this->getHashSize()];                                                // output buffer with hashsize length
    SHA256(bytesin, str.length(), bytesout);                                                    // performs the hash
    Bytes ret(this->getHashSize());
    ret.setBytes(bytesout, this->getHashSize());  // sets the output vector as new bytes in the Bytes object
    return ret;
}
