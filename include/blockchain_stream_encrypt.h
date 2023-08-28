#pragma once

#include "blockchain_stream.h"
#include "logger.h"

class EncryptBlockChainStream : public BlockChainStream {
    /*
    the DecryptBlockChainStream class represents a BlockChainStream with EncryptBlocks
    it is used to encrypt data, its one type of BlockChainStream
    */
   public:
    EncryptBlockChainStream(std::unique_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt) : BlockChainStream(std::move(hash), passwordhash, enc_salt) {
        PLOG_VERBOSE << "created new EncryptBlockChainStream";
    };

   protected:
    bool setBlock() noexcept override final;  // sets a new EncryptBlocks to the chain
};
