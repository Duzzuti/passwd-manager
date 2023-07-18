/*
contains the implementation of the abstract Block class
*/
#include "block.h"

Block::Block(std::shared_ptr<Hash> hash, const int len, const Bytes salt) {
    if (len <= 0) {
        // invalid block length
        throw std::range_error("length of the block cannot be negative or zero");
    }
    if (salt.getLen() != len) {
        // block length was not fulfilled by the salt
        throw std::length_error("length of salt bytes does not match with the block length");
    }
    this->block_len = len;
    this->data = Bytes();
    this->dec_hash = Bytes();
    this->salt = salt;
    this->hash = std::move(hash);
}

int Block::getFreeSpace() const noexcept {
    // returns the number of bytes that can be added to the block until it is completed
    return this->block_len - this->data.getLen();
}

Bytes Block::getHash() const {
    // returns the block hash of the decrypted data if the block is completed
    if (this->data.getLen() != this->block_len) throw std::length_error("block is not completed, cannot get hash");
    if (this->dec_hash.getLen() == 0) throw std::length_error("block hash was not calculated");
    return this->dec_hash;
}