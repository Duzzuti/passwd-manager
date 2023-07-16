#pragma once

#include "block.h"

class DecryptBlock : public Block {
    /*
    the DecryptBlock class represents one block of the Decrypt blockchain
    its a block that is used to decrypt data
    */
   public:
    void addData(const Bytes data);            // adds new data to the block (this data is decrypted with the salt)
};