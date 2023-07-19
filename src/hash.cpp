/*
implementations of the hash class
*/
#include "hash.h"

#include "logger.h"

Bytes Hash::hash(const Block* block) const {
    // the block hash function takes a block and returns the hash of the result Bytes
    // that means that the block has to be completed before it can be hashed
    Bytes result = block->getResult();
    if (result.getLen() != this->getHashSize()) {
        PLOG_ERROR << "block has to be completed before it can be hashed (hash size: " << this->getHashSize() << ", block size: " << result.getLen() << ")";
        throw std::length_error("block has to be completed before it can be hashed");
    }
    return this->hash(result);
}