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
    std::string format;
    std::vector<Bytes> data_parts;
    std::vector<NameLen> name_lens;
private:
    void clear() noexcept;
public:
    ChainHashData() = default;
    ChainHashData(std::string format);

    bool isComplete() const noexcept;
    bool isCompletedFormat(std::string format) const noexcept;

    unsigned char getPartsNumber() const noexcept;
    Bytes getDataBlock() const;
    Bytes getPart(std::string data_name) const;
    
    void addBytes(Bytes bytes);

};

#endif //CHAINHASHDATA_H