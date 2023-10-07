#include "blockchain_stream.h"

#include "utility.h"

BlockChainStream::BlockChainStream(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt) : hash_size(hash->getHashSize()), last_block_hash(hash_size) {
    // initialize the salt generator (iterator)
    this->salt_iter.init(passwordhash, enc_salt, std::move(hash));
}

void BlockChainStream::enc_stream(std::ifstream&& in, std::ofstream&& out) noexcept {
    if (this->current_block == nullptr || this->getFreeSpaceInLastBlock() == 0) this->setBlock();
    Bytes data(this->hash_size);
    while (true) {
        size_t free_space = this->current_block->getFreeSpace();
        // get the number of bytes that were already written to the block
        size_t written = this->hash_size - free_space;
        // read the data from the file (max. the free space in the last block)
        data.setLen(0);
        readData(in, data, free_space);

        // add the data to the last block
        this->current_block->addData(data);
        free_space = this->current_block->getFreeSpace();
        // write the result data to the output file, but ignore the first written bytes because they already were written to the file
        
        out.write(reinterpret_cast<char*>(this->current_block->getResult().getBytes() + written), this->hash_size - written - free_space);
            
        if (in.peek() != EOF) {
            // more data is available, so create a new block
            assert(free_space == 0);
            this->setBlock();
        } else
            break;
    }
    PLOG_VERBOSE << "added new data to blockchain [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B (" << this->getDataSize() / 1024 << "KB, "
                 << this->getDataSize() / 1024 / 1024 << "MB)";
}

size_t BlockChainStream::getFreeSpaceInLastBlock() const noexcept {
    // returns the free space in the last block
    if (this->current_block != nullptr)
        return this->current_block->getFreeSpace();
    else
        return 0;
}
