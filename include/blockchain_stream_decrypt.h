#pragma once

#include "blockchain_stream.h"
#include "logger.h"

class DecryptBlockChainStream : public BlockChainStream {
    /*
    the DecryptBlockChainStream class represents a BlockChainStream with DecryptBlocks
    it is used to decrypt data, its one type of BlockChainStream
    */
   public:
    DecryptBlockChainStream(std::unique_ptr<Hash> hash, const Bytes passwordhash, const Bytes enc_salt) : BlockChainStream(std::move(hash), passwordhash, enc_salt) {
        PLOG_VERBOSE << "created new DecryptBlockChainStream";
    };

   protected:
    bool setBlock() noexcept override final;  // sets a new DecryptBlock to the chain
};
