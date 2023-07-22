/*
contains the implementation of the abstract Block class
*/
#include "block.h"

#include "logger.h"

Block::Block(std::shared_ptr<Hash> hash, const int len, const Bytes salt) {
    if (len <= 0) {
        // invalid block length
        PLOG_ERROR << "cannot create new block with invalid block length (len: " << len << ")";
        throw std::range_error("length of the block cannot be negative or zero");
    }
    if (salt.getLen() != len) {
        // block length was not fulfilled by the salt
        PLOG_ERROR << "length of salt bytes does not match with the given block length (block_len: " << len << ", salt_len: " << salt.getLen() << ")";
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
    if (this->data.getLen() != this->block_len) {
        // block is not completed
        PLOG_ERROR << "block is not completed, cannot get hash (block_len: " << this->block_len << ", data_len: " << this->data.getLen() << ")";
        throw std::length_error("block is not completed, cannot get hash");
    }
    if (this->dec_hash.getLen() == 0) {
        // block hash was not calculated previously. The only way that should happen is if the block is not yet completed
        // this is checked in the previous if statement so this should never happen
        PLOG_FATAL << "block hash was not calculated, but the block is completed (block_len: " << this->block_len << ", data_len: " << this->data.getLen() << ")";
        throw std::length_error("block hash was not calculated");
    }
    return this->dec_hash;
}