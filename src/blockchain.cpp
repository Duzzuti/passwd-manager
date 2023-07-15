#include "blockchain.h"

BlockChain::BlockChain(const Hash* hash, const Bytes passwordhash, const Bytes enc_salt) {
    // initialize the salt generator (iterator)
    this->salt_iter.init(passwordhash, enc_salt, hash);
    this->hash = hash;
}

void BlockChain::addData(const Bytes data) noexcept {
    Bytes data_copy = data;
    while (true) {
        // get the data that can be added on the last block to complete it
        // it is the minimum of the free space in the last block and the length of the data
        std::optional<Bytes> data_part = data_copy.popFirstBytes(std::min<unsigned char>(this->getFreeSpaceInLastBlock(), data_copy.getLen()));
        if (!data_part.has_value()) {
            // there is no data left to add
            break;
        }
        // add the data to the last block
        this->chain.back()->addData(data_part.value());
        // add a new block if still data is left
        if (data_copy.getLen() != 0)
            this->addBlock();
        else
            break;
    }
}

Bytes BlockChain::getResult() const {
    // returns the result of the blockchain by concatenating the results of all blocks
    Bytes res{};
    for (int i = 0; i < this->chain.size(); i++) {
        res.addBytes(this->chain[i]->getResult());
    }
    return res;
}

// bool BlockChain::addBlock() noexcept{
//     if(this->getFreeSpaceInLastBlock() != 0)
//         // there is still free space in the last block no new block is needed
//         return false;

//     // get the next block salt
//     Bytes next_salt;
//     if(this->chain.size() > 0)
//         // hashes the last block and use it to generate the next salt
//         next_salt = this->salt_iter.next(this->hash->hash(this->chain.back()));
//     else
//         // no previous block, generate the next salt without a last block hash
//         next_salt = this->salt_iter.next();

//     // create the new block
//     Block new_block(this->hash->getHashSize(), next_salt);

//     // add the new block to the chain
//     this->chain.push_back(new_block);

//     return true;
// }

unsigned char BlockChain::getFreeSpaceInLastBlock() const noexcept {
    // returns the free space in the last block
    if (this->chain.size() > 0)
        return this->chain.back()->getFreeSpace();
    else
        return 0;
}
