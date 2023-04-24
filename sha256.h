#include "hash.h"

class sha256 : Hash{
public:
    static const int hash_size = SHA256_DIGEST_LENGTH;
    Bytes hash(Bytes bytes);
    Bytes hash(std::string str);
};