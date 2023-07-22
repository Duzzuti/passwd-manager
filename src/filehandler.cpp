/*
implements the FileHandler class
*/

#include "filehandler.h"

#include <fstream>

#include "logger.h"

const std::string FileHandler::extension = ".enc";

ErrorStruct<bool> FileHandler::isValidPath(std::filesystem::path file, bool should_exist) noexcept {
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
    } else {
        if (std::filesystem::exists(file)) {
            // the given path already exists
            PLOG_ERROR << "The given path already exists (file path: " << file << ")";
            err.errorCode = ErrorCode::ERR_FILE_EXISTS;
            return err;
        }
    }
    std::ifstream file_stream(file);  // create the file stream
    if (!file_stream) {
        // cannot open the file
        PLOG_ERROR << "Cannot open the file (file path: " << file << ")";
        err.errorCode = ErrorCode::ERR_FILE_NOT_OPEN;
        return err;
    }
    // all checks passed
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> FileHandler::createFile(std::filesystem::path file) noexcept {
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
    std::ofstream file_stream{file};
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

FileHandler::FileHandler(std::filesystem::path file) {
    // sets the filepath
    if (FileHandler::isValidPath(file, true).isSuccess()) {
        this->filepath = file;
        return;
    }
    PLOG_FATAL << "The given file path is invalid (file path: " << file << ")";
    throw std::runtime_error("The given file path is invalid (file path: " + file.string() + ")");
}

bool FileHandler::isDataHeader(FModes exp_file_mode) const noexcept {
    ErrorStruct<DataHeader> err = this->getDataHeader();
    if (!err.isSuccess()) {
        // the data header could not be read
        PLOG_ERROR << "The data header could not be read (isDataHeader) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
        return false;
    } else if (err.returnValue().getDataHeaderParts().file_mode == exp_file_mode) {
        // the data header was read successfully and the file mode is correct
        return true;
    }
    PLOG_WARNING << "The data header was read successfully but the file mode is incorrect (isDataHeader) (file path: " << this->filepath << ", expected file mode: " << exp_file_mode
                 << ", actual file mode: " << err.returnValue().getDataHeaderParts().file_mode << ")";
    return false;
}

bool FileHandler::isEmtpy() const noexcept {
    // checks if the file is empty
    try {
        // try to read the first byte
        this->getFirstBytes(1);
    } catch (std::length_error& e) {
        // the file is empty
        return true;
    }
    return false;
}

ErrorStruct<DataHeader> FileHandler::getDataHeader() const noexcept {
    // reads the data header from the file
    Bytes content = this->getAllBytes();
    return DataHeader::setHeaderBytes(content);
}

ErrorStruct<Bytes> FileHandler::getData() const noexcept {
    // reads the data from the file (without the data header)
    Bytes content = this->getAllBytes();
    ErrorStruct<DataHeader> err = DataHeader::setHeaderBytes(content);
    if (!err.isSuccess()) {
        // the data header could not be read
        PLOG_ERROR << "The data header could not be read (getData) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
        return ErrorStruct<Bytes>{err.success, err.errorCode, err.errorInfo, err.what};
    }
    // the data header was read successfully
    // the remaining bytes are the data
    return ErrorStruct<Bytes>{content};
}

ErrorStruct<bool> FileHandler::writeBytes(Bytes bytes) const noexcept {
    // writes bytes to the file
    // overrides old content
    PLOG_VERBOSE << "Writing to file (file_path: " << this->filepath << ")";
    // checks if the selected file exists
    ErrorStruct<bool> err_file = this->isValidPath(this->filepath, true);
    if (!err_file.isSuccess()) {
        PLOG_ERROR << "The provided file path is invalid (writeBytes) (errorCode: " << +err_file.errorCode << ", errorInfo: " << err_file.errorInfo << ", what: " << err_file.what << ")";
        return err_file;
    }
    // file path is valid
    std::ofstream file(this->filepath, std::ios::binary);
    if (!file.is_open()) {
        PLOG_FATAL << "The file could not be opened";
        // should not happen because the file was checked before
        return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_OPEN, this->filepath.c_str()};
    }
    // write the data
    unsigned char* data = new unsigned char[bytes.getLen()];
    bytes.getBytesArray(data);
    file.write(reinterpret_cast<char*>(data), sizeof(reinterpret_cast<char*>(data)));
    file.close();

    delete[] data;
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> FileHandler::writeBytesIfEmpty(Bytes bytes) const noexcept {
    // writes bytes to the file if the file is empty
    if (!this->isEmtpy()) {
        // the file is not empty
        PLOG_WARNING << "The file is not empty (writeBytesIfEmpty) (file_path: " << this->filepath << ")";
        return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_EMPTY, this->filepath.c_str()};
    }
    // the file is empty
    return this->writeBytes(bytes);
}

Bytes FileHandler::getAllBytes() const {
    // reads all Bytes from the file
    std::ifstream file(this->filepath.c_str());  // create the file stream
    if (!file) {
        // the set encryption file does not exist on the system
        throw std::runtime_error("file cannot be opened");
    }
    // get length of file:
    file.seekg(0, file.end);  // jump to the end
    int num = file.tellg();   // saves the position
    file.close();

    // gets the Bytes
    char buf[num];        // creates a buffer to hold the read bytes
    file.read(buf, num);  // reads into the buffer
    // transform the char array into an Bytes object
    Bytes ret;
    for (char c : buf) {
        ret.addByte(static_cast<unsigned char>(c));  // cast from char to unsigned char
    }
    return ret;
}

Bytes FileHandler::getFirstBytes(const int num) const {
    // reads the num first bytes of the encryption file
    std::ifstream file(this->filepath.c_str());  // create the file stream
    if (!file) {
        // the set encryption file does not exist on the system
        throw std::runtime_error("file cannot be opened");
    }
    // get length of file:
    file.seekg(0, file.end);    // jump to the end
    int length = file.tellg();  // saves the position
    file.seekg(0, file.beg);    // jumps to the start again
    if (length < num) {
        // not enough characters to read (file is not long enough)
        throw std::length_error("File contains to few characters");
    }
    char buf[num];        // creates a buffer to hold the read bytes
    file.read(buf, num);  // reads into the buffer
    // transform the char array into an Bytes object
    Bytes ret;
    for (char c : buf) {
        ret.addByte(static_cast<unsigned char>(c));  // cast from char to unsigned char
    }
    return ret;
}
