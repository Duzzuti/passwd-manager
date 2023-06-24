/*
implementation of api.h
*/

#include "api.h"

#include "file_modes.h"
#include "filehandler.h"

ErrorStruct<bool> API::checkFilePath(const std::filesystem::path file_path, const bool should_exist) const noexcept {
    // checks if the given file path is valid (file_path has to be not empty and have the right extension)
    // if should_exist is true, the file has to exist otherwise it has to not exist
    ErrorStruct<bool> err;
    err.returnValue = false;
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
    if (should_exist) {
        if (!std::filesystem::exists(file_path)) {
            // the given path does not exist
            err.errorCode = ERR_FILE_NOT_FOUND;
            err.errorInfo = file_path.c_str();
            return err;
        }
    } else {
        if (std::filesystem::exists(file_path)) {
            // the given path already exists
            err.errorCode = ERR_FILE_EXISTS;
            err.errorInfo = file_path.c_str();
            return err;
        }
    }
    // all checks passed
    return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", true};
}

ErrorStruct<bool> API::checkFileData(const std::filesystem::path file_path) const noexcept {
    // checks if the given file data is valid or the file is empty
    // there has to be a valid data header in this file
    ErrorStruct<bool> err;
    FileHandler fh;
    if (fh.setEncryptionFilePath(file_path) != SUCCESS) {
        // the given file path is invalid
        err.errorCode = ERR_FILE_NOT_FOUND;
        err.errorInfo = file_path.c_str();
        return err;
    }
    try {
        Bytes file_mode;
        file_mode = fh.getFirstBytes(1);
        // the file is not empty
        if (FModes(file_mode.getBytes()[0]) != this->file_data_struct.file_mode) {
            // the file mode does not match with the file data mode
            err.errorCode = ERR_FILEMODE_INVALID;
            err.errorInfo = file_path.c_str();
            return err;
        }
    } catch (std::length_error& e) {
        // the file is empty
    } catch (std::exception& e) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = file_path.c_str();
        err.what = e.what();
        return err;
    }

    // file is empty or file mode matches with file data mode
    err.success = SUCCESS;
    err.returnValue = true;
    return err;
}

API::API(Workflow workflow, const FModes file_mode) {
    // constructs the API in a given worflow mode and initializes the private variables
    if (workflow != WORK_WITH_OLD_FILE && workflow != WORK_WITH_NEW_FILE && workflow != DELETE_FILE) {
        throw std::invalid_argument("Invalid workflow mode");
    }
    if (!FileModes::isModeValid(file_mode)) {
        throw std::invalid_argument("Invalid file mode");
    }
    this->current_workflow = workflow;
    this->file_data_struct = FileDataStruct{file_mode, Bytes()};
    this->current_state = INIT;
}

std::filesystem::path API::getCurrentDirPath() noexcept {
    // gets the current directory path
    return std::filesystem::current_path();
}

ErrorStruct<std::filesystem::path> API::getEncDirPath() noexcept {
    // gets the path of the directory where the .enc files are stored at default
    FileHandler fh;
    std::filesystem::path enc_path = fh.getEncryptionFilePath();
    ErrorStruct<std::filesystem::path> err;
    err.returnValue = enc_path;
    err.success = FAIL;
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
        err.success = SUCCESS;
    }
    return err;
}

ErrorStruct<std::vector<std::string>> API::getAllEncFileNames(const std::filesystem::path dir) noexcept {
    // gets all the .enc file names in the given directory
    std::vector<std::string> file_names;
    ErrorStruct<std::vector<std::string>> err;
    err.success = FAIL;
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
    err.success = SUCCESS;
    err.returnValue = file_names;
    return err;
}

ErrorStruct<std::vector<std::string>> API::getRelevantFileNames(const std::filesystem::path dir) noexcept {
    // only gets the file names that have the same file mode as the given file data or are empty
    ErrorStruct<std::vector<std::string>> ret;
    if (this->current_state < INIT) {
        // the api is not initialized
        ret.success = FAIL;
        ret.errorCode = ERR_API_NOT_INITIALIZED;
        ret.errorInfo = "getRelevantFileNames";
        return ret;
    }
    // gets all file names
    ErrorStruct<std::vector<std::string>> err = API::getAllEncFileNames(dir);
    if (err.success != SUCCESS) {
        return err;
    }
    ret.success = SUCCESS;
    ret.returnValue = std::vector<std::string>();
    for (int i = 0; i < err.returnValue.size(); i++) {
        // checks for every file if it has the same file mode as the given file data
        // build the complete file path
        std::filesystem::path fp = dir / err.returnValue[i];
        std::ifstream file(fp);
        // checks if the file data mode matches with the file mode of the file is empty
        ErrorStruct<bool> err2 = this->checkFileData(fp);
        if (err2.success == SUCCESS) {
            // file mode is the same as the given file data
            // we include the file to the relevant files
            ret.returnValue.push_back(err.returnValue[i]);
        }
    }
    return ret;
}

ErrorStruct<bool> API::createEncFile(const std::filesystem::path file_path) noexcept {
    // creates a new .enc file at the given path and validates it
    ErrorStruct<bool> err;
    err.success = FAIL;
    if (this->current_state != INIT) {
        // the api is in the wrong state
        err.errorCode = ERR_API_STATE_INVALID;
        err.errorInfo = "createEncFile is only available in the INIT state";
        return err;
    }
    if (this->current_workflow != WORK_WITH_NEW_FILE) {
        // wrong workflow to access this create file function
        err.errorCode = ERR_WRONG_WORKFLOW;
        err.errorInfo = "createEncFile is only available in the WORK_WITH_NEW_FILE workflow";
        return err;
    }
    // check if the file path is valid and the file does not exist
    ErrorStruct<bool> err2 = this->checkFilePath(file_path);
    if (err2.success != SUCCESS) {
        // the file path is invalid
        return err2;
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

ErrorStruct<bool> API::deleteEncFile(const std::filesystem::path file_path) const noexcept {
    // deletes the given file if it matches with the file data mode or if the file is empty
    ErrorStruct<bool> err;
    err.success = FAIL;
    if (this->current_workflow != DELETE_FILE) {
        // wrong workflow to access this delete file function
        err.errorCode = ERR_WRONG_WORKFLOW;
        err.errorInfo = "deleteEncFile is only available in the DELETE_FILE workflow";
        return err;
    }
    if (this->current_state != INIT) {
        // the api is in the wrong state
        err.errorCode = ERR_API_STATE_INVALID;
        err.errorInfo = "deleteEncFile is only available in the INIT state";
        return err;
    }
    // check if the file path is valid and the file exists
    ErrorStruct<bool> err2 = this->checkFilePath(file_path, true);
    if (err2.success != SUCCESS) {
        // the file path is invalid
        return err2;
    }
    // check if the file data mode matches with the mode used in the API (or the file is empty)
    ErrorStruct<bool> err3 = this->checkFileData(file_path);
    if (err3.success != SUCCESS) {
        // the file data mode is invalid
        return err3;
    }
    // file is empty or file mode matches
    if (!std::filesystem::remove(file_path)) {
        // the file could not be deleted
        err.errorCode = ERR_FILE_NOT_DELETED;
        err.errorInfo = file_path.c_str();
        return err;
    }
    err.success = SUCCESS;
    err.returnValue = true;
    return err;
}

ErrorStruct<DataHeader> API::getDataHeader(const Bytes file_content) noexcept {
    // gets the data header from the file content
    Bytes file_content_copy = file_content;
    return DataHeader::setHeaderBytes(file_content_copy);
}

ErrorStruct<Bytes> API::getData(const Bytes file_content) noexcept {
    // gets the data from the file content (removes the header)
    Bytes file_content_copy = file_content;
    ErrorStruct<Bytes> err;
    // calculate the header bytes
    ErrorStruct<DataHeader> err_header = DataHeader::setHeaderBytes(file_content_copy);
    // copy the struct except for the DataHeader object
    err.errorCode = err_header.errorCode;
    err.errorInfo = err_header.errorInfo;
    err.what = err_header.what;
    err.success = err_header.success;
    if (err_header.success) {
        // sets the bytes that are remaining after the header as the data
        err.returnValue = file_content_copy;
    }
    return err;
}

ErrorStruct<Bytes> API::getFileContent(const std::filesystem::path file_path) noexcept {
    // gets the content of the given file in Bytes
    ErrorStruct<Bytes> err;
    err.success = FAIL;
    if (this->current_workflow != WORK_WITH_OLD_FILE) {
        // wrong workflow to access this get file content function
        err.errorCode = ERR_WRONG_WORKFLOW;
        err.errorInfo = "getFileContent is only available in the WORK_WITH_OLD_FILE workflow";
        return err;
    }
    if (this->current_state != INIT) {
        // the api is in the wrong state
        err.errorCode = ERR_API_STATE_INVALID;
        err.errorInfo = "getFileContent is only available in the INIT state";
        return err;
    }
    std::filesystem::path file_path_copy = file_path;
    if (file_path_copy.has_extension()) {
        // the given path does not have an extension
        // we have to add it
        file_path_copy = std::filesystem::path(file_path_copy.c_str() + FileHandler::extension);
        if (!file_path_copy.has_extension()) {
            // something went wrong
            err.errorCode = ERR_BUG;
            err.errorInfo = "In function getFileContent: file_path_copy has no extension after adding one";
            return err;
        }
    }
    // file has now an extension
    ErrorStruct<bool> err2 = this->checkFilePath(file_path_copy, true);
    if (err2.success != SUCCESS) {
        // the file path is invalid
        return ErrorStruct<Bytes>{err2.success, err2.errorCode, err2.errorInfo, err2.what};
    }
    // the file path is valid
    ErrorStruct<bool> err3 = this->checkFileData(file_path_copy);
    if (err3.success != SUCCESS) {
        // the file data mode is invalid
        return ErrorStruct<Bytes>{err3.success, err3.errorCode, err3.errorInfo, err3.what};
    }
    // the content is valid
    FileHandler fh;
    fh.setEncryptionFilePath(file_path_copy);
    try {
        err.returnValue = fh.getAllBytes();
    } catch (const std::exception& e) {
        // something went wrong while reading the file
        err.errorCode = ERR;
        err.errorInfo = "Something went wrong while reading the file (trying to get all bytes)[getFileContent]";
        err.what = e.what();
        return err;
    }

    // this file is set as the working file now
    this->valid_file = file_path_copy;
    this->current_state = FILE_GOTTEN;

    err.success = SUCCESS;
    return err;
}
