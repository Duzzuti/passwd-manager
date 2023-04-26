#include "sha384.h"

Bytes sha384::hash(Bytes bytes){
    unsigned char bytesin[bytes.getLen()];
    unsigned char bytesout[this->getHashSize()];
    std::copy(bytes.getBytes().begin(), bytes.getBytes().end(), bytesin);
    SHA384(bytesin, bytes.getLen(), bytesout);
    std::vector<unsigned char> v(bytesout, bytesout + sizeof(bytesout) / sizeof(bytesout[0]));
    Bytes ret;
    ret.setBytes(v);
    return ret;
}

Bytes sha384::hash(std::string str){
    unsigned char bytesin[str.length()];
    std::copy( str.begin(), str.end(), bytesin );
    unsigned char bytesout[this->getHashSize()];
    SHA384(bytesin, str.length(), bytesout);
    std::vector<unsigned char> v(bytesout, bytesout + sizeof(bytesout) / sizeof(bytesout[0]));
    Bytes ret;
    ret.setBytes(v);
    return ret;
}
