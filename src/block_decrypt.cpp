#include "block_decrypt.h"

#include "hash.h"

void DecryptBlock::addData(const Bytes enc_data) {
    // add the data to the block
    this->data.addBytes(enc_data);
    if (this->data.getLen() > this->block_len) {
        // reset the data
        this->data = this->data.getFirstBytes(this->data.getLen() - enc_data.getLen()).value();
        throw std::length_error("block data length exceeded the block length");
    }
    if (this->getFreeSpace() == 0) {
        // block is completed
        // calculate the block hash of the decrypted data
        this->data = this->data - this->salt;
        this->dec_hash = this->hash->hash(this->data);
    }
}

Bytes DecryptBlock::getResult() const noexcept {
    // returns the decrypted data
    if (this->data.getLen() != this->block_len) {
        // block is not completed
        // calculate the decrypted data
        return this->data - this->salt.getFirstBytes(this->data.getLen()).value();
    }
    // the block has been decrypted
    return this->data;
}