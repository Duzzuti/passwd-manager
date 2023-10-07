#pragma once
#include <memory>

#include "block.h"
#include "salt_iter.h"

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
    
    std::unique_ptr<Block> current_block = nullptr;  // the current block that is being filled
    std::unique_ptr<Bytes> result = nullptr;         // the result data of the blockchain
    SaltIterator salt_iter;                          // the salt iterator that is used to generate the salts
    size_t hash_size;                                // the byte size of the hash function
    size_t chain_height = 0;                         // the height of the chain

   protected:
    // adds a new block to the chain
    virtual bool addBlock() = 0;
    // returns the free space in the last block
    unsigned char getFreeSpaceInLastBlock() const noexcept;

   public:
    // creates a new empty blockchain with the hash function, the password hash and the encrypted salt
    BlockChain(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt);
    BlockChain(const BlockChain&) = delete;
    BlockChain& operator=(const BlockChain&) = delete;
    BlockChain() = delete;

    // adds new data to the blockchain
    void addData(const Bytes& data);
    void addData(std::ifstream&& filestream, const size_t stream_len);
    void addData(std::unique_ptr<Bytes>&& data);

    // returns the result data of the blockchain
    std::unique_ptr<Bytes> getResult();

    // returns the number of blocks in the chain
    size_t getHeight() const noexcept { return this->chain_height; };

    // returns the size of the data in the chain (in Bytes)
    size_t getDataSize() const noexcept { return this->getHeight() * this->hash_size - this->getFreeSpaceInLastBlock(); };
};