#pragma once
#ifndef DATAHEADER_H
#define DATAHEADER_H

#include "bytes.h"
#include "hash_modes.h"
#include "chainhash_modes.h"

class DataHeader{
private:
    unsigned char file_mode;   //the file data mode that is choosen (content of the file)
    unsigned char hash_mode;   //the hash mode that is choosen (hash function)
    unsigned char hash_size;    //the size of the hash provided by the hash function (in Bytes)
    unsigned char chainhash1_mode;  //chainhash mode for the first chainhash (password -> passwordhash)
    unsigned char chainhash2_mode;  //chainhash mode for the second chainhash (passwordhash -> validate password)
    unsigned long chainhash1_iters; //iterations for the first chainhash
    unsigned long chainhash2_iters; //iterations for the second chainhash
    unsigned char chainhash1_datablock_len; //the length of the first datablock
    unsigned char chainhash2_datablock_len; //the length of the second datablock
    Bytes chainhash1_datablock;     //the first datablock
    Bytes chainhash2_datablock;     //the second datablock
    Bytes valid_passwordhash;       //saves the hash that should be the result of the second chainhash
    Bytes enc_salt;                 //saves the encoded salt
    Bytes header_bytes;             //bytes that are in the header

private:

public:
    DataHeader(const unsigned char hash_mode);
    void setFileDataMode(const unsigned char file_mode);
    void setHeaderBytes(const Bytes headerBytes);
    unsigned int getHeaderLength() const noexcept;
    void setChainHash1(const unsigned char mode, const unsigned long iters, const unsigned char len, const Bytes datablock);
    void setChainHash2(const unsigned char mode, const unsigned long iters, const unsigned char len, const Bytes datablock);
    void setValidPasswordHashBytes(const Bytes validBytes);
    void calcHeaderBytes(const Bytes passwordhash);
    Bytes getHeaderBytes() const;
};


#endif //DATAHEADER_H