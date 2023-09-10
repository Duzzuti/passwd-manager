#pragma once

#include "blockchain.h"
#include "logger.h"

class EncryptBlockChain : public BlockChain {
    /*
    the EncryptBlockChain class represents a queue (vector) of EncryptBlocks
    it is used to encrypt data, its one type of BlockChain
    */
   public:
    EncryptBlockChain(std::unique_ptr<Hash>&& hash, const Bytes& passwordhash, const Bytes& enc_salt) : BlockChain(std::move(hash), passwordhash, enc_salt) {
        PLOG_VERBOSE << "created new EncryptBlockChain";
    };

   protected:
    bool addBlock() override;  // adds a new EncryptBlocks to the chain
};
