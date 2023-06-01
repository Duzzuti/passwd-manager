#pragma once
#include "blockchainBase.h"
#include "bytes.h"
#ifndef BLOCK_H
#define BLOCK_H

class Block {
    /*
    the block class represents one block of the blockchain
    it has data, salt and the passwordhash to compute the encoded data
    */
    friend BlockChain;  // the blockchain has to access private members (like getData)
   private:
    int len;             // block len in bytes
    Bytes data;          // plain text data
    Bytes salt;          // salt to keep the passwordhash secure even if the attacker knows the data
    Bytes passwordhash;  // passwordhash (hash that is derived from the password)
    Bytes encoded;       // the encoded data

   private:
    Bytes getSalt() const noexcept;          // getter for the salt bytes
    Bytes getPasswordHash() const noexcept;  // getter for the passwordhash bytes
    Bytes getData() const noexcept;          // getter for the data bytes
   public:
    // encrypt
    Block();                                                                             // creates a block with a length of zero (you have to call setLen to use this block)
    Block(const int len, const Bytes data, const Bytes salt, const Bytes passwordhash);  // creates a block with all neccessary data to encode
    void setLen(const int len);                                                          // sets the len of the block (note that this only works if no other data is set yet)
    void setData(const Bytes data);                                                      // sets the data of the block (note that the length has to be right)
    void setPasswordHash(const Bytes passwordhash);                                      // sets the passwordhash of the block (note that the length has to be right)
    void setSalt(const Bytes salt);                                                      // sets the salt of the block (note that the length has to be right)
    int getLen() const noexcept;                                                         // getter for the block length
    Bytes getEncoded() const noexcept;                                                   // getter for the encoded bytes
    bool isReadyForEncode() const noexcept;                                              // returns true if the block has all data to compute the encoded data
    void calcEncoded();                                                                  // computes the encoded data
    bool isEncoded() const noexcept;                                                     // returns true if the data has been encoded

    // decrypt
    Block(const Bytes encoded);              // creates a block with only encoded data (to decrypt you have to set a passwword hash and a salt)
    void setEncoded(const Bytes encoded);    // setter for encoded data
    bool isReadyForDecode() const noexcept;  // returns true if the block has all data to decrypt
    void calcData();                         // decrypt the encoded data to plain data
    bool isDecoded() const noexcept;         // returns true if the encoded data was decrypted

    void clear() noexcept;  // clears the block data and sets length to zero
};

#endif  //(BLOCK_H)