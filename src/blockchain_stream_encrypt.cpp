#include "blockchain_stream_encrypt.h"

#include "block_encrypt.h"

bool EncryptBlockChainStream::setBlock() noexcept {
    if (this->getFreeSpaceInLastBlock() != 0) {
        // there is still free space in the last block no new block is needed
        PLOG_WARNING << "tried to add a new block but there is still free space in the last block (free_space: " << this->getFreeSpaceInLastBlock() << ")";
        return false;
    }

    // get the next block salt
    if (this->current_block != nullptr){
        // hashes the last block and use it to generate the next salt
        this->current_block->clear(this->salt_iter.next(this->current_block->getHash()));
    }else{
        // no previous block, generate the next salt without a last block hash
        // create the new block
        // add the new block to the chain
        this->current_block = std::make_unique<EncryptBlock>(this->salt_iter.hashObj, this->salt_iter.next());
    }
    this->height++;
    return true;
}