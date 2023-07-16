#pragma once

#include "block.h"

class EncryptBlock : public Block {
    /*
    the EncryptBlock class represents one block of the Encrypt blockchain
    its a block that is used to encrypt data
    */
   public:
    void addData(const Bytes data);  // adds new data to the block (this data is encrypted with the salt)
};