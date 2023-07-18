#include "blockchain_decrypt.h"

#include "block_decrypt.h"

bool DecryptBlockChain::addBlock() noexcept {
    if (this->getFreeSpaceInLastBlock() != 0)
        // there is still free space in the last block no new block is needed
        return false;

    // get the next block salt
    Bytes next_salt;
    if (this->chain.size() > 0)
        // hashes the last block and use it to generate the next salt
        next_salt = this->salt_iter.next(this->chain.back()->getHash());
    else
        // no previous block, generate the next salt without a last block hash
        next_salt = this->salt_iter.next();

    // create the new block
    std::unique_ptr<Block> new_block = std::make_unique<DecryptBlock>(this->salt_iter.hashObj, this->hash_size, next_salt);

    // add the new block to the chain
    this->chain.push_back(std::move(new_block));

    return true;
}