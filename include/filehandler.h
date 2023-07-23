#pragma once

#include <filesystem>

#include "base.h"
#include "bytes.h"
#include "dataheader.h"

class FileHandler {
    /*
    this class handles the files
    That means it writes and reads from a given enc file
    */
   private:
    std::filesystem::path filepath;  // stores the path to the encryption file
   public:
    static const std::string extension;  // encryption files (.enc)
   public:
    static ErrorStruct<bool> isValidPath(std::filesystem::path file, bool should_exist) noexcept;  // checks if the path is valid
    static ErrorStruct<bool> createFile(std::filesystem::path file) noexcept;                      // creates a file
    FileHandler(std::filesystem::path file);                                                       // sets the filepath
    ErrorStruct<bool> isDataHeader(FModes exp_file_mode) const noexcept;                           // checks if the file has a valid data header
    bool isEmtpy() const;                                                                          // checks if the file is empty
    ErrorStruct<DataHeader> getDataHeader() const noexcept;                                        // reads the data header from the file
    ErrorStruct<Bytes> getData() const noexcept;                                                   // reads the data from the file (without the data header)
    Bytes getFirstBytes(const int num) const;                                                      // reads the first num Bytes from the encryption file
    Bytes getAllBytes() const;                                                                     // reads all Bytes from the given encryption file
    ErrorStruct<bool> writeBytesIfEmpty(Bytes bytes) const noexcept;                               // writes the given bytes to the file if it is empty
    ErrorStruct<bool> writeBytes(Bytes bytes) const noexcept;                                      // writes the given bytes to the file
    std::filesystem::path getPath() const noexcept;                                                // returns the filepath
};