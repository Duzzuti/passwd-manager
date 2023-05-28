/*
this file contains the implementations of Data header class
*/
#include "dataheader.h"
#include "utility.h"
#include "rng.h"
#include "file_modes.h"

DataHeader::DataHeader(const HModes hash_mode){
    //initialize the hash mode
    this->dh.hash_mode = hash_mode;
    Hash* hash = HashModes::getHash(hash_mode);
    this->hash_size = hash->getHashSize();      //gets the hash size of the hash that corresponds to the given mode
    delete hash;
}

bool DataHeader::isComplete() const noexcept{
    //checks if the dataheader has everything set
    if(this->dh.chainhash1_mode == 0 || this->dh.chainhash2_mode == 0 || 
        this->dh.valid_passwordhash.getLen() != this->hash_size || !FileModes::isModeValid(this->dh.file_mode)){
        return false;
    }
    return true;
}

unsigned int DataHeader::getHeaderLength() const noexcept{
    //gets the header len, if there is not header set try to calculate the len, else return 0
    if(this->header_bytes.getLen() > 0){
        return this->header_bytes.getLen();     //header bytes are set, so we get this length
    }
    if(this->dh.chainhash1_mode != 0 && this->dh.chainhash2_mode != 0){   //all data set to calculate the header length
        return 22 + 2*this->hash_size + this->dh.chainhash1_datablock_len + this->dh.chainhash2_datablock_len;    //dataheader.md
    }else{
        return 0;   //not enough infos to get the header length
    }
}

void DataHeader::setChainHash1(const CHModes mode, const u_int64_t iters, const unsigned char len, const ChainHashData datablock){
    //sets the information about the first chainhash
    if(len != datablock.getDataBlock().getLen()){  //validates the datablock length
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    if(!ChainHashModes::isChainHashValid(mode, iters, datablock)){
        //validates the chainhash
        throw std::invalid_argument("given data is not valid");
    }
    //set the information to the object
    this->dh.chainhash1_mode = mode;
    this->dh.chainhash1_datablock = datablock;
    this->dh.chainhash1_datablock_len = len;
    this->dh.chainhash1_iters = iters;
}

void DataHeader::setChainHash2(const CHModes mode, const u_int64_t iters, const unsigned char len, const ChainHashData datablock){
    //sets the information about the second chainhash
    if(len != datablock.getDataBlock().getLen()){  //validates the datablock length
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    if(!ChainHashModes::isChainHashValid(mode, iters, datablock)){
        //validates the chainhash
        throw std::invalid_argument("given data is not valid");
    }
    //set the information to the object
    this->dh.chainhash2_mode = mode;
    this->dh.chainhash2_datablock = datablock;
    this->dh.chainhash2_datablock_len = len;
    this->dh.chainhash2_iters = iters;
}

void DataHeader::setFileDataMode(const FModes file_mode){
    //sets the file data mode
    this->dh.file_mode = file_mode;
}

void DataHeader::setValidPasswordHashBytes(const Bytes validBytes){
    //set the passwordhash validator hash
    if(validBytes.getLen() != this->hash_size){ //check if the hash size is right
        throw std::length_error("Length of the given validBytes does not match with the hash size");
    }
    this->dh.valid_passwordhash = validBytes;
}

Bytes DataHeader::getHeaderBytes() const{
    //gets the header bytes
    if(this->getHeaderLength() == 0){
        //no header is set and there are missing information
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    if(this->getHeaderLength() != this->header_bytes.getLen()){
        //current header has not the length that it should have (current header is out of date)
        throw std::logic_error("the calculated header length is not equal to the set header length. Call calcHeaderBytes() first");
    }
    return this->header_bytes;
}

void DataHeader::calcHeaderBytes(const Bytes passwordhash, const bool verify_pwhash){
    //calculates the header
    if(!this->isComplete()){
        //header bytes cannot be calculated (data is missing)
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    if(verify_pwhash){
        //verifies the given pwhash with the currently set validator
        Hash* hash = HashModes::getHash(this->dh.hash_mode);   //gets the right hash function
        //is the chainhash from the given hash equal to the validator
        const bool isOkay = (this->dh.valid_passwordhash == ChainHashModes::performChainHash(
            this->dh.chainhash2_mode, this->dh.chainhash2_iters, this->dh.chainhash2_datablock, hash, passwordhash));
        delete hash;
        if(!isOkay){
            //given pwhash is not valid
            throw std::invalid_argument("provided passwordhash is not valid (against validator)");
        }
    }
    //saves the expected length to validate the result
    //need to clear header bytes to calculate the length. If it is not clear getHeaderLength() may return the current length
    this->header_bytes = Bytes();
    unsigned int len = this->getHeaderLength();

    Bytes dataheader = Bytes();
    Bytes tmp = Bytes();
    dataheader.addByte(this->dh.file_mode);                //add file mode byte
    dataheader.addByte(this->dh.hash_mode);                //add hash mode byte
    dataheader.addByte(this->dh.chainhash1_mode);          //add first chainhash mode byte
    tmp.setBytes(LongToCharVec(this->dh.chainhash1_iters));
    dataheader.addBytes(tmp);                           //add iterations for the first chainhash
    dataheader.addByte(this->dh.chainhash1_datablock_len); //add datablock length byte
    dataheader.addBytes(this->dh.chainhash1_datablock.getDataBlock());    //add first datablock
    dataheader.addByte(this->dh.chainhash2_mode);          //add second chainhash mode
    tmp.setBytes(LongToCharVec(this->dh.chainhash2_iters));
    dataheader.addBytes(tmp);                           //add iterations for the second chainhash
    dataheader.addByte(this->dh.chainhash2_datablock_len); //add datablock length byte
    dataheader.addBytes(this->dh.chainhash2_datablock.getDataBlock());    //add second datablock
    dataheader.addBytes(this->dh.valid_passwordhash);      //add password validator
    tmp.setBytes(RNG::get_random_bytes(this->hash_size));   //generate the salt with random bytes
    tmp = tmp + passwordhash;       //encrypt the salt with the password hash
    this->dh.enc_salt = tmp;           //set the encrypted salt
    dataheader.addBytes(this->dh.enc_salt);                //add encrypted salt
    if(dataheader.getLen() != len){     //checks if the length is equal to the expected length
        throw std::logic_error("calculated header has not the expected length");
    }
    this->header_bytes = dataheader;
}

DataHeaderParts DataHeader::getDataHeaderParts() const{
    //getter for the dataheader parts
    if(!this->isComplete()){
        //dataheader parts are not ready
        throw std::logic_error("Cannot get DataHeaderParts because its not complete");
    }
    return this->dh;
}

