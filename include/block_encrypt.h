#pragma once

#include "block.h"

class EncryptBlock : public Block {
    /*
    the EncryptBlock class represents one block of the Encrypt blockchain
    its a block that is used to encrypt data
    */
   public:
    EncryptBlock(const int len, const Bytes salt) : Block(len, salt){};
    void addData(const Bytes data) override;  // adds new data to the block (this data is encrypted with the salt)
};