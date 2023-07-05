/*
implementation of api.h
*/

#include "api.h"

#include "file_modes.h"
#include "filehandler.h"
#include "timer.h"

ErrorStruct<bool> API::checkFilePath(const std::filesystem::path file_path, const bool should_exist) const noexcept {
    // checks if the given file path is valid (file_path has to be not empty and have the right extension)
    // if should_exist is true, the file has to exist otherwise it has to not exist
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
    ErrorStruct<Bytes> err1 = this->getFileContent(file_path);
    if (!err1.isSuccess()) {
        // the file could not be read
        return ErrorStruct<bool>{err1.success, err1.errorCode, err1.errorInfo, err1.what, false};
    }
    if (err1.returnValue().isEmpty()) {
        // the file is empty
        return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", false};
    }
    ErrorStruct<DataHeader> err2 = this->getDataHeader(err1.returnValue());
    if (!err2.isSuccess()) {
        // the data header could not be read
        return ErrorStruct<bool>{err2.success, err2.errorCode, err2.errorInfo, err2.what, false};
    }
    if (err2.returnValue().getDataHeaderParts().file_mode != this->file_data_struct.file_mode) {
        // the file mode does not match with the file data mode
        return ErrorStruct<bool>{FAIL, ERR_FILEMODE_INVALID, file_path.c_str(), "", false};
    }
    return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", true};
}

ErrorStruct<std::filesystem::path> API::addExtension(const std::filesystem::path file_path) const noexcept {
    // adds the extension to the given file path if it does not have one
    ErrorStruct<std::filesystem::path> err;
    err.success = FAIL;
    std::filesystem::path file_path_copy = file_path;
    if (!file_path_copy.has_extension()) {
        // the given path does not have an extension
        // we have to add it
        file_path_copy = std::filesystem::path(file_path_copy.c_str() + FileHandler::extension);
        if (!file_path_copy.has_extension()) {
            // something went wrong
            err.errorCode = ERR_BUG;
            err.errorInfo = "In function addExtension: file_path_copy has no extension after adding one";
            return err;
        }
    }
    return ErrorStruct<std::filesystem::path>{SUCCESS, NO_ERR, "", "", file_path_copy};
}

ErrorStruct<Bytes> API::getFileContent(const std::filesystem::path file_path) const noexcept {
    // gets the content of the given file in Bytes
    ErrorStruct<Bytes> err;
    err.success = FAIL;

    // add the extension if it does not have one
    ErrorStruct<std::filesystem::path> err_ext = this->addExtension(file_path);
    if (!err_ext.isSuccess()) {
        // something went wrong while adding the extension
        return ErrorStruct<Bytes>{err_ext.success, err_ext.errorCode, err_ext.errorInfo, err_ext.what};
    }
    std::filesystem::path file_path_copy = err_ext.returnValue();

    // file has now an extension
    ErrorStruct<bool> err3 = this->checkFileData(file_path_copy);
    if (!err3.isSuccess()) {
        // the file data mode is invalid
        return ErrorStruct<Bytes>{err3.success, err3.errorCode, err3.errorInfo, err3.what};
    }
    // the content is valid
    FileHandler fh;
    fh.setEncryptionFilePath(file_path_copy);
    try {
        err.setReturnValue(fh.getAllBytes());
    } catch (const std::exception& e) {
        // something went wrong while reading the file
        err.errorCode = ERR;
        err.errorInfo = "Something went wrong while reading the file (trying to get all bytes)[getFileContent]";
        err.what = e.what();
        return err;
    }
    err.success = SUCCESS;
    return err;
}

DataHeaderHelperStruct API::createDataHeaderIters(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout) const noexcept {
    // creates a DataHeader with the given settings (helper function for createDataHeader)
    ErrorStruct<DataHeader> err{FAIL, ERR, "", "", DataHeader{HModes(STANDARD_HASHMODE)}};
    DataHeaderHelperStruct dhhs{err};
    if(ds.file_mode != this->file_data_struct.file_mode) {
        // the file mode does not match with the file data mode
        dhhs.errorStruct.errorCode = ERR_FILEMODE_INVALID;
        dhhs.errorStruct.errorInfo = "The file mode does not match with the file data mode";
        return dhhs;
    }
    DataHeaderParts dhp;
    try {
        // trying to get the chainhashes
        ChainHashData chd1{Format{ds.chainhash1_mode}};
        ChainHashData chd2{Format{ds.chainhash2_mode}};
        // setting random data for the chainhashes datablocks (salts that are used by the chainhash)
        chd1.generateRandomData();
        chd2.generateRandomData();
        // creating ChainHash structures
        dhp.chainhash1 = ChainHash{ds.chainhash1_mode, ds.chainhash1_iters, chd1};
        dhp.chainhash2 = ChainHash{ds.chainhash2_mode, ds.chainhash2_iters, chd2};
        dhp.chainhash1_datablock_len = chd1.getLen();
        dhp.chainhash2_datablock_len = chd2.getLen();

    } catch (const std::exception& e) {
        // something went wrong while creating the chainhashes
        dhhs.errorStruct.errorInfo = "Something went wrong while creating the chainhashes";
        dhhs.errorStruct.what = e.what();
        return dhhs;
    }
    // setting up the other parts
    dhp.file_mode = ds.file_mode;
    dhp.hash_mode = ds.hash_mode;
    Hash* hash = HashModes::getHash(dhp.hash_mode);
    dhp.enc_salt = Bytes{hash->getHashSize()};  // generating random salt for encryption

    // setting up an timer to calculate the remaining time for the second chainhash
    Timer timer;
    timer.start();
    // first chainhash (password -> passwordhash)
    ErrorStruct<Bytes> ch1_err = ChainHashModes::performChainHash(dhp.chainhash1, hash, password, timeout);
    if (!ch1_err.isSuccess()) {
        // something went wrong while performing the first chainhash
        delete hash;
        dhhs.errorStruct.success = ch1_err.success;
        dhhs.errorStruct.errorCode = ch1_err.errorCode;
        dhhs.errorStruct.errorInfo = ch1_err.errorInfo;
        dhhs.errorStruct.what = ch1_err.what;
        return dhhs;
    }
    // handling the timeout
    u_int64_t elapsedTime = timer.peekTime();
    u_int64_t timeout_copy = timeout;
    if (timeout != 0) {
        // if the user set a timeout, we have to check how much time is left
        if (timeout <= elapsedTime) {
            // should not happen, but if the function closes and the timeout runs out shortly after, we have to return an timeout
            delete hash;
            dhhs.errorStruct.success = TIMEOUT;
            dhhs.errorStruct.errorCode = ERR_TIMEOUT;
            return dhhs;
        } else {
            // if there is still time left, we have to subtract the elapsed time from the timeout
            timeout_copy -= elapsedTime;
        }
    }
    // second chainhash (passwordhash -> passwordhashhash = validation hash)
    ErrorStruct<Bytes> ch2_err = ChainHashModes::performChainHash(dhp.chainhash2, hash, ch1_err.returnValue(), timeout_copy);
    delete hash;
    if (!ch2_err.isSuccess()) {
        // something went wrong while performing the second chainhash
        dhhs.errorStruct.success = ch2_err.success;
        dhhs.errorStruct.errorCode = ch2_err.errorCode;
        dhhs.errorStruct.errorInfo = ch2_err.errorInfo;
        dhhs.errorStruct.what = ch2_err.what;
        return dhhs;
    }

    // setting the validation hash
    dhp.valid_passwordhash = ch2_err.returnValue();

    // dataheader parts is now ready to create the dataheader object
    dhhs.errorStruct = DataHeader::setHeaderParts(dhp);
    dhhs.Password_hash(ch1_err.returnValue());  // adding the password hash to the dhhs struct
    return dhhs;
}

DataHeaderHelperStruct API::createDataHeaderTime(const std::string password, const DataHeaderSettingsTime ds) const noexcept {
    // creates a DataHeader with the given settings (helper function for createDataHeader)
    // sets up the return struct
    ErrorStruct<DataHeader> err{FAIL, ERR, "", "", DataHeader{HModes(STANDARD_HASHMODE)}};
    DataHeaderHelperStruct dhhs{err};

    if(ds.file_mode != this->file_data_struct.file_mode) {
        // the file mode does not match with the file data mode
        dhhs.errorStruct.errorCode = ERR_FILEMODE_INVALID;
        dhhs.errorStruct.errorInfo = "The file mode does not match with the file data mode";
        return dhhs;
    }

    DataHeaderParts dhp;
    ChainHashData chd1{Format{CHModes(STANDARD_CHAINHASHMODE)}};
    ChainHashData chd2{Format{CHModes(STANDARD_CHAINHASHMODE)}};
    ChainHashTimed ch1;
    ChainHashTimed ch2;
    try {
        // trying to get the chainhashes
        chd1 = ChainHashData(Format{ds.chainhash1_mode});
        chd2 = ChainHashData(Format{ds.chainhash2_mode});
        // setting random data for the chainhashes datablocks (salts that are used by the chainhash)
        chd1.generateRandomData();
        chd2.generateRandomData();
        // creating ChainHash structures
        ch1 = ChainHashTimed{ds.chainhash1_mode, ds.chainhash1_time, chd1};
        ch2 = ChainHashTimed{ds.chainhash2_mode, ds.chainhash2_time, chd2};
        dhp.chainhash1_datablock_len = chd1.getLen();
        dhp.chainhash2_datablock_len = chd2.getLen();

    } catch (const std::exception& e) {
        // something went wrong while creating the chainhashes
        dhhs.errorStruct.errorInfo = "Something went wrong while creating the chainhashes";
        dhhs.errorStruct.what = e.what();
        return dhhs;
    }
    // setting up the other parts
    dhp.file_mode = ds.file_mode;
    dhp.hash_mode = ds.hash_mode;
    Hash* hash = HashModes::getHash(dhp.hash_mode);
    dhp.enc_salt = Bytes{hash->getHashSize()};  // generating random salt for encryption

    // first chainhash (password -> passwordhash)
    ErrorStruct<ChainHashResult> ch1_err = ChainHashModes::performChainHash(ch1, hash, password);
    if (!ch1_err.isSuccess()) {
        // something went wrong while performing the first chainhash
        delete hash;
        dhhs.errorStruct.success = ch1_err.success;
        dhhs.errorStruct.errorCode = ch1_err.errorCode;
        dhhs.errorStruct.errorInfo = ch1_err.errorInfo;
        dhhs.errorStruct.what = ch1_err.what;
        return dhhs;
    }
    // collecting the ChainHash that was used (getting the iterations)
    dhp.chainhash1 = ch1_err.returnValue().chainhash;

    // second chainhash (passwordhash -> passwordhashhash = validation hash)
    ErrorStruct<ChainHashResult> ch2_err = ChainHashModes::performChainHash(ch2, hash, ch1_err.returnValue().result);
    delete hash;
    if (!ch2_err.isSuccess()) {
        // something went wrong while performing the second chainhash
        dhhs.errorStruct.success = ch2_err.success;
        dhhs.errorStruct.errorCode = ch2_err.errorCode;
        dhhs.errorStruct.errorInfo = ch2_err.errorInfo;
        dhhs.errorStruct.what = ch2_err.what;
        return dhhs;
    }

    // collecting the ChainHash that was used (getting the iterations)
    dhp.chainhash2 = ch2_err.returnValue().chainhash;

    // dataheader parts is now ready to create the dataheader object
    dhhs.errorStruct = DataHeader::setHeaderParts(dhp);
    dhhs.Password_hash(ch1_err.returnValue().result);  // adding the password hash to the dhhs struct
    return dhhs;
}

API::API(const FModes file_mode) {
    // constructs the API in a given worflow mode and initializes the private variables
    if (!FileModes::isModeValid(file_mode)) {
        throw std::invalid_argument("Invalid file mode");
    }
    this->file_data_struct = FileDataStruct{file_mode, Bytes()};
    this->correct_password_hash = Bytes();
    this->dh = DataHeader{HModes(STANDARD_HASHMODE)};
    this->encrypted_data = Bytes();
    this->selected_file = std::filesystem::path();
    this->current_state = INIT(this);
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
    err.setReturnValue(enc_path);
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
    err.setReturnValue(file_names);
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
        err.setReturnValue(file_content_copy);
    }
    return err;
}

ErrorStruct<std::vector<std::string>> API::INIT::getRelevantFileNames(const std::filesystem::path dir) noexcept {
    // only gets the file names that have the same file mode as the given file data or are empty
    // gets all file names
    ErrorStruct<std::vector<std::string>> err = API::getAllEncFileNames(dir);
    if (!err.isSuccess()) {
        return err;
    }
    ErrorStruct<std::vector<std::string>> ret{SUCCESS, NO_ERR, "", "", std::vector<std::string>()};
    for (int i = 0; i < err.returnValue().size(); i++) {
        // checks for every file if it has the same file mode as the given file data
        // build the complete file path
        std::filesystem::path fp = dir / err.returnValue()[i];
        std::ifstream file(fp);
        // checks if the file data mode matches with the file mode of the file is empty
        ErrorStruct<bool> err2 = this->parent->checkFileData(fp);
        if (err2.isSuccess()) {
            // file mode is the same as the given file data
            // we include the file to the relevant files
            std::vector<std::string> tmp = ret.returnValue();
            tmp.push_back(err.returnValue()[i]);
            ret.setReturnValue(tmp);
        }
    }
    return ret;
}

ErrorStruct<bool> API::INIT::createFile(const std::filesystem::path file_path) noexcept {
    // creates a new .enc file at the given path and validates it
    ErrorStruct<bool> err;
    err.success = FAIL;
    // check if the file path is valid and the file does not exist
    ErrorStruct<bool> err2 = this->parent->checkFilePath(file_path);
    if (!err2.isSuccess()) {
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
    err.setReturnValue(true);
    return err;
}

ErrorStruct<bool> API::INIT::selectFile(const std::filesystem::path file_path) noexcept {
    // selects the given file as the working file
    ErrorStruct<bool> err = this->parent->checkFileData(file_path);
    if (!err.isSuccess()) {
        // file data header is invalid
        return err;
    }
    // sets the file data
    if (err.returnValue()) {
        // the file is not empty
        ErrorStruct<Bytes> err_file = this->getFileContent();
        if (!err_file.isSuccess()) {
            // the file could not be read
            return ErrorStruct<bool>{FAIL, err_file.errorCode, err_file.errorInfo};
        }
        this->parent->encrypted_data = err_file.returnValue();
        ErrorStruct<DataHeader> err_dh = this->parent->getDataHeader(this->parent->encrypted_data);
        if (!err_dh.isSuccess()) {
            // the file data header is invalid
            return ErrorStruct<bool>{FAIL, err_dh.errorCode, err_dh.errorInfo};
        }
        this->parent->dh = err_dh.returnValue();
    }

    // set the selected file
    this->parent->selected_file = file_path;
    this->parent->file_empty = !err.returnValue();
    this->parent->current_state = FILE_SELECTED(this->parent);
    return err;
}

ErrorStruct<bool> API::FILE_SELECTED::isFileEmpty() const noexcept {
    // gets if the working file is empty
    return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", this->parent->file_empty};
    ;
}

ErrorStruct<bool> API::FILE_SELECTED::deleteFile() noexcept {
    // deletes the working file
    if (!std::filesystem::remove(this->parent->selected_file)) {
        // the file could not be deleted
        return ErrorStruct<bool>{FAIL, ERR_FILE_NOT_DELETED, this->parent->selected_file.c_str()};
    }
    // reset the selected file
    this->parent->selected_file = "";
    this->parent->encrypted_data = Bytes();
    this->parent->current_state = INIT(this->parent);
    return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", true};
}

ErrorStruct<Bytes> API::FILE_SELECTED::getFileContent() noexcept {
    // gets the content of the working file
    return this->parent->getFileContent(this->parent->selected_file);
}

ErrorStruct<bool> API::FILE_SELECTED::unselectFile() noexcept {
    // unselects the working file
    this->parent->selected_file = "";
    this->parent->encrypted_data = Bytes();
    this->parent->current_state = INIT(this->parent);
    return ErrorStruct<bool>{SUCCESS, NO_ERR, "", "", true};
}

ErrorStruct<Bytes> API::FILE_SELECTED::verifyPassword(const std::string password, const u_int64_t timeout) noexcept {
    // timeout=0 means no timeout
    // checks if a password (given from the user to decrypt) is valid for the selected file and returns its hash.
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid

    if (this->parent->file_empty) {
        // file is empty, veriyPassword is not possible
        return ErrorStruct<Bytes>{FAIL, ERR_API_STATE_INVALID, "file is empty in verifyPassword"};
    }

    Hash* hash;

    try {
        // gets the header parts to work with (could throw)
        DataHeaderParts dhp = this->parent->dh.getDataHeaderParts();
        // gets the hash function (could throw)
        hash = HashModes::getHash(dhp.hash_mode);
        // perform the first chain hash (password -> passwordhash)
        ErrorStruct<Bytes> err1 = ChainHashModes::performChainHash(dhp.chainhash1, hash, password, timeout);
        if (!err1.isSuccess()) {
            // the first chain hash failed (due to timeout or other error)
            delete hash;
            return err1;
        }
        // perform the second chain hash (passwordhash -> passwordhashhash = validation hash)
        ErrorStruct<Bytes> err2 = ChainHashModes::performChainHash(dhp.chainhash2, hash, err1.returnValue(), timeout);
        if (!err2.isSuccess()) {
            // the second chain hash failed (due to timeout or other error)
            delete hash;
            return err2;
        }
        if (err2.returnValue() == dhp.valid_passwordhash) {
            // the password is valid (because the validation hashes match)
            ErrorStruct<Bytes> err;
            err.success = SUCCESS;
            err.setReturnValue(err1.returnValue());

            // updating the state
            // setting the correct password hash and dataheader to the application
            this->parent->correct_password_hash = err1.returnValue();
            this->parent->current_state = PASSWORD_VERIFIED(this->parent);
            delete hash;
            return err;
        }
        // the password is invalid
        ErrorStruct<Bytes> err;
        err.success = FAIL;
        err.errorCode = ERR_PASSWORD_INVALID;
        delete hash;
        return err;

    } catch (const std::exception& e) {
        // some error occured
        ErrorStruct<Bytes> err;
        err.errorCode = ERR;
        err.errorInfo = "Some error occured while verifying the password";
        err.what = e.what();
        err.success = FAIL;
        if (hash != nullptr) delete hash;
        return err;
    }
}

ErrorStruct<DataHeader> API::FILE_SELECTED::createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout) noexcept {
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations

    if (!this->parent->file_empty) {
        // file is not empty, createDataHeader is not possible
        return ErrorStruct<DataHeader>{FAIL, ERR_API_STATE_INVALID, "file is not empty in createDataHeader", "", DataHeader{HModes(STANDARD_HASHMODE)}};
    }
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderIters(password, ds, timeout);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        return dhhs.errorStruct;
    }

    // updating the state
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnValue();
    this->parent->file_data_struct = FileDataStruct{this->parent->file_data_struct.file_mode, Bytes()};
    this->parent->current_state = DECRYPTED(this->parent);
    return dhhs.errorStruct;
}

ErrorStruct<DataHeader> API::FILE_SELECTED::createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept {
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // you can specify the time (in ms) that the chainhashes should take in the settings

    if (!this->parent->file_empty) {
        // file is not empty, createDataHeader is not possible
        return ErrorStruct<DataHeader>{FAIL, ERR_API_STATE_INVALID, "file is not empty in createDataHeader", "", DataHeader{HModes(STANDARD_HASHMODE)}};
    }

    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderTime(password, ds);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        return dhhs.errorStruct;
    }

    // updating the state
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnValue();
    this->parent->file_data_struct = FileDataStruct{this->parent->file_data_struct.file_mode, Bytes()};
    this->parent->current_state = DECRYPTED(this->parent);
    return dhhs.errorStruct;
}

ErrorStruct<FileDataStruct> API::PASSWORD_VERIFIED::getDecryptedData() noexcept {
    // decrypts the data (requires successful verifyPassword or createDataHeader run)
    // returns the decrypted content (without the data header)
    // uses the password and data header that were passed to verifyPassword (or createDataHeader for new files)

    // WORK
    Bytes decrypted;

    this->parent->file_data_struct = FileDataStruct{this->parent->file_data_struct.file_mode, decrypted};
    ErrorStruct<FileDataStruct> err_res{SUCCESS, NO_ERR, "", "", this->parent->file_data_struct};
    this->parent->current_state = DECRYPTED(this->parent);
    return err_res;
}

ErrorStruct<Bytes> API::DECRYPTED::getEncryptedData(const FileDataStruct file_data) noexcept {
    // WORK
    return ErrorStruct<Bytes>();
}

ErrorStruct<FileDataStruct> API::DECRYPTED::getFileData() noexcept { return ErrorStruct<FileDataStruct>{SUCCESS, NO_ERR, "", "", this->parent->file_data_struct}; }

ErrorStruct<DataHeader> API::DECRYPTED::changeSalt() noexcept {
    // creates data header with the current settings and password, just changes the salt
    // this call is not expensive because it does not have to chainhash the password

    DataHeaderParts dhp = this->parent->dh.getDataHeaderParts();
    // only changes salt
    dhp.enc_salt = Bytes(dhp.enc_salt.getLen());
    // dataheader parts is now ready to create the dataheader object
    ErrorStruct<DataHeader> err = DataHeader::setHeaderParts(dhp);
    if(err.isSuccess()){
        this->parent->dh = err.returnValue();
    }
    return err;
}

ErrorStruct<DataHeader> API::DECRYPTED::createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout) noexcept {
    // the new generated header will be used for the next encryption, that means the old password is not valid anymore
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations

    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderIters(password, ds, timeout);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        return dhhs.errorStruct;
    }

    // updating the data header and the password hash
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnValue();
    return dhhs.errorStruct;
}

ErrorStruct<DataHeader> API::DECRYPTED::createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept {
    // the new generated header will be used for the next encryption, that means the old password is not valid anymore
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // you can specify the time (in ms) that the chainhashes should take in the settings

    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderTime(password, ds);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        return dhhs.errorStruct;
    }

    // updating the data header and the password hash
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnValue();
    return dhhs.errorStruct;
}
