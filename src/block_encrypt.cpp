#include "block_encrypt.h"

void EncryptBlock::addData(const Bytes data) {
    // encrypt the data with the salt
    Bytes encrypted_data = data + this->salt;
    // add the encrypted data to the block
    this->data.addBytes(encrypted_data);
    if (this->data.getLen() > this->block_len) throw std::length_error("block data length exceeded the block length");
}