#pragma once
#ifndef CHAINHASHDATA_H
#define CHAINHASHDATA_H

#include "bytes.h"

struct NameLen{
public:
    std::string name;
    unsigned char len;
    NameLen(std::string name, unsigned char len){
        this->name = name;
        this->len = len;
    }
};

class ChainHashData{
private:
    std::vector<Bytes> data_parts;
    std::vector<NameLen> name_lens;
private:
    void clear() noexcept;
public:
    ChainHashData(std::string format);

    bool isComplete() const noexcept;

    unsigned char getPartsNumber() const noexcept;
    Bytes getDataBlock() const;
    
    void addBytes(Bytes bytes);

};

#endif //CHAINHASHDATA_H