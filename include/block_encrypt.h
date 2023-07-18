#pragma once

#include "block.h"

class EncryptBlock : public Block {
    /*
    the EncryptBlock class represents one block of the Encrypt blockchain
    its a block that is used to encrypt data
    */
   public:
    EncryptBlock(std::shared_ptr<Hash> hash, const int len, const Bytes salt) : Block(std::move(hash), len, salt){};
    void addData(const Bytes dec_data) override;  // adds new data to the block (this data is encrypted with the salt)
    Bytes getResult() const noexcept override;    // getter for the result data
};