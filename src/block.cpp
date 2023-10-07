/*
contains the implementation of the abstract Block class
*/
#include "block.h"

#include "logger.h"

Block::Block(std::shared_ptr<Hash> hash, const Bytes& salt) : block_len(hash->getHashSize()), data(Bytes(block_len)), dec_hash(Bytes(block_len)), salt(salt) {
    if (this->block_len <= 0) {
        // invalid block length
        PLOG_ERROR << "cannot create new block with invalid block length (len: " << this->block_len << ")";
        throw std::invalid_argument("length of the block cannot be negative or zero");
    }
    if (salt.getLen() != this->block_len) {
        // block length was not fulfilled by the salt
        PLOG_ERROR << "length of salt bytes does not match with the given block length (block_len: " << this->block_len << ", salt_len: " << salt.getLen() << ")";
        throw std::length_error("length of salt bytes does not match with the block length");
    }
    this->hash = std::move(hash);
}

size_t Block::getFreeSpace() const noexcept {
    // returns the number of bytes that can be added to the block until it is completed
    return this->block_len - this->data.getLen();
}

Bytes Block::getHash() const {
    // returns the block hash of the decrypted data if the block is completed
    if (this->getFreeSpace() != 0) {
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

void Block::clear(const Bytes&& salt) {
    // clears the block and sets a new salt
    if (salt.getLen() != this->block_len) {
        // block length was not fulfilled by the salt
        PLOG_ERROR << "length of salt bytes does not match with the given block length (block_len: " << this->block_len << ", salt_len: " << salt.getLen() << ")";
        throw std::length_error("length of salt bytes does not match with the block length");
    }
    this->data.setLen(0);
    this->dec_hash.setLen(0);
    this->salt = salt;
}
