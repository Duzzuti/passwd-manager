#pragma once

#include <filesystem>

#include "base.h"
#include "dataheader.h"

class FileHandler {
    /*
    this class handles the files
    That means it writes and reads from a given enc file
    */
   private:
    const std::filesystem::path filepath;  // stores the path to the encryption file
    size_t header_size = 0;                // stores the size of the data header
    size_t file_size = 0;                  // stores the size of the file
    bool empty;                            // stores if the file is empty
   public:
    static const std::string extension;  // encryption files (.enc)
   public:
    static ErrorStruct<bool> isValidPath(const std::filesystem::path& file, bool should_exist) noexcept;  // checks if the path is valid
    static ErrorStruct<bool> createFile(const std::filesystem::path& file) noexcept;                      // creates a file
    FileHandler(const std::filesystem::path& file);                                                       // sets the filepath
    void update();                                                                                        // updates the header_size and file_size variables
    ErrorStruct<bool> isDataHeader(FModes exp_file_mode) noexcept;                                        // checks if the file has a valid data header
    bool isEmtpy() const noexcept;                                                                        // checks if the file is empty
    ErrorStruct<std::unique_ptr<DataHeader>> getDataHeader() noexcept;                                    // reads the data header from the file
    std::ifstream getFileStream() const noexcept;                                                         // returns the file stream
    std::ifstream getDataStream() const noexcept;                                                         // returns the data stream from the file (without the data header)
    size_t getHeaderSize() const noexcept;                                                                // returns the size of the data header
    size_t getFileSize() const noexcept;                                                                  // returns the length of the data in the file (without the data header)

    // NO update() needed
    Bytes getFirstBytes(const size_t num) const;  // reads the first num Bytes from the encryption file
    Bytes getAllBytes() const;                    // reads all Bytes from the given encryption file

    // ErrorStruct<bool> writeBytesIfEmpty(Bytes& bytes) noexcept;                                           // writes the given bytes to the file if it is empty
    // ErrorStruct<bool> writeBytes(Bytes& bytes) noexcept;                                                  // writes the given bytes to the file
    ErrorStruct<std::ofstream> getWriteStream() noexcept;         // returns a write stream to the file
    ErrorStruct<std::ofstream> getWriteStreamIfEmpty() noexcept;  // returns a write stream to the file if it is empty
    std::filesystem::path getPath() const noexcept;               // returns the filepath
};