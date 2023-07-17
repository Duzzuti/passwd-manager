#include "block_decrypt.h"

void DecryptBlock::addData(const Bytes data) {
    // decrypt the data with the salt
    Bytes decrypted_data = data - this->salt;
    // add the decrypted data to the block
    this->data.addBytes(decrypted_data);
    if (this->data.getLen() > this->block_len) throw std::length_error("block data length exceeded the block length");
}