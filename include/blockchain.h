#pragma once
#include "block.h"
#include "blockchainBase.h"
#include "hash.h"

class BlockChain {
   private:
    class SaltIterator {
       private:
        bool ready;
        bool first;
        Bytes hash;
        Bytes salt;
        const Hash* hashObj;

       public:
        SaltIterator() {
            this->ready = false;
            this->first = true;
        }
        void init(const Bytes pwhash, const Bytes enc_salt, const Hash* hashObj) {
            if (hashObj == nullptr) throw std::invalid_argument("hash cannot be nullptr");
            if (pwhash.getLen() != hashObj->getHashSize()) throw std::invalid_argument("passwordhash has to be the same size as the hash");
            if (enc_salt.getLen() != hashObj->getHashSize()) throw std::invalid_argument("enc_salt has to be the same size as the hash");
            this->ready = true;
            this->hash = pwhash;
            this->salt = enc_salt;
            this->hashObj = hashObj;
        }
        Bytes next(Bytes last_block_hash = Bytes(0)) {
            if (!first) {
                if (last_block_hash.getLen() != this->hashObj->getHashSize()) throw std::invalid_argument("last_block_hash has to be the same size as the hash");
            } else {
                first = false;
                // generate a Bytes object with the size of the hash with all bytes set to 0
                last_block_hash = Bytes(0);
                last_block_hash = last_block_hash.getFirstBytesFilledUp(this->hashObj->getHashSize(), 0);
            }
            if (last_block_hash.getLen() != this->hashObj->getHashSize()) throw std::invalid_argument("last_block_hash has to be the same size as the hash");
            if (!this->ready) throw std::runtime_error("SaltIterator is not ready, call init first");
            this->hash = this->hashObj->hash(this->hash + this->salt + last_block_hash);
            this->salt = this->hashObj->hash(this->hash + this->salt + last_block_hash);
            return this->hashObj->hash(this->hash + this->salt + last_block_hash);
        }
        ~SaltIterator() { delete this->hashObj; }
    };

    std::vector<Block> chain;
    const Hash* hash;
    SaltIterator salt_iter;

   private:
    bool addBlock() noexcept;
    unsigned char getFreeSpaceInLastBlock() const noexcept;

   public:
    BlockChain(const Hash* hash, const Bytes passwordhash, const Bytes enc_salt);
    BlockChain(const BlockChain&) = delete;
    BlockChain& operator=(const BlockChain&) = delete;
    BlockChain() = delete;

    virtual void addData(const Bytes data) noexcept;

    virtual Bytes getData() const;

    ~BlockChain() { delete this->hash; };
};