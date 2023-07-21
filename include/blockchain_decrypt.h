#pragma once

#include "blockchain.h"
#include "logger.h"

class DecryptBlockChain : public BlockChain {
    /*
    the DecryptBlockChain class represents a queue (vector) of DecryptBlocks
    it is used to decrypt data, its one type of BlockChain
    */
   public:
    DecryptBlockChain(std::unique_ptr<Hash> hash, const Bytes passwordhash, const Bytes enc_salt) : BlockChain(std::move(hash), passwordhash, enc_salt) {
        PLOG_VERBOSE << "created new DecryptBlockChain";
    };

   protected:
    bool addBlock() noexcept override;  // adds a new DecryptBlock to the chain
};
