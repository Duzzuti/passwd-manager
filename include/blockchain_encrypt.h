#pragma once

#include "blockchain.h"

class EncryptBlockChain : public BlockChain{
    /*
    the EncryptBlockChain class represents a queue (vector) of EncryptBlocks
    it is used to encrypt data, its one type of BlockChain
    */
   public:
    bool addBlock() noexcept override;  // adds a new EncryptBlocks to the chain
};
