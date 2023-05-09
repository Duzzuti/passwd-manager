#pragma once
#ifndef DATAHEADER_H
#define DATAHEADER_H

#include "bytes.h"
#include "modes.h"

class DataHeader{
private:
    unsigned char mode;   //the mode that is choosen
    int bytesLen;
    Bytes header_bytes;           //bytes that are used for the mode

public:
    DataHeader(unsigned char const mode);
    void setHeaderBytes(Bytes headerBytes);
    int getHeaderLength() const noexcept;
};


#endif //DATAHEADER_H