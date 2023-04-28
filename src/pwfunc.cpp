#include "pwfunc.h"

PwFunc::PwFunc(const Hash *hash) noexcept{
    this->hash = hash;
}

Bytes PwFunc::chainhash(const std::string password, int iterations) const noexcept{
    Bytes ret = this->hash->hash(password);
    for(int i=1; i < iterations; i++){
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithConstantSalt(const std::string password, int iterations, const std::string salt) const noexcept{
    Bytes ret = this->hash->hash(password+salt);
    Bytes hashed_salt = this->hash->hash(salt);
    for(int i=1; i < iterations; i++){
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountSalt(const std::string password, int iterations, int salt_start) const noexcept{
    Bytes ret = this->hash->hash(password+std::to_string(salt_start));
    for(int i=1; i < iterations; i++){
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountAndConstantSalt(const std::string password, int iterations, int salt_start, const std::string salt) const noexcept{
    Bytes ret = this->hash->hash(password+salt+std::to_string(salt_start));
    Bytes hashed_constant_salt = this->hash->hash(salt);
    for(int i=1; i < iterations; i++){
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));
        ret.addBytes(hashed_constant_salt);
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}
