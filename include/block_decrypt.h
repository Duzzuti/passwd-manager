#pragma once

#include "block.h"

class DecryptBlock : public BlockOpt {
    /*
    the DecryptBlock class represents one block of the Decrypt blockchain
    its a block that is used to decrypt data
    */
   public:
    DecryptBlock(std::shared_ptr<Hash> hash, const Bytes& salt) : BlockOpt(std::move(hash), salt){};
    void addData(const Bytes& enc_data) override;  // adds new data to the block (this data is decrypted with the salt)
    Bytes getResult() const noexcept override;     // getter for the result data
};