#include "pwfunc.h"
#include "settings.h"

ErrorStruct<bool> PwFunc::isPasswordValid(const std::string password) noexcept{
    //checks the password for illegal format, sets an error if it is not valid
    ErrorStruct<bool> ret;
    for(int i=0; i < password.length(); i++){
        //loops through the password characters
        bool found = false;
        for(int j=0; j < VALID_PASS_CHARSET.length(); j++){
            if(password[i] == VALID_PASS_CHARSET[j]){   //checks if the character is in the valid chars
                found = true;
                break;  //character found
            }
        }
        if(!found){ //invalid character, is not in charset
            ret.error = "Password conatins illegal character: '";
            ret.error += password[i];
            ret.error += "'";
            ret.success = false; //passwd contains illegal char
            return ret;
        }
    }
    if(password.length() < MIN_PASS_LEN){   //password length does not match the min length
        ret.error = "Password is too short, it has to be at least "+std::to_string(MIN_PASS_LEN);
        ret.error += " characters long";
        ret.success = false; //passwd too short
        return ret;
    }
    ret.success = true; //passwd is valid
    return ret;
}

PwFunc::PwFunc(const Hash *hash) noexcept
{
    this->hash = hash;
}

Bytes PwFunc::chainhash(const std::string password, const u_int64_t iterations) const noexcept{
    Bytes ret = this->hash->hash(password);     //hashes the password
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations -1 the hash is hashed again
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithConstantSalt(const std::string password, const u_int64_t iterations, const std::string salt) const noexcept{
    Bytes ret = this->hash->hash(password+salt);        //hashes the password with the salt added
    Bytes hashed_salt = this->hash->hash(salt);         //hashes the salt
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations -1 the salt hash is added to the current hash and the result is hashed again
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountSalt(const std::string password, const u_int64_t iterations, u_int64_t salt_start) const noexcept{
    Bytes ret = this->hash->hash(password+std::to_string(salt_start));  //hashes the password with the start salt added
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations - 1 the salt will count up and get hashed. The hash is added to the current hash and is hashed again
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountAndConstantSalt(const std::string password, const u_int64_t iterations, u_int64_t salt_start, const std::string salt) const noexcept{
    Bytes ret = this->hash->hash(password+salt+std::to_string(salt_start)); //the password is hashed with the salt and the count salt
    Bytes hashed_constant_salt = this->hash->hash(salt);    //the constant salt gets hashed
    for(u_int64_t i=1; i < iterations; i++){
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

Bytes PwFunc::chainhashWithQuadraticCountSalt(const std::string password, const u_int64_t iterations, u_int64_t salt_start, const u_int64_t a, const u_int64_t b, const u_int64_t c) const noexcept{
    Bytes ret = this->hash->hash(password+std::to_string(a*salt_start*salt_start + b*salt_start + c));  //hashes the password with the a*start_salt^2 + b*start_salt + c added
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations - 1 the salt will count up and get hashed. The hash is added to the current hash and is hashed again
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(a*salt_start*salt_start + b*salt_start + c));
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhash(const Bytes data, const u_int64_t iterations) const noexcept{
    Bytes ret = this->hash->hash(data);     //hashes the data
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations -1 the hash is hashed again
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithConstantSalt(const Bytes data, const u_int64_t iterations, const std::string salt) const noexcept{
    Bytes hashed_salt = this->hash->hash(salt);         //hashes the salt
    Bytes new_data = data;
    new_data.addBytes(hashed_salt);             //add the salt bytes to the data
    Bytes ret = this->hash->hash(new_data); //hashes the data with the salt added
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations -1 the salt hash is added to the current hash and the result is hashed again
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountSalt(const Bytes data, const u_int64_t iterations, u_int64_t salt_start) const noexcept{
    Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));         //hashes the salt
    Bytes new_data = data;
    new_data.addBytes(hashed_salt);             //add the salt bytes to the data
    Bytes ret = this->hash->hash(new_data); //hashes the data with the salt added
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations - 1 the salt will count up and get hashed. The hash is added to the current hash and is hashed again
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}

Bytes PwFunc::chainhashWithCountAndConstantSalt(const Bytes data, const u_int64_t iterations, u_int64_t salt_start, const std::string salt) const noexcept{
    Bytes hashed_constant_salt = this->hash->hash(salt);         //hashes the constant salt
    Bytes hashed_salt = this->hash->hash(std::to_string(salt_start));         //hashes the count salt
    Bytes new_data = data;
    new_data.addBytes(hashed_constant_salt);    //add the constant salt bytes to the data
    new_data.addBytes(hashed_salt);             //add the count salt bytes to the data
    Bytes ret = this->hash->hash(new_data); //hashes the data with the salt added
    for(u_int64_t i=1; i < iterations; i++){
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

Bytes PwFunc::chainhashWithQuadraticCountSalt(const Bytes data, const u_int64_t iterations, u_int64_t salt_start, const u_int64_t a, const u_int64_t b, const u_int64_t c) const noexcept{
    Bytes hashed_salt = this->hash->hash(std::to_string(a*salt_start*salt_start + b*salt_start + c));         //hashes the quadartic salt
    Bytes new_data = data;
    new_data.addBytes(hashed_salt);             //add the salt bytes to the data
    Bytes ret = this->hash->hash(new_data); //hashes the data with the salt added
    for(u_int64_t i=1; i < iterations; i++){
        //for iterations - 1 the salt will count up and get hashed. The hash is added to the current hash and is hashed again
        salt_start++;
        Bytes hashed_salt = this->hash->hash(std::to_string(a*salt_start*salt_start + b*salt_start + c));
        ret.addBytes(hashed_salt);
        ret = this->hash->hash(ret);
    }
    return ret;
}