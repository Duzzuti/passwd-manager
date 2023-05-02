#include "modes.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"
#include "pwfunc.h"
#include <iostream>

int Modes::getModeBytesLen(unsigned char const mode)
{
    switch (mode){
    case 1:
        return SHA256_DIGEST_LENGTH;  //simple sha256
    case 2:
        return SHA384_DIGEST_LENGTH;  //simple sha384
    case 3:
        return SHA512_DIGEST_LENGTH;  //simple sha512
    case 4:
        return SHA256_DIGEST_LENGTH+iteration_bytes;  //sha256 chainhash
    case 5:
        return SHA384_DIGEST_LENGTH+iteration_bytes;  //sha384 chainhash
    case 6:
        return SHA512_DIGEST_LENGTH+iteration_bytes;  //sha512 chainhash
    default:
        throw std::invalid_argument("mode "+std::to_string(mode)+" is not defined");
    }
}

Bytes Modes::getSalt(unsigned char const mode, std::string password, Bytes headerBytes){
    if(headerBytes.getLen() != Modes::getModeBytesLen(mode)){
        throw std::invalid_argument("for mode "+std::to_string(mode)+" there are "+std::to_string(Modes::getModeBytesLen(mode))+" bytes required ("+std::to_string(headerBytes.getLen())+" given)");
    }
    switch (mode){
    case 1:
        return Modes::getSalt1(password, headerBytes);
    case 2:
        return Modes::getSalt2(password, headerBytes);
    case 3:
        return Modes::getSalt3(password, headerBytes);
    default:
        throw std::invalid_argument("mode "+std::to_string(mode)+" is not defined");
    }
}

Bytes Modes::getSalt1(std::string password, Bytes headerBytes){
    //mode 1 is a simple sha256 on the password and subtracting from the header bytes to get the salt
    sha256 sha;
    Bytes hashedpass = sha.hash(password);
    return headerBytes - hashedpass;
}

Bytes Modes::getSalt2(std::string password, Bytes headerBytes){
    //mode 2 is a simple sha384 on the password and subtracting from the header bytes to get the salt
    sha384 sha;
    Bytes hashedpass = sha.hash(password);
    return headerBytes - hashedpass;
}

Bytes Modes::getSalt3(std::string password, Bytes headerBytes){
    //mode 3 is a simple sha512 on the password and subtracting from the header bytes to get the salt
    sha512 sha;
    Bytes hashedpass = sha.hash(password);
    return headerBytes - hashedpass;
}

Bytes Modes::getSalt4(std::string password, Bytes headerBytes){
    //mode 4 is a sha256 chainhash on the password and subtracting from the header bytes to get the salt
    sha256* sha = new sha256();
    PwFunc pwf = PwFunc(sha);
    //gets the first bytes that are representing the encoded salt
    Bytes salt_part = headerBytes.popFirstBytes(sha->getHashSize()).value();
    long iterations = toLong(headerBytes);      //the next bytes representing the iterations
    Bytes hashedpass = pwf.chainhash(password, iterations);
    delete sha;
    return salt_part - hashedpass;
}

Bytes Modes::getSalt5(std::string password, Bytes headerBytes){
    //mode 5 is a sha384 chainhash on the password and subtracting from the header bytes to get the salt
    sha384* sha = new sha384();
    PwFunc pwf = PwFunc(sha);
    //gets the first bytes that are representing the encoded salt
    Bytes salt_part = headerBytes.popFirstBytes(sha->getHashSize()).value();
    long iterations = toLong(headerBytes);      //the next bytes representing the iterations
    Bytes hashedpass = pwf.chainhash(password, iterations);
    delete sha;
    return salt_part - hashedpass;
}

Bytes Modes::getSalt6(std::string password, Bytes headerBytes){
    //mode 6 is a sha512 chainhash on the password and subtracting from the header bytes to get the salt
    sha512* sha = new sha512();
    PwFunc pwf = PwFunc(sha);
    //gets the first bytes that are representing the encoded salt
    Bytes salt_part = headerBytes.popFirstBytes(sha->getHashSize()).value();
    long iterations = toLong(headerBytes);      //the next bytes representing the iterations
    Bytes hashedpass = pwf.chainhash(password, iterations);
    delete sha;
    return salt_part - hashedpass;
}
