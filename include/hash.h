#include "bytes.h"
class Hash{
public:
    Hash() = default;
    virtual int getHashSize() const noexcept = 0;
    virtual Bytes hash(Bytes bytes) = 0;
    virtual Bytes hash(std::string str) = 0;
    virtual ~Hash() {};
};