#pragma once
#ifndef FILEDATA_H
#define FILEDATA_H

#include "bytes.h"

class FileData{
private:
    virtual bool isDataValid(Bytes bytes) noexcept = 0;
public:
    virtual Bytes run(Bytes) = 0;
    virtual std::string getError() = 0;
};

#endif //FILEDATA_H