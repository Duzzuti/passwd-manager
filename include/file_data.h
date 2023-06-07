#pragma once

#include "base.h"
#include "bytes.h"
#include "error.h"

struct FileDataStruct {
    /*
    this struct is used to construct the FileData objects.
    These objects are used to store the data of the file in a way that the user can interact with it
    */
    FModes file_mode;
    Bytes dec_data;
};

class FileData {
    /*
    Interface class that holds methods that every FileData class has to implement
    A FileData class is a class that implements this interface. It is used for the semantik of the data
    If we decrypted the file's data, the user can do different actions dependend on the type of data that is stored in the file
    */
   public:
    // checks if the decrypted data is valid for it's use case. Checks if the file mode is correct
    // formats the byte data into the right datatypes (like maps and lists)
    virtual ErrorStruct<bool> constructFileData(FileDataStruct file_data) noexcept = 0;

    // returns a file data struct that contains the current data of the file
    virtual FileDataStruct getFileData() const noexcept = 0;

    // returns the file mode that corresponds to the file data object
    virtual FModes getFileMode() const noexcept = 0;

    virtual ~FileData(){};
};
