#include "block_encrypt.h"

#include "hash.h"

void EncryptBlock::addData(const Bytes dec_data) {
    // add the decrypted data to the block
    this->data.addBytes(dec_data);
    if (this->data.getLen() > this->block_len) {
        // reset the data
        this->data = this->data.getFirstBytes(this->data.getLen() - dec_data.getLen()).value();
        throw std::length_error("block data length exceeded the block length");
    }
    if (this->getFreeSpace() == 0) {
        // block is completed
        // calculate the block hash of the decrypted data
        this->dec_hash = this->hash->hash(this->data);
        this->data = this->data + this->salt;
    }
}

Bytes EncryptBlock::getResult() const noexcept {
    // returns the encrypted data
    if (this->data.getLen() != this->block_len) {
        // block is not completed
        // calculate the encrypted data
        return this->data + this->salt.getFirstBytes(this->data.getLen()).value();
    }
    // the block has been encrypted
    return this->data;
}