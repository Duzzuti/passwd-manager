#include "block.h"

Block::Block(){
    this->len = 0;
    this->data = Bytes();
    this->encoded = Bytes();
    this->passwordhash = Bytes();
    this->salt = Bytes();
}

Block::Block(int len, Bytes data, Bytes salt, Bytes password){
    if(len <= 0){
        throw std::range_error("length of the block cannot be negative or zero");
    }
    if(!((salt.getLen() == len) && (password.getLen() == len) && (len == data.getLen()))){
        throw std::length_error("lengths of bytes dont match with the given length");
    }
    this->len = len;
    this->data = data;
    this->encoded = Bytes();
    this->passwordhash = password;
    this->salt = salt;
}

void Block::setLen(int len){
    if(len > 0 && this->data.getLen() == 0 && this->encoded.getLen() == 0 && this->salt.getLen() == 0 && this->passwordhash.getLen() == 0){
       this->len = len; 
    }else{
        throw std::length_error("there already is some data stored in the block. Clear it first");
    }
}

void Block::setData(Bytes data){
    if(data.getLen() != this->getLen() || this->getLen() <= 0){
        throw std::length_error("length of data bytes does not match with the block length");
    }
    this->data = data;
}

void Block::setPasswordHash(Bytes passwordhash){
    if(passwordhash.getLen() != this->getLen() || this->getLen() <= 0){
        throw std::length_error("length of passwordhash bytes does not match with the block length");
    }
    this->passwordhash = passwordhash;
}

void Block::setSalt(Bytes salt){
    if(salt.getLen() != this->getLen() || this->getLen() <= 0){
        throw std::length_error("length of salt bytes does not match with the block length");
    }
    this->salt = salt;
}

int Block::getLen() const noexcept{
    return this->len;
}

Bytes Block::getEncoded() const noexcept{
    return this->encoded;
}

bool Block::isReadyForEncode() const noexcept{
    return (this->getLen() > 0 && (this->getLen() == this->data.getLen()) && (this->getLen() == this->passwordhash.getLen()) && (this->getLen() == this->salt.getLen()));
}

void Block::calcEncoded(){
    if(!this->isReadyForEncode()){
        throw std::length_error("Block does not have all needed data (block length, data, salt and passwordhash)");
    }
    this->encoded = this->data + this->salt + this->passwordhash;
}

bool Block::isEncoded() const noexcept{
    return (this->getLen() > 0 && this->getLen() == this->encoded.getLen());
}

Block::Block(Bytes encoded){
    if(encoded.getLen() <= 0){
        throw std::range_error("length of the block cannot be negative or zero");
    }
    this->encoded = encoded;
    this->len = encoded.getLen();
    this->data = Bytes();
    this->passwordhash = Bytes();
    this->salt = Bytes();
}

void Block::setEncoded(Bytes encoded){
    if(this->getLen() != encoded.getLen() || this->getLen() <= 0){
        throw std::length_error("length of encoded bytes does not match with the block length");
    }
    this->encoded = encoded;
}

bool Block::isReadyForDecode() const noexcept{
    return (this->getLen() > 0 && (this->getLen() == this->encoded.getLen()) && (this->getLen() == this->salt.getLen()) && (this->getLen() == this->passwordhash.getLen()));
}

void Block::calcData(){
    if(!this->isReadyForDecode()){
        throw std::length_error("Block does not have all needed data (block length, encoded, salt and passwordhash)");
    }
    this->data = this->encoded - this->salt - this->passwordhash;
}

bool Block::isDecoded() const noexcept{
    return (this->getLen() > 0 && this->data.getLen() == this->getLen());
}

void Block::clear() noexcept{
    this->len = 0;
    this->data = Bytes();
    this->encoded = Bytes();
    this->passwordhash = Bytes();
    this->salt = Bytes();
}

Bytes Block::getSalt() const noexcept{
    return this->salt;
}

Bytes Block::getPasswordHash() const noexcept{
    return this->passwordhash;
}

Bytes Block::getData() const noexcept{
    return this->data;
}


