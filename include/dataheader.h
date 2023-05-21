#pragma once
#ifndef DATAHEADER_H
#define DATAHEADER_H

#include "bytes.h"
#include "hash_modes.h"
#include "chainhash_modes.h"
#include "chainhash_data.h"

class DataHeader{
    /*
    this class stores the functionalities of the dataheader
    the header is some byte information about the file that is written at the beginning of the file
    the programm can read this header and knows everything about the encryption that is used
    this DataHeader class is handling this information
    more information about the header: docs/dataheader.md
    */
private:
    unsigned char file_mode;   //the file data mode that is choosen (content of the file)
    const unsigned char hash_mode;   //the hash mode that is choosen (hash function)
    unsigned char hash_size;    //the size of the hash provided by the hash function (in Bytes)
    CHModes chainhash1_mode;  //chainhash mode for the first chainhash (password -> passwordhash)
    CHModes chainhash2_mode;  //chainhash mode for the second chainhash (passwordhash -> validate password)
    u_int64_t chainhash1_iters; //iterations for the first chainhash
    u_int64_t chainhash2_iters; //iterations for the second chainhash
    unsigned char chainhash1_datablock_len; //the length of the first datablock
    unsigned char chainhash2_datablock_len; //the length of the second datablock
    ChainHashData chainhash1_datablock;     //the first datablock
    ChainHashData chainhash2_datablock;     //the second datablock
    Bytes valid_passwordhash;       //saves the hash that should be the result of the second chainhash
    Bytes enc_salt;                 //saves the encoded salt
    Bytes header_bytes;             //bytes that are in the header

public:
    //sets up the data header with a hash mode which is necessary to
    //calculate the length of the header, get the hash function and it is also const and cannot be changed anyways
    DataHeader(const unsigned char hash_mode);
    //setter for the data that is needed for the header
    void setFileDataMode(const unsigned char file_mode);    //sets the file data mode (semantik of content)
    //setter for the chainhashs, takes a mode, number of iters, a datablock length (to verify datablock) and the datablock
    //which conains data for the chainhash
    void setChainHash1(const CHModes mode, const u_int64_t iters, const unsigned char len, const ChainHashData datablock);
    void setChainHash2(const CHModes mode, const u_int64_t iters, const unsigned char len, const ChainHashData datablock);
    void setValidPasswordHashBytes(const Bytes validBytes); //sets the passwordhashhash to validate the password hash
    //calculates the header bytes with all information that is set, throws if not enough information is set (or not valid)
    //verifies the pwhash with the previous set pwhash validator
    void calcHeaderBytes(const Bytes passwordhash, const bool verify_pwhash=true);
    Bytes getHeaderBytes() const;   //gets the current set header bytes, calcHeaderBytes overrites this variable

    void setHeaderBytes(const Bytes headerBytes);   //sets the header bytes (if known)
    //gets the length of the currently set header bytes, if its not currently set, we try to calculate the expected len
    //all set data, such as chainhash data etc. are used to calculate this expected len. If the data is not enough we return 0
    unsigned int getHeaderLength() const noexcept;
};


#endif //DATAHEADER_H