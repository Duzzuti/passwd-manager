#include "blockchain_stream.h"
#include "utility.h"

BlockChainStream::BlockChainStream(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt) : hash_size(hash->getHashSize()), last_block_hash(hash_size) {
    // initialize the salt generator (iterator)
    this->salt_iter.init(passwordhash, enc_salt, std::move(hash));
}

void BlockChainStream::enc_stream(std::ifstream&& in, std::ofstream&& out) noexcept {
    if (!this->current_block.has_value() || this->getFreeSpaceInLastBlock() == 0) this->setBlock();
    while (true) {
        // get the number of bytes that were already written to the block
        size_t written = this->hash_size - this->getFreeSpaceInLastBlock();
        // read the data from the file (max. the free space in the last block)
        Bytes data(this->getFreeSpaceInLastBlock());
        readData(in, data, this->getFreeSpaceInLastBlock());

        // add the data to the last block
        this->current_block.value()->addData(data);

        // write the result data to the output file, but ignore the first written bytes because they already were written to the file
        size_t free_space = this->current_block.value()->getFreeSpace();
        out.write(reinterpret_cast<char*>(this->current_block.value()->getResult().copySubBytes(written, this->hash_size - free_space).getBytes()), this->hash_size - written - free_space);

        if (in.peek() != EOF) {
            // more data is available, so create a new block
            assert(this->getFreeSpaceInLastBlock() == 0);
            assert(free_space == 0);
            this->setBlock();
        } else
            break;
    }
    PLOG_VERBOSE << "added new data to blockchain [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B (" << this->getDataSize() / 1024 << "KB, " << this->getDataSize() / 1024 / 1024 << "MB)";
}

size_t BlockChainStream::getFreeSpaceInLastBlock() const noexcept {
    // returns the free space in the last block
    if (this->current_block.has_value())
        return this->current_block.value()->getFreeSpace();
    else
        return 0;
}
