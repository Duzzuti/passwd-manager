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

unsigned char BlockChain::getFreeSpaceInLastBlock() const noexcept {
    // returns the free space in the last block
    if (this->chain.size() > 0)
        return this->chain.back()->getFreeSpace();
    else
        return 0;
}
