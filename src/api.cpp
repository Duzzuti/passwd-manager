/*
implementation of api.h
*/

#include "api.h"

#include "filehandler.h"

template <typename FData>
std::filesystem::path API<FData>::getCurrentDirPath() noexcept {
    // gets the current directory path
    return std::filesystem::current_path();
}

template <typename FData>
ErrorStruct<std::filesystem::path> API<FData>::getEncDirPath() noexcept {
    // gets the path of the directory where the .enc files are stored at default
    FileHandler fh;
    std::filesystem::path enc_path = fh.getEncryptionFilePath();
    ErrorStruct<std::filesystem::path> err;
    err.returnValue = enc_path;
    err.success = false;
    if (enc_path.empty()) {
        // if the path is empty, the filehandler could not find the encryption dir
        err.errorCode = ERR_EMPTY_FILEPATH;
        err.errorInfo = "Encryption dir";
    } else if (!std::filesystem::exists(enc_path)) {
        // if the path does not exist, the encryption dir does not exist
        err.errorCode = ERR_FILEPATH_INVALID;
        err.errorInfo = enc_path.c_str();
    } else {
        // otherwise the path is valid
        err.success = true;
    }
    return err;
}

template <typename FData>
ErrorStruct<std::vector<std::string>> API<FData>::getAllEncFileNames(std::filesystem::path dir) noexcept {
    // gets all the .enc file names in the given directory
    std::vector<std::string> file_names;
    ErrorStruct<std::vector<std::string>> err;
    err.success = false;
    if (dir.empty()) {
        // the given path is empty
        err.errorCode = ERR_EMPTY_FILEPATH;
        return err;
    }
    if (!std::filesystem::exists(dir)) {
        // the given path does not exist
        err.errorCode = ERR_FILEPATH_INVALID;
        err.errorInfo = dir.c_str();
        return err;
    }
    // iterate over all files in the given directory
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        // if the file is a regular file and has the right extension, add it to the vector
        if (entry.is_regular_file() && entry.path().extension() == FileHandler::extension) {
            file_names.push_back(entry.path().filename().string());
        }
    }
    err.success = true;
    err.returnValue = file_names;
    return err;
}

template <typename FData>
ErrorStruct<std::vector<std::string>> API<FData>::getRelevantFileNames(std::filesystem::path dir) noexcept {
    // only gets the file names that have the same file mode as the given file data
    ErrorStruct<std::vector<std::string>> ret;
    // gets all file names
    ErrorStruct<std::vector<std::string>> err = API<FData>::getAllEncFileNames(dir);
    if (err.success != SUCCESS) {
        return err;
    }
    ret.success = SUCCESS : ret.returnValue = std::vector<std::string>();
    for (int i = 0; i < err.returnValue.size(); i++) {
        // checks for every file if it has the same file mode as the given file data
        // build the complete file path
        std::filesystem::path fp = dir / err.returnValue[i];
        std::ifstream file(fp);
        FileHandler fh;
        if (!fh.setEncryptionFilePath(fp);) {
            // file path is invalid
            err.success = FAIL;
            err.errorCode = ERR_FILE_NOT_FOUND;
            err.errorInfo = fp.c_str();
            return err;
        }
        // gets the file mode (first byte of the file)
        std::optional<Bytes> file_mode = fh.getFirstBytes(1);
        if (file_mode.has_value()) {
            // first byte was gotten successfully
            if (FModes(file_mode.value()[0]) == FData.getFileMode()) {
                // file mode is the same as the given file data
                ret.returnValue.push_back(err.returnValue[i]);
            }
        }
    }
    return ret;
}

template <typename FData>
ErrorStruct<bool> API<FData>::createEncFile(std::filesystem::path file_path) noexcept {
    ErrorStruct<bool> err;
    err.success = FAIL;
    if (file_path.empty()) {
        // the given path is empty
        err.errorCode = ERR_EMPTY_FILEPATH;
        err.errorInfo = file_path.c_str();
        return err;
    }
    if (file_path.extension() != FileHandler::extension) {
        // the given path does not have the right extension
        err.errorCode = ERR_EXTENSION_INVALID;
        err.errorInfo = file_path.c_str();
        return err;
    }
    if (std::filesystem::exists(file_path)) {
        // the given path already exists
        err.errorCode = ERR_FILE_EXISTS;
        err.errorInfo = file_path.c_str();
        return err;
    }
    // create the file
    std::ofstream file(file_path);
    if (!file.is_open()) {
        // the file could not be created
        err.errorCode = ERR_FILE_NOT_CREATED;
        err.errorInfo = file_path.c_str();
        return err;
    }
    err.success = SUCCESS;
    err.returnValue = true;
    // validates this file path
    this->valid_file = file_path;
    return err;
}

template <typename FData>
ErrorStruct<DataHeader> API<FData>::getDataHeader(const Bytes file_content) noexcept {
    // gets the data header from the file content
    Bytes file_content_copy = file_content;
    ErrorStruct<DataHeader> err;
    err.success = false;
    err.returnValue = DataHeader(CHAINHASH_NORMAL);
    ErrorStruct<Bytes> err_header = err.returnValue.setHeaderBytes(file_content_copy);
    err.success = err_header.success;
    err.errorCode = err_header.errorCode;
    err.errorInfo = err_header.errorInfo;
    err.what = err_header.what;
    return err;
}

template <typename FData>
ErrorStruct<Bytes> API<FData>::getData(const Bytes file_content) noexcept {
    // gets the data from the file content (removes the header)
    Bytes file_content_copy = file_content;
    ErrorStruct<Bytes> err;
    // calculate the header bytes
    ErrorStruct<Bytes> err_header = DataHeader(CHAINHASH_NORMAL).setHeaderBytes(file_content_copy);
    err.errorCode = err_header.errorCode;
    err.errorInfo = err_header.errorInfo;
    err.what = err_header.what;
    if (err_header.success) {
        Bytes data = file_content_copy;
        // remove the header from the data
        data.popFirstBytes(err_header.returnValue.getLen());
        err.returnValue = data;
        err.success = true;
    } else {
        err.success = false;
    }
    return err;
}
