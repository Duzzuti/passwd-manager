#include "dataHeader.h"

DataHeader::DataHeader(unsigned char const hash_mode){
    this->hash_mode = hash_mode;
    if(!HashModes::isModeValid(hash_mode)){
        std::cout << "ERROR: file is corupted and cannot be read " << std::endl;
        std::cout << "The given hash mode of the file is not valid (" << +hash_mode << ")" << std::endl;
        std::cout << "Update the application, correct the mode byte in the file or try a backup file you have made" << std::endl;
        throw std::runtime_error("Cannot read data header. Invalid hash mode");
    }
    Hash* hash = HashModes::getHash(hash_mode);
    this->hash_size = hash->getHashSize();
    delete hash;
}

unsigned int DataHeader::getHeaderLength() const noexcept{
    if(this->header_bytes.getLen() > 0){
        return this->header_bytes.getLen();     //header bytes are set, so we get this length
    }
    if(this->chainhash1_mode != 0 && this->chainhash2_mode != 0){   //all data set to calculate the header length
        return 21 + 2*this->hash_size + this->chainhash1_datablock_len + this->chainhash2_datablock_len;    //dataheader.md
    }else{
        return 0;   //not enough infos to get the header length
    }
}

void DataHeader::setChainHash1(unsigned char mode, unsigned long iters, unsigned char len, Bytes datablock){
    if(len != datablock.getLen()){
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    if(!ChainHashModes::isChainHashValid(mode, iters, datablock)){
        throw std::invalid_argument("given data is not valid");
    }
    this->chainhash1_mode = mode;
    this->chainhash1_datablock = datablock;
    this->chainhash1_datablock_len = len;
    this->chainhash1_iters = iters;
}

void DataHeader::setValidPasswordHashBytes(Bytes validBytes){
    if(validBytes.getLen() != this->hash_size){
        throw std::length_error("Length of the given validBytes does not match with the hash size");
    }
    this->valid_passwordhash = validBytes;
}

void DataHeader::setChainHash2(unsigned char mode, unsigned long iters, unsigned char len, Bytes datablock){
    if(len != datablock.getLen()){
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    if(!ChainHashModes::isChainHashValid(mode, iters, datablock)){
        throw std::invalid_argument("given data is not valid");
    }
    this->chainhash2_mode = mode;
    this->chainhash2_datablock = datablock;
    this->chainhash2_datablock_len = len;
    this->chainhash2_iters = iters;
}
