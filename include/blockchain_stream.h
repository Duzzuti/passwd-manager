#pragma once
#include <fstream>
#include <memory>
#include <optional>

#include "block.h"
#include "salt_iter.h"

class BlockChainStream {
    /*
    The blockchain stream class represents a optimized blockchain class
    it will not store the blocks in a vector but stream the data from a file to a file

    A BlockChainStream is initialized with a hash function, a password hash and an encrypted salt
    The hash function should be got from the DataHeader´s hash mode
    The password hash is the hash of the correct password
    The encrypted salt is got from the DataHeader

    The BlockChainStream class is abstract, because it uses different blocks for encryption and decryption

    You can stream new data that should be encrypted/decrypted with enc_stream
    that will add the data to the last block of the chain and if the block is completed it will write the result to the output file
    for each block there is a salt generated that is used to encrypt/decrypt the data
    the salt is generated with the password hash and the encrypted salt as well as the last blocks hash
    this is done with the SaltIterator class
    It is:
        - deterministic
        - you can only get the next salt if you have the last block hash
        - if the first salt is known to the attacker and even if he knows the data of the first block
        he cannot get the next salt or passwordhash (except he knows the inverse hash function :))
    */
   protected:
    u_int64_t height;                                // the height of the chain
    const size_t hash_size;                          // the byte size of the hash function
    std::unique_ptr<Block> current_block = nullptr;  // the current block
    Bytes last_block_hash;                           // the hash of the last block
    SaltIterator salt_iter;                          // the salt iterator that is used to generate the salts

   protected:
    // sets a new block and updates the last block hash from the current block
    virtual bool setBlock() noexcept = 0;
    // returns the free space in the block
    size_t getFreeSpaceInLastBlock() const noexcept;

   public:
    // creates a new empty blockchain with the hash function, the password hash and the encrypted salt
    BlockChainStream(std::shared_ptr<Hash> hash, const Bytes& passwordhash, const Bytes& enc_salt);
    BlockChainStream(const BlockChainStream&) = delete;
    BlockChainStream& operator=(const BlockChainStream&) = delete;
    BlockChainStream() = delete;

    // adds new data to the blockchain (streams the data encrypted/decrypted from the input file to the output file)
    void enc_stream(std::ifstream&& in, std::ofstream&& out) noexcept;

    // returns the number of blocks in the chain
    size_t getHeight() const noexcept { return this->height; };

    // returns the size of the data in the chain (in Bytes)
    size_t getDataSize() const noexcept { return this->height * this->hash_size - this->getFreeSpaceInLastBlock(); };
};