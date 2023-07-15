#pragma once

#include "bytes.h"

class Block {
    /*
    the abstract block class represents one block of the blockchain
    one block has a fixed length and a salt that is used to encrypt the input data
    if new data is pushed into the block (with addData) it is encrypted/decrypted with the salt
    that should be implemented in the derived class
    after encrypting/decrypting the data it is added to the data member
    if the block is completed (the data member is full) the result can be retrieved with getResult
    */
   private:
    unsigned char block_len;  // block len in bytes
    Bytes data;               // data produced by the block
    Bytes salt;               // salt for decryption. The salt is added/subtracted from the input data

   public:
    // delete the default constructor
    // blocks should only be created with a valid length and salt
    Block() = delete;
    Block(const unsigned char len, const Bytes salt);  // creates a block with a length and a salt that is used to encrypt input data
    unsigned char getFreeSpace() const noexcept;       // returns the available space in the block
    virtual void addData(const Bytes data);            // adds new data to the block (this data is encrypted with the salt)
    Bytes getResult() const noexcept;                  // getter for the result data

    virtual ~Block() = default;  // virtual destructor to make sure the derived class destructor is called
};
