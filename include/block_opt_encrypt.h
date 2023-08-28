#pragma once

#include "block_opt.h"

class EncryptBlockOpt : public BlockOpt {
    /*
    the EncryptBlock class represents one block of the Encrypt blockchain
    its a block that is used to encrypt data
    */
   public:
    EncryptBlockOpt(std::shared_ptr<Hash> hash, const Bytes& salt) : BlockOpt(std::move(hash), salt){};
    void addData(const Bytes& dec_data) override;  // adds new data to the block (this data is encrypted with the salt)
    Bytes getResult() const noexcept override;     // getter for the result data
};