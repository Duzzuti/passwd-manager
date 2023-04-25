#include <openssl/sha.h>
#include "hash.h"

class sha256 : Hash{
public:
    int getHashSize() const noexcept{return SHA256_DIGEST_LENGTH;}
    Bytes hash(Bytes bytes);
    Bytes hash(std::string str);
};