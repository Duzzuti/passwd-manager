#include "sha512.h"

Bytes sha512::hash(const Bytes bytes) const {
    unsigned char bytesin[bytes.getLen()];        // input buffer with length of the input
    unsigned char bytesout[this->getHashSize()];  // output buffer with hashsize length
    auto inpbytes = bytes.getBytes();
    std::copy(inpbytes.begin(), inpbytes.end(), bytesin);                                       // copy the data into the input buffer
    SHA512(bytesin, bytes.getLen(), bytesout);                                                  // performs the hash
    std::vector<unsigned char> v(bytesout, bytesout + sizeof(bytesout) / sizeof(bytesout[0]));  // turns the output buffer into a vector
    Bytes ret;
    ret.setBytes(v);  // sets the output vector as new bytes in the Bytes object
    return ret;
}

Bytes sha512::hash(const std::string str) const {
    unsigned char bytesin[str.length()];                                                        // input buffer with length of the input
    std::copy(str.begin(), str.end(), bytesin);                                                 // copy the string data into the input buffer
    unsigned char bytesout[this->getHashSize()];                                                // output buffer with hashsize length
    SHA512(bytesin, str.length(), bytesout);                                                    // performs the hash
    std::vector<unsigned char> v(bytesout, bytesout + sizeof(bytesout) / sizeof(bytesout[0]));  // turns the output buffer into a vector
    Bytes ret;
    ret.setBytes(v);  // sets the output vector as new bytes in the Bytes object
    return ret;
}
