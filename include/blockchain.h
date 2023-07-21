#pragma once
#include <memory>

#include "block.h"
#include "hash.h"
#include "logger.h"

class BlockChain {
    /*
    The blockchain class represents a queue (vector) of blocks
    A BlockChain is initialized with a hash function and a password hash and an encrypted salt
    The hash function should be got from the DataHeader´s hash mode
    The password hash is the hash of the correct password
    The encrypted salt is got from the DataHeader

    The blockchain class is abstract, because you use it differently for encryption and decryption

    You can add new data that should be encrypted/decrypted with addData
    that will add the data to the last block of the chain and if the block is completed it will add a new block
    for each block there is a salt generated that is used to encrypt/decrypt the data
    the salt is generated with the password hash and the encrypted salt as well as the last blocks hash
    this is done with the SaltIterator class
    It is:
        - deterministic
        - you can only get the next salt if you have the last block hash
        - the first salt is known to the attacker and even if he knows the data of the first block
        he cannot get the next salt or passwordhash (except he knows the inverse hash function :))
    */
   protected:
    class SaltIterator {
        /*
        the SaltIterator class is used to generate the salts for the blocks
        it is initialized with the password hash and the encrypted salt
        for every block it generates a new salt with the hash of the last block
        */
       private:
        bool ready;  // is the iterator ready to generate salts
        bool first;  // is this the first salt/block
        Bytes hash;  // the current hash (first is the passwordhash)
        Bytes salt;  // the current salt (first is the encrypted salt)
       public:
        std::shared_ptr<Hash> hashObj;  // the hash object that provides the hash function

       public:
        SaltIterator() {
            // not ready yet, has to be initialized first (with init)
            this->ready = false;
            this->first = true;
        }
        void init(const Bytes pwhash, const Bytes enc_salt, std::shared_ptr<Hash> hashObj) {
            // note that SaltIterator does not take ownership of the hashObj pointer you need to delete it yourself
            // initializes the iterator with the password hash and the encrypted salt
            if (hashObj == nullptr){
                PLOG_FATAL << "given hash object is nullptr";
                throw std::invalid_argument("hash cannot be nullptr");
            }
            if (pwhash.getLen() != hashObj->getHashSize()){
                PLOG_FATAL << "passwordhash has to be the same size as the hash from the hash function (passwordhash_len: " << pwhash.getLen() << ", hash_size: " << hashObj->getHashSize() << ")";
                throw std::invalid_argument("passwordhash has to be the same size as the hash");
            }
            if (enc_salt.getLen() != hashObj->getHashSize()){
                PLOG_FATAL << "encrypted salt has to be the same size as the hash from the hash function (enc_salt_len: " << enc_salt.getLen() << ", hash_size: " << hashObj->getHashSize() << ")";
                throw std::invalid_argument("enc_salt has to be the same size as the hash");
            }
            this->ready = true;
            this->hash = pwhash;
            this->salt = enc_salt;
            this->hashObj = std::move(hashObj);
        }
        Bytes next(Bytes last_block_hash = Bytes(0)) {
            // generates the next salt with the last block hash
            if(this->first) {
                // if this is the first block, the last_block_hash is set to 0
                first = false;
                // generate a Bytes object with the size of the hash with all bytes set to 0
                last_block_hash = Bytes(0);
                last_block_hash = last_block_hash.getFirstBytesFilledUp(this->hashObj->getHashSize(), 0);
            }
            if (last_block_hash.getLen() != this->hashObj->getHashSize()){
                PLOG_FATAL << "last_block_hash has to be the same size as the hash from the hash function (last_block_hash_len: " << last_block_hash.getLen() << ", hash_size: " << this->hashObj->getHashSize() << ")";
                throw std::invalid_argument("last_block_hash has to be the same size as the hash");
            }
            if (!this->ready){
                PLOG_FATAL << "SaltIterator is not ready, call init first";
                throw std::runtime_error("SaltIterator is not ready, call init first");
            }
            // generate the next hash and salt by hashing the last hash and salt with the last block hash
            this->hash = this->hashObj->hash(this->hash + this->salt + last_block_hash);
            // note that the salt is not equal to the hash because the salt is generated with the new hash
            this->salt = this->hashObj->hash(this->hash + this->salt + last_block_hash);
            // return a hash of the hash and the salt to make sure you cannot calculate any of the other values
            return this->hashObj->hash(this->hash + this->salt);
        }
    };

    std::vector<std::unique_ptr<Block>> chain;  // the chain of blocks
    SaltIterator salt_iter;                     // the salt iterator that is used to generate the salts
    size_t hash_size;                           // the byte size of the hash function

   protected:
    // adds a new block to the chain
    virtual bool addBlock() noexcept = 0;
    // returns the free space in the last block
    unsigned char getFreeSpaceInLastBlock() const noexcept;

   public:
    // creates a new empty blockchain with the hash function, the password hash and the encrypted salt
    BlockChain(std::shared_ptr<Hash> hash, const Bytes passwordhash, const Bytes enc_salt);
    BlockChain(const BlockChain&) = delete;
    BlockChain& operator=(const BlockChain&) = delete;
    BlockChain() = delete;

    // adds new data to the blockchain
    void addData(const Bytes data) noexcept;

    // returns the result data of the blockchain
    Bytes getResult() const;

    // returns the number of blocks in the chain
    size_t getHeight() const noexcept { return this->chain.size(); };

    // returns the size of the data in the chain (in Bytes)
    size_t getDataSize() const noexcept { return this->getHeight() * this->hash_size - this->getFreeSpaceInLastBlock(); };
};