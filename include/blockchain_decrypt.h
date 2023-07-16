#pragma once

#include "blockchain.h"

class DecryptBlockChain : public BlockChain {
    /*
    the DecryptBlockChain class represents a queue (vector) of DecryptBlocks
    it is used to decrypt data, its one type of BlockChain
    */
   public:
    bool addBlock() noexcept override;  // adds a new DecryptBlock to the chain
};
