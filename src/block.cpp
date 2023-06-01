#include "block.h"

Block::Block() {
    this->len = 0;
    this->data = Bytes();
    this->encoded = Bytes();
    this->passwordhash = Bytes();
    this->salt = Bytes();
}

Block::Block(const int len, const Bytes data, const Bytes salt, const Bytes passwordhash) {
    if (len <= 0) {
        // invalid block length
        throw std::range_error("length of the block cannot be negative or zero");
    }
    if (!((salt.getLen() == len) && (passwordhash.getLen() == len) && (len == data.getLen()))) {
        // block length was not fulfilled by the data
        throw std::length_error("lengths of bytes dont match with the given length");
    }
    this->len = len;
    this->data = data;
    this->encoded = Bytes();
    this->passwordhash = passwordhash;
    this->salt = salt;
}

void Block::setLen(const int len) {
    if (len > 0 && this->data.getLen() == 0 && this->encoded.getLen() == 0 && this->salt.getLen() == 0 && this->passwordhash.getLen() == 0) {
        this->len = len;
    } else {
        // you cannot set the block length because its not a valid length or other data was set before
        throw std::length_error("there already is some data stored in the block. Clear it first");
    }
}

void Block::setData(const Bytes data) {
    if (data.getLen() != this->getLen() || this->getLen() <= 0) {
        // you cannot set the data because it does not fullfil the block length
        throw std::length_error("length of data bytes does not match with the block length");
    }
    this->data = data;
}

void Block::setPasswordHash(const Bytes passwordhash) {
    if (passwordhash.getLen() != this->getLen() || this->getLen() <= 0) {
        // you cannot set the passwordhash because it does not fullfil the block length
        throw std::length_error("length of passwordhash bytes does not match with the block length");
    }
    this->passwordhash = passwordhash;
}

void Block::setSalt(const Bytes salt) {
    if (salt.getLen() != this->getLen() || this->getLen() <= 0) {
        // you cannot set the salt because it does not fullfil the block length
        throw std::length_error("length of salt bytes does not match with the block length");
    }
    this->salt = salt;
}

int Block::getLen() const noexcept { return this->len; }

Bytes Block::getEncoded() const noexcept { return this->encoded; }

bool Block::isReadyForEncode() const noexcept {
    // the block length has to be valid and all data has to be set with the block size length
    return (this->getLen() > 0 && (this->getLen() == this->data.getLen()) && (this->getLen() == this->passwordhash.getLen()) && (this->getLen() == this->salt.getLen()));
}

void Block::calcEncoded() {
    if (!this->isReadyForEncode()) {
        // block was not ready for encode
        throw std::logic_error("Block does not have all needed data (block length, data, salt and passwordhash)");
    }
    this->encoded = this->data + this->salt + this->passwordhash;  // bytes operator overload
}

bool Block::isEncoded() const noexcept {
    // encoded data length is equal to block length and greater zero
    return (this->getLen() > 0 && this->getLen() == this->encoded.getLen());
}

Block::Block(const Bytes encoded) {
    if (encoded.getLen() <= 0) {
        // provided data length is invalid
        throw std::range_error("length of the block cannot be negative or zero");
    }
    this->encoded = encoded;
    this->len = encoded.getLen();
    this->data = Bytes();
    this->passwordhash = Bytes();
    this->salt = Bytes();
}

void Block::setEncoded(const Bytes encoded) {
    if (this->getLen() != encoded.getLen() || this->getLen() <= 0) {
        // you cannot set the encoded because it does not fullfil the block length
        throw std::length_error("length of encoded bytes does not match with the block length");
    }
    this->encoded = encoded;
}

bool Block::isReadyForDecode() const noexcept {
    // the block length has to be valid and all data has to be set with the block size length
    return (this->getLen() > 0 && (this->getLen() == this->encoded.getLen()) && (this->getLen() == this->salt.getLen()) && (this->getLen() == this->passwordhash.getLen()));
}

void Block::calcData() {
    if (!this->isReadyForDecode()) {
        // block was not ready for decoding
        throw std::logic_error("Block does not have all needed data (block length, encoded, salt and passwordhash)");
    }
    this->data = this->encoded - this->salt - this->passwordhash;  // bytes operator overload
}

bool Block::isDecoded() const noexcept {
    // data has to be set and valid
    return (this->getLen() > 0 && this->data.getLen() == this->getLen());
}

void Block::clear() noexcept {
    // resets the block
    this->len = 0;
    this->data = Bytes();
    this->encoded = Bytes();
    this->passwordhash = Bytes();
    this->salt = Bytes();
}

Bytes Block::getSalt() const noexcept { return this->salt; }

Bytes Block::getPasswordHash() const noexcept { return this->passwordhash; }

Bytes Block::getData() const noexcept { return this->data; }
