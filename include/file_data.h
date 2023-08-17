#pragma once

#include <optional>

#include "base.h"
#include "bytes.h"
#include "error.h"
#include "file_modes.h"

struct FileDataStruct {
    /*
    this struct is used to construct the FileData objects.
    These objects are used to store the data of the file in a way that the user can interact with it
    */
   private:
    std::optional<FModes> file_mode;

   public:
    Bytes dec_data;

    FileDataStruct() = default;
    FileDataStruct(FModes file_mode, Bytes dec_data) {
        // constructs the FileDataStruct object
        this->setFileMode(file_mode);
        this->dec_data = dec_data;
    }
    bool isFileModeSet() const noexcept {
        // checks if the file mode is set
        return file_mode.has_value();
    }
    FModes getFileMode() const {
        // gets the file mode
        if (this->file_mode.has_value())
            return this->file_mode.value();
        else {
            PLOG_ERROR << "file mode is not set";
            throw std::runtime_error("file mode is not set");
        }
    }
    void setFileMode(FModes file_mode) {
        // sets the file data mode
        if (FileModes::isModeValid(file_mode))
            this->file_mode = file_mode;
        else {
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::invalid_argument("file mode is not valid");
        }
    }

    bool isComplete() const noexcept {
        // checks if the FileDataStruct object is complete
        if (!this->isFileModeSet()) {
            PLOG_WARNING << "file mode is not completed";
            return false;
        }
        return true;
    }
};

class FileData {
    /*
    Interface class that holds methods that every FileData class has to implement
    A FileData class is a class that implements this interface. It is used for the semantic of the data
    If we decrypted the file's data, the user can do different actions dependent on the type of data that is stored in the file
    */
   public:
    // checks if the decrypted data is valid for it's use case. Checks if the file mode is correct
    // formats the byte data into the right datatypes (like maps and lists)
    virtual ErrorStruct<bool> constructFileData(FileDataStruct& file_data) noexcept = 0;

    // returns a file data struct that contains the current data of the file
    virtual FileDataStruct getFileData() const = 0;

    // returns the file mode that corresponds to the file data object
    virtual FModes getFileMode() const noexcept = 0;

    virtual ~FileData(){};
};
