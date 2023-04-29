#include "pwfunc.h"

PwFunc::PwFunc(const Hash *hash) noexcept{
    this->hash = hash;
}

Bytes PwFunc::chainhash(const std::string password, int iterations) const noexcept{
    Bytes ret = this->hash->hash(password);     //hashes the password
    for(int i=1; i < iterations; i++){
        //for iterations -1 the hash is hashed again
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithConstantSalt(const std::string password, int iterations, const std::string salt) const noexcept{
    Bytes ret = this->hash->hash(password+salt);        //hashes the password with the salt added
    Bytes hashed_salt = this->hash->hash(salt);         //hashes the salt
    for(int i=1; i < iterations; i++){
        //for iterations -1 the salt hash is added to the current hash and the result is hashed again
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountSalt(const std::string password, int iterations, int salt_start) const noexcept{
    Bytes ret = this->hash->hash(password+std::to_string(salt_start));  //hashes the password with the start salt added
    for(int i=1; i < iterations; i++){
        //for iterations - 1 the salt will count up and get hashed. The hash is added to the current hash and is hashed again
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountAndConstantSalt(const std::string password, int iterations, int salt_start, const std::string salt) const noexcept{
    Bytes ret = this->hash->hash(password+salt+std::to_string(salt_start)); //the password is hashed with the salt and the count salt
    Bytes hashed_constant_salt = this->hash->hash(salt);    //the constant salt gets hashed
    for(int i=1; i < iterations; i++){
        //for iterations -1 the count salt will increment and get hashed. Next the constant salt hash gets added to the current hash as well as the count salt hash
        //the result is hashed again
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));
        ret.addBytes(hashed_constant_salt);
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}
