#include "blockchain.h"

BlockChain::BlockChain(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt) {
    // initialize the salt generator (iterator)
    this->hash_size = hash->getHashSize();
    this->salt_iter.init(passwordhash, enc_salt, std::move(hash));
}

void BlockChain::addData(const Bytes& data) noexcept {
    if (this->chain.size() == 0) this->addBlock();
    u_int64_t written = 0;  // the amount of data that has been added to the blockchain
    while (true) {
        // get the data that can be added on the last block to complete it
        // it is the minimum of the free space in the last block and the length of the remaining data
        Bytes data_part = data.copySubBytes(written, written + std::min<int>(this->getFreeSpaceInLastBlock(), data.getLen() - written));
        written += data_part.getLen();
        this->chain.back()->addData(data_part);
        // add a new block if data is left
        if (written < data.getLen())
            this->addBlock();
        else
            break;
    }
    PLOG_VERBOSE << "added new data to blockchain [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B";
}

Bytes BlockChain::getResult() const {
    // returns the result of the blockchain by concatenating the results of all blocks
    Bytes res(this->getDataSize());
    for (int i = 0; i < this->chain.size(); i++) {
        this->chain[i]->getResult().addcopyToBytes(res);
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
