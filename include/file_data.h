#pragma once
#ifndef FILEDATA_H
#define FILEDATA_H

#include "bytes.h"

class FileData{
    virtual Bytes run(Bytes) = 0;
};

#endif //FILEDATA_H