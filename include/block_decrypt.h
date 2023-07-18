#pragma once

#include "block.h"

class DecryptBlock : public Block {
    /*
    the DecryptBlock class represents one block of the Decrypt blockchain
    its a block that is used to decrypt data
    */
   public:
    DecryptBlock(std::shared_ptr<Hash> hash, const int len, const Bytes salt) : Block(std::move(hash), len, salt){};
    void addData(const Bytes enc_data) override;  // adds new data to the block (this data is decrypted with the salt)
    Bytes getResult() const noexcept override;    // getter for the result data
};