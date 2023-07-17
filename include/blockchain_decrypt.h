#pragma once

#include "blockchain.h"

class DecryptBlockChain : public BlockChain {
    /*
    the DecryptBlockChain class represents a queue (vector) of DecryptBlocks
    it is used to decrypt data, its one type of BlockChain
    */
   public:
    DecryptBlockChain(const Hash* hash, const Bytes passwordhash, const Bytes enc_salt) : BlockChain(hash, passwordhash, enc_salt){};
   protected:
    bool addBlock() noexcept override;  // adds a new DecryptBlock to the chain
};
