#include <fstream>

#include "blockchain.h"

BlockChain::BlockChain(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt) {
    // initialize the salt generator (iterator)
    this->hash_size = hash->getHashSize();
    this->result = std::make_unique<Bytes>(0);
    this->salt_iter.init(passwordhash, enc_salt, std::move(hash));
}

void BlockChain::addData(const Bytes& data) {
    this->result->addSize(data.getLen());
    if (this->current_block == nullptr) this->addBlock();
    u_int64_t written = 0;  // the amount of data that has been added to the blockchain
    while (true) {
        // get the data that can be added on the last block to complete it
        // it is the minimum of the free space in the last block and the length of the remaining data
        Bytes data_part = data.copySubBytes(written, written + std::min<int>(this->getFreeSpaceInLastBlock(), data.getLen() - written));
        written += data_part.getLen();
        this->current_block->addData(data_part);
        // add a new block if data is left
        if (written < data.getLen())
            this->addBlock();
        else
            break;
    }
    PLOG_VERBOSE << "added new data to blockchain [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B";
}

void BlockChain::addData(std::ifstream&& filestream, const size_t stream_len) {
    this->result->addSize(stream_len);
    if (this->current_block == nullptr) this->addBlock();
    u_int64_t written = 0;  // the amount of data that has been added to the blockchain
    while (true) {
        // get the data that can be added on the last block to complete it
        // it is the minimum of the free space in the last block and the length of the remaining data
        Bytes data_part{std::min<int>(this->getFreeSpaceInLastBlock(), stream_len - written)};
        if(filestream.readsome(reinterpret_cast<char*>(data_part.getBytes()), data_part.getMaxLen()) != data_part.getMaxLen()) {
            PLOG_FATAL << "could not read all bytes from file. streamsize: " << stream_len << ", written: " << written << ", data_part_len: " << data_part.getLen();
            throw std::runtime_error("could not read all bytes from file");
        }
        data_part.setLen(data_part.getMaxLen());
        written += data_part.getLen();
        this->current_block->addData(data_part);
        // add a new block if data is left
        if (written < stream_len)
            this->addBlock();
        else
            break;
    }
    PLOG_VERBOSE << "added new data to blockchain [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B";
}

void BlockChain::addData(std::unique_ptr<Bytes>&& data) {
    this->result->addSize(data->getLen());
    if (this->current_block == nullptr) this->addBlock();
    u_int64_t written = 0;  // the amount of data that has been added to the blockchain
    while (true) {
        // get the data that can be added on the last block to complete it
        // it is the minimum of the free space in the last block and the length of the remaining data
        Bytes data_part = data->copySubBytes(written, written + std::min<int>(this->getFreeSpaceInLastBlock(), data->getLen() - written));
        written += data_part.getLen();
        this->current_block->addData(data_part);
        // add a new block if data is left
        if (written < data->getLen())
            this->addBlock();
        else
            break;
    }
    PLOG_VERBOSE << "added new data to blockchain [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B";
}

std::unique_ptr<Bytes> BlockChain::getResult() {
    // returns the bytes of the blockchain by moving the result
    PLOG_DEBUG << "moving blockchain result. [HEIGHT] " << this->getHeight() << " [DATA_SIZE] " << this->getDataSize() << "B";
    this->current_block->getResult().addcopyToBytes(this->result);
    return std::move(this->result);
}

unsigned char BlockChain::getFreeSpaceInLastBlock() const noexcept {
    // returns the free space in the last block
    if (this->current_block != nullptr)
        return this->current_block->getFreeSpace();
    else
        return 0;
}
