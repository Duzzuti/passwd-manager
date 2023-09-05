#include "blockchain_stream_decrypt.h"

#include "block_decrypt.h"

bool DecryptBlockChainStream::setBlock() noexcept {
    if (this->getFreeSpaceInLastBlock() != 0) {
        // there is still free space in the last block no new block is needed
        PLOG_WARNING << "tried to add a new block but there is still free space in the last block (free_space: " << this->getFreeSpaceInLastBlock() << ")";
        return false;
    }

    // get the next block salt
    Bytes next_salt(this->hash_size);
    if (this->current_block.has_value())
        // hashes the last block and use it to generate the next salt
        next_salt = this->salt_iter.next(this->current_block.value()->getHash());
    else
        // no previous block, generate the next salt without a last block hash
        next_salt = this->salt_iter.next();

    // create the new block
    std::unique_ptr<Block> new_block = std::make_unique<DecryptBlock>(this->salt_iter.hashObj, next_salt);

    // add the new block to the chain
    this->current_block = std::move(new_block);

    return true;
}