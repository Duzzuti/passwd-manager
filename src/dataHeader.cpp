#include "dataHeader.h"
#include "utility.h"
#include "rng.h"
#include "file_modes.h"

DataHeader::DataHeader(unsigned char const hash_mode){
    if(!HashModes::isModeValid(hash_mode)){
        std::cout << "ERROR: file is corupted and cannot be read " << std::endl;
        std::cout << "The given hash mode of the file is not valid (" << +hash_mode << ")" << std::endl;
        std::cout << "Update the application, correct the mode byte in the file or try a backup file you have made" << std::endl;
        throw std::runtime_error("Cannot read data header. Invalid hash mode");
    }
    this->hash_mode = hash_mode;
    Hash* hash = HashModes::getHash(hash_mode);
    this->hash_size = hash->getHashSize();
    delete hash;
}

unsigned int DataHeader::getHeaderLength() const noexcept{
    if(this->header_bytes.getLen() > 0){
        return this->header_bytes.getLen();     //header bytes are set, so we get this length
    }
    if(this->chainhash1_mode != 0 && this->chainhash2_mode != 0){   //all data set to calculate the header length
        return 22 + 2*this->hash_size + this->chainhash1_datablock_len + this->chainhash2_datablock_len;    //dataheader.md
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

void DataHeader::setFileDataMode(unsigned char const file_mode){
    if(!FileModes::isModeValid(file_mode)){
        std::cout << "ERROR: file is corupted and cannot be read " << std::endl;
        std::cout << "The given file mode of the file is not valid (" << +file_mode << ")" << std::endl;
        std::cout << "Update the application, correct the mode byte in the file or try a backup file you have made" << std::endl;
        throw std::runtime_error("Cannot read data header. Invalid file mode");
    }
    this->file_mode = file_mode;
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

Bytes DataHeader::getHeaderBytes() const{
    if(this->getHeaderLength() == 0){
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    if(this->getHeaderLength() != this->header_bytes.getLen()){
        throw std::logic_error("the calculated header length is not equal to the set header length. Call calcHeaderBytes() first");
    }
    return this->header_bytes;
}

void DataHeader::calcHeaderBytes(){
    if(this->chainhash1_mode == 0 || this->chainhash2_mode == 0 || 
        this->valid_passwordhash.getLen() != this->hash_size || !FileModes::isModeValid(this->file_mode)){
        //header bytes cannot be calculated (data is missing)
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    //saves the expected length to validate the result
    this->header_bytes = Bytes();
    unsigned int len = this->getHeaderLength();

    Bytes dataheader = Bytes();
    Bytes tmp = Bytes();
    dataheader.addByte(this->file_mode);
    dataheader.addByte(this->hash_mode);
    dataheader.addByte(this->chainhash1_mode);
    tmp.setBytes(LongToCharVec(this->chainhash1_iters));
    dataheader.addBytes(tmp);
    dataheader.addByte(this->chainhash1_datablock_len);
    dataheader.addBytes(this->chainhash1_datablock);
    dataheader.addByte(this->chainhash2_mode);
    tmp.setBytes(LongToCharVec(this->chainhash2_iters));
    dataheader.addBytes(tmp);
    dataheader.addByte(this->chainhash2_datablock_len);
    dataheader.addBytes(this->chainhash2_datablock);
    dataheader.addBytes(this->valid_passwordhash);
    tmp.setBytes(RNG::get_random_bytes(this->hash_size));   //generate the salt
    dataheader.addBytes(tmp);
    if(dataheader.getLen() != len){
        throw std::logic_error("calculated header has not the expected length");
    }
    this->header_bytes = dataheader;
}
