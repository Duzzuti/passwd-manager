#include <openssl/sha.h>
#include "sha256.h"


Bytes sha256::hash(Bytes bytes){
    unsigned char bytesin[bytes.getLen()];
    unsigned char bytesout[bytes.getLen()];
    std::copy(bytes.getBytes().begin(), bytes.getBytes().end(), bytesin);
    SHA256(bytesin, bytes.getLen(), bytesout);
    std::vector<unsigned char> v(bytesout, bytesout + sizeof(bytesout) / sizeof(bytesout[0]));
    Bytes ret;
    ret.setBytes(v);
    return ret;
}

Bytes sha256::hash(std::string str){
    unsigned char bytesin[str.length()];
    std::copy( str.begin(), str.end(), bytesin );
    unsigned char bytesout[str.length()];
    SHA256(bytesin, str.length(), bytesout);
    std::vector<unsigned char> v(bytesout, bytesout + sizeof(bytesout) / sizeof(bytesout[0]));
    Bytes ret;
    ret.setBytes(v);
    return ret;
}
