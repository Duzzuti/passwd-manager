/*
implements the FileHandler class
*/

#include "filehandler.h"

#include <fstream>

#include "logger.h"
#include "utility.h"

const std::string FileHandler::extension = ".enc";

ErrorStruct<bool> FileHandler::isValidPath(const std::filesystem::path& file, bool should_exist) noexcept {
    // checks if the given file path is valid (file_path has to be not empty and have the right extension)
    // if should_exist is true, the file has to exist otherwise it has to not exist
    ErrorStruct<bool> err;
    err.success = SuccessType::FAIL;
    err.errorInfo = file.c_str();
    if (file.empty()) {
        // the given path is empty
        PLOG_ERROR << "The given path is empty (file path: " << file << ")";
        err.errorCode = ErrorCode::ERR_EMPTY_FILEPATH;
        return err;
    }
    if (file.extension() != FileHandler::extension) {
        // the given path does not have the right extension
        PLOG_ERROR << "The given path does not have the right extension (file path: " << file << ", file path extension: " << file.extension() << ")";
        err.errorCode = ErrorCode::ERR_EXTENSION_INVALID;
        return err;
    }
    if (should_exist) {
        if (!std::filesystem::exists(file)) {
            // the given path does not exist
            PLOG_ERROR << "The given path does not exist (file path: " << file << ")";
            err.errorCode = ErrorCode::ERR_FILE_NOT_FOUND;
            return err;
        }
        std::ifstream file_stream(file);  // create the file stream
        if (!file_stream) {
            // cannot open the file
            PLOG_ERROR << "Cannot open the file (file path: " << file << ")";
            err.errorCode = ErrorCode::ERR_FILE_NOT_OPEN;
            return err;
        }
    } else {
        if (std::filesystem::exists(file)) {
            // the given path already exists
            PLOG_ERROR << "The given path already exists (file path: " << file << ")";
            err.errorCode = ErrorCode::ERR_FILE_EXISTS;
            return err;
        }
    }
    // all checks passed
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> FileHandler::createFile(const std::filesystem::path& file) noexcept {
    // creates a new .enc file at the given path and validates it
    PLOG_VERBOSE << "Creating new file (file_path: " << file << ")";
    ErrorStruct<bool> err;
    err.success = SuccessType::FAIL;
    // check if the file path is valid and the file does not yet exist
    ErrorStruct<bool> err2 = FileHandler::isValidPath(file, false);
    if (!err2.isSuccess()) {
        // the file path is invalid
        PLOG_ERROR << "The file path is invalid (createFile) (errorCode: " << +err2.errorCode << ", errorInfo: " << err2.errorInfo << ", what: " << err2.what << ")";
        return err2;
    }
    // create the file
    std::ofstream file_stream{file.c_str()};
    if (!file_stream.is_open() || !file_stream) {
        // the file could not be created
        PLOG_ERROR << "The file could not be created (createFile)";
        err.errorCode = ErrorCode::ERR_FILE_NOT_CREATED;
        err.errorInfo = file.c_str();
        return err;
    }
    file_stream.close();
    return ErrorStruct<bool>{true};
}

FileHandler::FileHandler(const std::filesystem::path& file) : filepath(file) {
    // sets the filepath
    if (!FileHandler::isValidPath(file, true).isSuccess()) {
        PLOG_FATAL << "The given file path is invalid (file path: " << file << ")";
        throw std::runtime_error("The given file path is invalid (file path: " + file.string() + ")");
    }
    this->update();
}

void FileHandler::update() {
    std::ifstream filestream = this->getFileStream();
    std::streampos begin = filestream.tellg();
    filestream.seekg(0, std::ios::end);
    std::streampos end = filestream.tellg();
    this->file_size = end - begin;
    this->empty = (this->file_size == 0);
    filestream.seekg(begin);
    Bytes tmp(8);
    if (!readData(filestream, tmp, 8)) {
        if (this->file_size == 0)
            this->header_size = 0;
        else {
            PLOG_ERROR << "The file is too small to contain a data header but is not empty";
            throw std::length_error("The file is too small to contain a data header but is not empty");
        }
    } else {
        if (tmp.toLong() != this->file_size) {
            PLOG_ERROR << "The file size does not match with the file size in the data header";
            throw std::logic_error("The file size does not match with the file size in the data header");
        }
        tmp.setLen(0);  // reset the length
        if (!readData(filestream, tmp, 8)) {
            PLOG_ERROR << "The file is too small to contain a data header but is not empty";
            throw std::length_error("The file is too small to contain a data header but is not empty");
        }
        this->header_size = tmp.toLong();
    }
    filestream.close();
    if (this->header_size > this->file_size) {
        PLOG_ERROR << "The file size is smaller than the given header size";
        throw std::logic_error("The file size is smaller than the given header size");
    }
    if (this->header_size < MIN_DATAHEADER_LEN && !(this->header_size == 0 && this->file_size == 0)) {
        PLOG_ERROR << "The file is too small to contain a data header but is not empty";
        throw std::length_error("The file is too small to contain a data header but is not empty");
    }
}

ErrorStruct<bool> FileHandler::isDataHeader(FModes exp_file_mode) noexcept {
    ErrorStruct<std::unique_ptr<DataHeader>> err = this->getDataHeader();
    if (!err.isSuccess()) {
        // the data header could not be read
        PLOG_ERROR << "The data header could not be read (isDataHeader) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
        return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
    } else if (err.returnRef()->getDataHeaderParts().getFileDataMode() == exp_file_mode) {
        // the data header was read successfully and the file mode is correct
        return ErrorStruct<bool>{true};
    }
    PLOG_WARNING << "The data header was read successfully but the file mode is incorrect (isDataHeader) (file path: " << this->filepath << ", expected file mode: " << +exp_file_mode
                 << ", actual file mode: " << +err.returnRef()->getDataHeaderParts().getFileDataMode() << ")";
    return ErrorStruct<bool>{FAIL, ERR_FILEMODE_INVALID, std::to_string(+err.returnRef()->getDataHeaderParts().getFileDataMode())};
}

bool FileHandler::isEmtpy() const noexcept { return this->empty; }

ErrorStruct<std::unique_ptr<DataHeader>> FileHandler::getDataHeader() noexcept {
    // reads the data header from the file
    std::ifstream filestream = this->getFileStream();
    ErrorStruct<std::unique_ptr<DataHeader>> ret = DataHeader::setHeaderBytes(filestream);
    filestream.close();
    return ret;
}

std::ifstream FileHandler::getFileStream() const noexcept {
    // returns the file stream
    return std::ifstream(this->filepath.c_str(), std::ios::binary);
}

std::ifstream FileHandler::getDataStream() const noexcept {
    // reads the data from the file (without the data header)
    std::ifstream file(this->filepath.c_str(), std::ios::binary);
    file.seekg(this->header_size, std::ios::beg);
    return file;
}

size_t FileHandler::getHeaderSize() const noexcept { return this->header_size; }

size_t FileHandler::getFileSize() const noexcept { return this->file_size; }

size_t FileHandler::getDataSize() const noexcept { return this->file_size - this->header_size; }

// ErrorStruct<bool> FileHandler::writeBytes(Bytes& bytes) noexcept {
//     // writes bytes to the file
//     // overrides old content
//     PLOG_VERBOSE << "Writing to file (file_path: " << this->filepath.c_str() << ")";
//     this->header_size = 0;
//     this->file_size = 0;
//     ErrorStruct<std::ofstream> err = this->getWriteStream();
//     if (!err.isSuccess()) {
//         PLOG_ERROR << "The file could not be opened (writeBytes) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
//         return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
//     }
//     std::ofstream file = err.returnMove();
//     // write the data
//     file.write(reinterpret_cast<char*>(bytes.getBytes()), bytes.getLen());
//     file.close();
//     this->header_size = 0;
//     PLOG_VERBOSE << "Successfully wrote " << bytes.getLen() << " bytes to file (file_path: " << this->filepath.c_str() << ")";
//     return ErrorStruct<bool>{true};
// }

ErrorStruct<std::ofstream> FileHandler::getWriteStream() noexcept {
    this->header_size = 0;
    this->file_size = 0;
    // returns a write stream to the file
    // overrides old content
    PLOG_VERBOSE << "Getting write stream to file (file_path: " << this->filepath.c_str() << ")";
    // checks if the selected file exists
    ErrorStruct<bool> err_file = this->isValidPath(this->filepath, true);
    if (!err_file.isSuccess()) {
        PLOG_ERROR << "The provided file path is invalid (getWriteStream) (errorCode: " << +err_file.errorCode << ", errorInfo: " << err_file.errorInfo << ", what: " << err_file.what << ")";
        return ErrorStruct<std::ofstream>{err_file.success, err_file.errorCode, err_file.errorInfo, err_file.what};
    }
    // file path is valid
    std::ofstream file(this->filepath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        PLOG_FATAL << "The file could not be opened";
        // should not happen because the file was checked before
        return ErrorStruct<std::ofstream>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_OPEN, this->filepath.c_str()};
    }
    return ErrorStruct<std::ofstream>::createMove(std::move(file));
    ;
}

ErrorStruct<std::ofstream> FileHandler::getWriteStreamIfEmpty() noexcept {
    // returns a write stream to the file if it is empty
    // overrides old content
    if (this->isEmtpy()) {
        return this->getWriteStream();
    } else {
        PLOG_WARNING << "The file is not empty (file_path: " << this->filepath.c_str() << ")";
        return ErrorStruct<std::ofstream>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_EMPTY, this->filepath.c_str()};
    }
}

std::filesystem::path FileHandler::getPath() const noexcept { return this->filepath; }

// ErrorStruct<bool> FileHandler::writeBytesIfEmpty(Bytes& bytes) noexcept {
//     // writes bytes to the file if the file is empty
//     try {
//         if (!this->isEmtpy()) {
//             // the file is not empty
//             PLOG_WARNING << "The file is not empty (file_path: " << this->filepath.c_str() << ")";
//             return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_EMPTY, this->filepath.c_str()};
//         }
//     } catch (std::exception& e) {
//         PLOG_ERROR << "Some error occurred while checking the file data (" << e.what() << ")";
//         return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR, this->filepath.c_str(), e.what()};
//     }
//     // the file is empty
//     return this->writeBytes(bytes);
// }

Bytes FileHandler::getAllBytes() const {
    // reads all Bytes from the file
    PLOG_VERBOSE << "Reading all Bytes from file (file_path: " << this->filepath.c_str() << ")";
    std::ifstream file = this->getFileStream();  // create the file stream
    if (!file) {
        // the set encryption file does not exist on the system
        throw std::runtime_error("file cannot be opened");
    }
    // get length of file:
    file.seekg(0, file.end);                  // jump to the end
    std::streampos file_size = file.tellg();  // saves the position
    file.seekg(0, std::ios::beg);

    // gets the Bytes
    Bytes ret(file_size);
    if (!readData(file, ret, file_size)) {
        // not enough characters to read (file is not long enough)
        throw std::length_error("File contains to few characters");
    }
    file.close();
    return ret;
}

Bytes FileHandler::getFirstBytes(const size_t num) const {
    // reads the num first bytes of the encryption file
    std::ifstream file = this->getFileStream();  // create the file stream
    if (!file) {
        // the set encryption file does not exist on the system
        throw std::runtime_error("file cannot be opened");
    }
    Bytes b(num);                   // creates a buffer to hold the read bytes
    if (!readData(file, b, num)) {  // reads into the buffer
        // not enough characters to read (file is not long enough)
        throw std::length_error("File contains to few characters");
    }
    file.close();
    return b;
}
