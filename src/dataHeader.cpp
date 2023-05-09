#include "dataHeader.h"

DataHeader::DataHeader(unsigned char const mode){
    this->mode = mode;
    try{
        this->bytesLen = Modes::getModeBytesLen(this->mode);
    }catch(std::invalid_argument){
        std::cout << "ERROR: file is corupted and cannot be read " << std::endl;
        std::cout << "The given mode of the file is not valid " << std::endl;
        std::cout << "Update the application, correct the mode byte in the file or try a backup file you have made" << std::endl;
        throw std::runtime_error("Cannot read data header");
    }
}

void DataHeader::setHeaderBytes(Bytes headerBytes){
    if(this->getHeaderLength() != headerBytes.getLen()){
        throw std::length_error("Length of the given header is not matching with the expected length from the mode");
    }
    if(this->mode != header_bytes.getBytes()[0]){
        throw std::logic_error("expected mode is not the mode of the encrypted file");
    }
    this->header_bytes = headerBytes;
}

int DataHeader::getHeaderLength() const noexcept{
    return this->bytesLen;
}
