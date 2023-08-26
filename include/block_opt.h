#pragma once

#include <memory>

#include "bytes_opt.h"
#include "hash.h"

class BlockOpt {
    /*
    the abstract block class represents one block of the blockchain
    one block has a fixed length and a salt that is used to encrypt/decrypt the input data
    new data is pushed into the block with addData
    that should be implemented in the derived class
    if the block is completed (the data member is full) the result can be retrieved with getResult
    */
   protected:
    size_t block_len;            // block len in bytes
    BytesOpt data;               // data pushed to the block
    const BytesOpt salt;         // salt for encryption/decryption. The salt is added/subtracted from the input data
    BytesOpt dec_hash;           // block hash of the decrypted data
    std::shared_ptr<Hash> hash;  // hash function to calculate the block hash of the decrypted data

   public:
    // delete the default constructor
    // blocks should only be created with a valid length and salt
    BlockOpt() = delete;
    // creates a block with a length and a salt that is used to encrypt input data
    // requires a Hash to calculate the block hash of the decrypted data if necessary (save memory)
    BlockOpt(std::shared_ptr<Hash> hash, const BytesOpt& salt);
    size_t getFreeSpace() const noexcept;             // returns the available space in the block
    virtual void addData(const BytesOpt& data) = 0;   // adds new data to the block (this data is encrypted/decrypted with the salt)
    virtual BytesOpt getResult() const noexcept = 0;  // getter for the result data
    BytesOpt getHash() const;                         // getter for the block hash of the decrypted data (block has to be completed)

    virtual ~BlockOpt() = default;  // virtual destructor to make sure the derived class destructor is called
};
