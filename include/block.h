#pragma once

#include <memory>

#include "bytes.h"

class Hash;

class Block {
    /*
    the abstract block class represents one block of the blockchain
    one block has a fixed length and a salt that is used to encrypt/decrypt the input data
    new data is pushed into the block with addData
    that should be implemented in the derived class
    if the block is completed (the data member is full) the result can be retrieved with getResult
    */
   protected:
    int block_len;  // block len in bytes
    Bytes data;     // data pushed to the block
    Bytes salt;     // salt for encryption/decryption. The salt is added/subtracted from the input data
    Bytes dec_hash; // block hash of the decrypted data
    std::shared_ptr<Hash> hash; // hash function to calculate the block hash of the decrypted data

   public:
    // delete the default constructor
    // blocks should only be created with a valid length and salt
    Block() = delete;
    // creates a block with a length and a salt that is used to encrypt input data
    // requires a Hash to calculate the block hash of the decrypted data if necessary (save memory)
    Block(std::shared_ptr<Hash> hash, const int len, const Bytes salt);
    int getFreeSpace() const noexcept;       // returns the available space in the block
    virtual void addData(const Bytes data);  // adds new data to the block (this data is encrypted/decrypted with the salt)
    virtual Bytes getResult() const noexcept;        // getter for the result data
    Bytes getHash() const;          // getter for the block hash of the decrypted data (block has to be completed)

    virtual ~Block() = default;  // virtual destructor to make sure the derived class destructor is called
};
