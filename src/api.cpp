/*
implementation of api.h
*/

#include "api.h"

#include "blockchain_decrypt.h"
#include "blockchain_encrypt.h"
#include "file_modes.h"
#include "logger.h"
#include "timer.h"

ErrorStruct<FileHandler> API::getFileHandler(const std::filesystem::path file_path) const noexcept {
    // checks if the given file path is valid and sets the file handler
    ErrorStruct<bool> err1 = FileHandler::isValidPath(file_path, true);
    if (!err1.isSuccess()) {
        // the file path is invalid
        PLOG_ERROR << "The file path is invalid (file path: " << file_path.c_str() << ", errorCode: " << +err1.errorCode << ", errorInfo: " << err1.errorInfo << ", what: " << err1.what << ")";
        return ErrorStruct<FileHandler>{err1.success, err1.errorCode, err1.errorInfo, err1.what};
    }
    return ErrorStruct<FileHandler>{FileHandler(file_path)};
}

DataHeaderHelperStruct API::createDataHeaderIters(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout) const noexcept {
    // creates a DataHeader with the given settings (helper function for createDataHeader)
    PLOG_VERBOSE << "Creating data header with iter settings (file_mode: " << +ds.file_mode << ", ch1mode: " << +ds.chainhash1_mode << ", ch1iters: " << ds.chainhash1_iters
                 << ", ch2mode: " << +ds.chainhash2_mode << ", ch2iters: " << ds.chainhash2_iters << ", timeout: " << timeout << ")";
    ErrorStruct<DataHeader> err{SuccessType::FAIL, ErrorCode::ERR, ""};
    DataHeaderHelperStruct dhhs{err};
    if (ds.file_mode != this->file_data_struct.file_mode) {
        // the file mode does not match with the file data mode
        PLOG_ERROR << "The provided file mode does not match with selected file (file_mode: " << +ds.file_mode << ", selected file file_mode: " << +this->file_data_struct.file_mode << ")";
        dhhs.errorStruct.errorCode = ErrorCode::ERR_FILEMODE_INVALID;
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
        PLOG_ERROR << "Something went wrong while creating the chainhashes (what: " << e.what() << ")";
        dhhs.errorStruct.errorInfo = "Something went wrong while creating the chainhashes";
        dhhs.errorStruct.what = e.what();
        return dhhs;
    }
    // setting up the other parts
    dhp.file_mode = ds.file_mode;
    dhp.hash_mode = ds.hash_mode;
    // create a new shared hash ptr
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(dhp.hash_mode));
    dhp.enc_salt = Bytes{hash->getHashSize()};  // generating random salt for encryption

    // setting up an timer to calculate the remaining time for the second chainhash
    Timer timer;
    timer.start();
    // first chainhash (password -> passwordhash)
    ErrorStruct<Bytes> ch1_err = ChainHashModes::performChainHash(dhp.chainhash1, hash, password, timeout);
    if (!ch1_err.isSuccess()) {
        // something went wrong while performing the first chainhash
        PLOG_ERROR << "Something went wrong while performing the first chainhash (success: " << +ch1_err.success << ", errorCode: " << +ch1_err.errorCode << ", errorInfo: " << ch1_err.errorInfo
                   << ", what: " << ch1_err.what << ")";
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
            PLOG_ERROR << "Timeout reached after the first chainhash";
            dhhs.errorStruct.success = SuccessType::TIMEOUT;
            dhhs.errorStruct.errorCode = ErrorCode::ERR_TIMEOUT;
            return dhhs;
        } else {
            // if there is still time left, we have to subtract the elapsed time from the timeout
            timeout_copy -= elapsedTime;
        }
    }
    // second chainhash (passwordhash -> passwordhashhash = validation hash)
    ErrorStruct<Bytes> ch2_err = ChainHashModes::performChainHash(dhp.chainhash2, hash, ch1_err.returnValue(), timeout_copy);
    if (!ch2_err.isSuccess()) {
        // something went wrong while performing the second chainhash
        PLOG_ERROR << "Something went wrong while performing the second chainhash (success: " << +ch2_err.success << ", errorCode: " << +ch2_err.errorCode << ", errorInfo: " << ch2_err.errorInfo
                   << ", what: " << ch2_err.what << ")";
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
    PLOG_VERBOSE << "Creating data header with time settings (file_mode: " << +ds.file_mode << ", ch1mode: " << +ds.chainhash1_mode << ", ch1time: " << ds.chainhash1_time
                 << ", ch2mode: " << +ds.chainhash2_mode << ", ch2time: " << ds.chainhash2_time << ")";
    ErrorStruct<DataHeader> err{SuccessType::FAIL, ErrorCode::ERR, "", ""};
    DataHeaderHelperStruct dhhs{err};

    if (ds.file_mode != this->file_data_struct.file_mode) {
        // the file mode does not match with the file data mode
        PLOG_ERROR << "The provided file mode does not match with selected file (file_mode: " << +ds.file_mode << ", selected file file_mode: " << +this->file_data_struct.file_mode << ")";
        dhhs.errorStruct.errorCode = ErrorCode::ERR_FILEMODE_INVALID;
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
        PLOG_ERROR << "Something went wrong while creating the chainhashes (what: " << e.what() << ")";
        dhhs.errorStruct.errorInfo = "Something went wrong while creating the chainhashes";
        dhhs.errorStruct.what = e.what();
        return dhhs;
    }
    // setting up the other parts
    dhp.file_mode = ds.file_mode;
    dhp.hash_mode = ds.hash_mode;
    // create new shared hash ptr
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(dhp.hash_mode));
    dhp.enc_salt = Bytes{hash->getHashSize()};  // generating random salt for encryption

    // first chainhash (password -> passwordhash)
    ErrorStruct<ChainHashResult> ch1_err = ChainHashModes::performChainHash(ch1, hash, password);
    if (!ch1_err.isSuccess()) {
        // something went wrong while performing the first chainhash
        PLOG_ERROR << "Something went wrong while performing the first chainhash (success: " << +ch1_err.success << ", errorCode: " << +ch1_err.errorCode << ", errorInfo: " << ch1_err.errorInfo
                   << ", what: " << ch1_err.what << ")";
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
    if (!ch2_err.isSuccess()) {
        // something went wrong while performing the second chainhash
        PLOG_ERROR << "Something went wrong while performing the second chainhash (success: " << +ch2_err.success << ", errorCode: " << +ch2_err.errorCode << ", errorInfo: " << ch2_err.errorInfo
                   << ", what: " << ch2_err.what << ")";
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

API::API(const FModes file_mode) : current_state(INIT(this)), dh(DataHeader{HModes(STANDARD_HASHMODE)}) {
    // constructs the API in a given workflow mode and initializes the private variables
    PLOG_VERBOSE << "API object created (file_mode: " << +file_mode << ")";
    if (!FileModes::isModeValid(file_mode)) {
        PLOG_ERROR << "Invalid file mode passed to API constructor (file_mode: " << +file_mode << ")";
        throw std::invalid_argument("Invalid file mode");
    }
    this->file_data_struct = FileDataStruct{file_mode, Bytes()};
    this->correct_password_hash = Bytes();
    this->encrypted_data = Bytes();
    this->selected_file = std::optional<FileHandler>();
}

std::filesystem::path API::getCurrentDirPath() noexcept {
    // gets the current directory path
    return std::filesystem::current_path();
}

ErrorStruct<std::filesystem::path> API::getEncDirPath() noexcept {
    // gets the path of the directory where the .enc files are stored at default
    std::filesystem::path enc_path = "WORK";
    ErrorStruct<std::filesystem::path> err;
    err.setReturnValue(enc_path);
    err.success = SuccessType::FAIL;
    if (enc_path.empty()) {
        // if the path is empty, the filehandler could not find the encryption dir
        PLOG_FATAL << "The encryption dir could not be found (getEncDirPath): empty path";
        err.errorCode = ErrorCode::ERR_EMPTY_FILEPATH;
        err.errorInfo = "Encryption dir";
    } else if (!std::filesystem::exists(enc_path)) {
        // if the path does not exist, the encryption dir does not exist
        PLOG_FATAL << "The encryption dir could not be found (getEncDirPath): path does not exist (enc_path: " << enc_path << ")";
        err.errorCode = ErrorCode::ERR_FILEPATH_INVALID;
        err.errorInfo = enc_path.c_str();
    } else {
        // otherwise the path is valid
        err.success = SuccessType::SUCCESS;
    }
    return err;
}

ErrorStruct<std::vector<std::string>> API::getAllEncFileNames(const std::filesystem::path dir) noexcept {
    // gets all the .enc file names in the given directory
    std::vector<std::string> file_names;
    ErrorStruct<std::vector<std::string>> err;
    err.success = SuccessType::FAIL;
    if (dir.empty()) {
        // the given path is empty
        PLOG_ERROR << "The given path is empty (getAllEncFileNames)";
        err.errorCode = ErrorCode::ERR_EMPTY_FILEPATH;
        return err;
    }
    if (!std::filesystem::exists(dir)) {
        // the given path does not exist
        PLOG_ERROR << "The given path does not exist (getAllEncFileNames) (dir: " << dir << ")";
        err.errorCode = ErrorCode::ERR_FILEPATH_INVALID;
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
    return ErrorStruct<std::vector<std::string>>{file_names};
}

ErrorStruct<std::vector<std::string>> API::INIT::getRelevantFileNames(const std::filesystem::path dir) noexcept {
    // only gets the file names that have the same file mode as the given file data or are empty
    // gets all file names
    PLOG_VERBOSE << "Getting relevant file names (dir: " << dir << ")";
    ErrorStruct<std::vector<std::string>> err = API::getAllEncFileNames(dir);
    if (!err.isSuccess()) {
        PLOG_ERROR << "Could not get all file names (getRelevantFileNames) (err.errorCode: " << +err.errorCode << ", err.errorInfo: " << err.errorInfo << ")";
        return err;
    }
    ErrorStruct<std::vector<std::string>> ret{std::vector<std::string>()};
    for (int i = 0; i < err.returnValue().size(); i++) {
        // checks for every file if it has the same file mode as the given file data
        // build the complete file path
        std::filesystem::path fp = dir / err.returnValue()[i];
        // checks if the file data mode matches with the file mode of the file is empty
        ErrorStruct<FileHandler> err2 = this->parent->getFileHandler(fp);
        if (err2.isSuccess()) {
            if(err2.returnValue().isEmtpy() ||
            err2.returnValue().isDataHeader(this->parent->file_data_struct.file_mode).isSuccess()){
                // dataheader is valid or file is empty
                // we include the file to the relevant files
                std::vector<std::string> tmp = ret.returnValue();
                tmp.push_back(err.returnValue()[i]);
                ret.setReturnValue(tmp);
            }
        }
    }
    return ret;
}

ErrorStruct<bool> API::INIT::createFile(const std::filesystem::path file_path) noexcept {
    // creates a new .enc file at the given path and validates it
    return FileHandler::createFile(file_path);
}

ErrorStruct<bool> API::INIT::selectFile(const std::filesystem::path file_path) noexcept {
    // selects the given file as the working file
    PLOG_VERBOSE << "Selecting file (file_path: " << file_path << ")";
    ErrorStruct<FileHandler> err = this->parent->getFileHandler(file_path);
    if (!err.isSuccess()) {
        // file path is invalid
        PLOG_ERROR << "The file path is invalid (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
        return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
    }
    // sets the file data
    ErrorStruct<DataHeader> err_dataheader = err.returnValue().getDataHeader();
    if(!err_dataheader.isSuccess()){
        // the data header could not be read
        PLOG_ERROR << "The data header could not be read (errorCode: " << +err_dataheader.errorCode << ", errorInfo: " << err_dataheader.errorInfo << ", what: " << err_dataheader.what << ")";
        return ErrorStruct<bool>{err_dataheader.success, err_dataheader.errorCode, err_dataheader.errorInfo, err_dataheader.what};
    }
    Bytes content;
    try{
        content = err.returnValue().getAllBytes();
    }catch(std::exception& e){
        // the data could not be read
        PLOG_ERROR << "The data could not be read (what: " << e.what() << ")";
        return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR_FILE_READ, file_path.c_str(), e.what()};
    }
    this->parent->encrypted_data = content;
    this->parent->dh = err_dataheader.returnValue();

    PLOG_INFO << "File selected (selectFile) (file_path: " << file_path << ")";
    // set the selected file
    this->parent->selected_file = err.returnValue();
    this->parent->current_state = FILE_SELECTED(this->parent);
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::FILE_SELECTED::isFileEmpty() const noexcept {
    // gets if the working file is empty
    return ErrorStruct<bool>{this->parent->selected_file.value().isEmtpy()};
}

ErrorStruct<bool> API::FILE_SELECTED::deleteFile() noexcept {
    // deletes the working file
    PLOG_VERBOSE << "Deleting file (file_path: " << this->parent->selected_file.value().getPath() << ")";
    if (!std::filesystem::remove(this->parent->selected_file.value().getPath())) {
        // the file could not be deleted
        PLOG_ERROR << "The file could not be deleted (deleteFile)";
        return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_DELETED, this->parent->selected_file.value().getPath().c_str()};
    }
    // reset the selected file
    return this->unselectFile();
}

ErrorStruct<Bytes> API::FILE_SELECTED::getFileContent() noexcept {
    // gets the content of the working file
    Bytes content;
    try{
        content = this->parent->selected_file.value().getAllBytes();
    }catch(std::exception& e){
        // the data could not be read
        PLOG_ERROR << "The data could not be read (what: " << e.what() << ")";
        return ErrorStruct<Bytes>{SuccessType::FAIL, ErrorCode::ERR_FILE_READ, this->parent->selected_file.value().getPath().c_str(), e.what()};
    }
    return ErrorStruct<Bytes>{content};
}

ErrorStruct<bool> API::FILE_SELECTED::unselectFile() noexcept {
    // unselects the working file
    PLOG_VERBOSE << "Unselecting file (file_path: " << this->parent->selected_file.value().getPath() << ")";
    this->parent->selected_file = std::optional<FileHandler>();
    this->parent->encrypted_data = Bytes();
    this->parent->current_state = INIT(this->parent);
    return ErrorStruct<bool>{true};
}

ErrorStruct<Bytes> API::FILE_SELECTED::verifyPassword(const std::string password, const u_int64_t timeout) noexcept {
    // timeout=0 means no timeout
    // checks if a password (given from the user to decrypt) is valid for the selected file and returns its hash.
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
    PLOG_VERBOSE << "Verifying password (timeout: " << timeout << ")";
    if (this->parent->selected_file.value().isEmtpy()) {
        // file is empty, verifyPassword is not possible
        PLOG_ERROR << "File is empty, verifyPassword is not possible (verifyPassword) (file_path: " << this->parent->selected_file.value().getPath().c_str() << ")";
        return ErrorStruct<Bytes>{SuccessType::FAIL, ErrorCode::ERR_API_STATE_INVALID, "file is empty in verifyPassword"};
    }

    std::shared_ptr<Hash> hash = nullptr;

    try {
        // gets the header parts to work with (could throw)
        DataHeaderParts dhp = this->parent->dh.getDataHeaderParts();
        // gets the hash function (could throw)
        hash = std::move(HashModes::getHash(dhp.hash_mode));
        // perform the first chain hash (password -> passwordhash)
        ErrorStruct<Bytes> err1 = ChainHashModes::performChainHash(dhp.chainhash1, hash, password, timeout);
        if (!err1.isSuccess()) {
            // the first chain hash failed (due to timeout or other error)
            PLOG_ERROR << "The first chain hash failed (verifyPassword) (err1.success: " << +err1.success << ", err1.errorCode: " << +err1.errorCode << ", err1.errorInfo: " << err1.errorInfo
                       << ", err1.what: " << err1.what << ")";
            return err1;
        }
        // perform the second chain hash (passwordhash -> passwordhashhash = validation hash)
        ErrorStruct<Bytes> err2 = ChainHashModes::performChainHash(dhp.chainhash2, hash, err1.returnValue(), timeout);
        if (!err2.isSuccess()) {
            // the second chain hash failed (due to timeout or other error)
            PLOG_ERROR << "The second chain hash failed (verifyPassword) (err2.success: " << +err2.success << ", err2.errorCode: " << +err2.errorCode << ", err2.errorInfo: " << err2.errorInfo
                       << ", err2.what: " << err2.what << ")";
            return err2;
        }
        if (err2.returnValue() == dhp.valid_passwordhash) {
            // the password is valid (because the validation hashes match)
            // updating the state
            // setting the correct password hash and dataheader to the application
            PLOG_INFO << "The given password is valid (verifyPassword)";
            this->parent->correct_password_hash = err1.returnValue();
            this->parent->current_state = PASSWORD_VERIFIED(this->parent);
            return err1;
        }
        // the password is invalid
        PLOG_INFO << "The given password is invalid (verifyPassword)";
        ErrorStruct<Bytes> err;
        err.success = SuccessType::FAIL;
        err.errorCode = ErrorCode::ERR_PASSWORD_INVALID;
        return err;

    } catch (const std::exception& e) {
        // some error occurred
        PLOG_ERROR << "Some error occurred while verifying the password (verifyPassword) (what: " << e.what() << ")";
        ErrorStruct<Bytes> err{SuccessType::FAIL, ErrorCode::ERR, "Some error occurred while verifying the password", e.what()};
        return err;
    }
}

ErrorStruct<DataHeader> API::FILE_SELECTED::createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout) noexcept {
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations
    PLOG_VERBOSE << "Creating data header with iterations settings (file_mode: " << +ds.file_mode << ", ch1mode: " << +ds.chainhash1_mode << ", ch1iters: " << ds.chainhash1_iters
                 << ", ch2mode: " << +ds.chainhash2_mode << ", ch2iters: " << ds.chainhash2_iters << ", timeout: " << timeout << ")";
    if (!this->parent->selected_file.value().isEmtpy()) {
        // file is not empty, createDataHeader is not possible
        PLOG_ERROR << "File is not empty, createDataHeader is not possible (createDataHeader) (file_path: " << this->parent->selected_file.value().getPath().c_str() << ")";
        return ErrorStruct<DataHeader>{SuccessType::FAIL, ErrorCode::ERR_API_STATE_INVALID, "file is not empty in createDataHeader"};
    }
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderIters(password, ds, timeout);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return dhhs.errorStruct;
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
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
    PLOG_VERBOSE << "Creating data header with time settings (file_mode: " << +ds.file_mode << ", ch1mode: " << +ds.chainhash1_mode << ", ch1time: " << ds.chainhash1_time
                 << ", ch2mode: " << +ds.chainhash2_mode << ", ch2time: " << ds.chainhash2_time << ")";
    if (!this->parent->selected_file.value().isEmtpy()) {
        // file is not empty, createDataHeader is not possible
        PLOG_ERROR << "File is not empty, createDataHeader is not possible (createDataHeader) (file_path: " << this->parent->selected_file.value().getPath().c_str() << ")";
        return ErrorStruct<DataHeader>{SuccessType::FAIL, ErrorCode::ERR_API_STATE_INVALID, "file is not empty in createDataHeader"};
    }

    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderTime(password, ds);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return dhhs.errorStruct;
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
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
    PLOG_VERBOSE << "Getting decrypted data";
    try {
        // get the hash object that corresponds to the hash mode
        std::unique_ptr<Hash> hash = std::move(HashModes::getHash(this->parent->dh.getDataHeaderParts().hash_mode));
        // construct the blockchain
        DecryptBlockChain dbc{std::move(hash), this->parent->correct_password_hash, this->parent->dh.getDataHeaderParts().enc_salt};
        // add the data onto the blockchain
        dbc.addData(this->parent->encrypted_data);
        // get the decrypted data
        Bytes decrypted = dbc.getResult();
        FileDataStruct result{this->parent->file_data_struct.file_mode, decrypted};
        this->parent->file_data_struct = result;
        // changes the state
        this->parent->current_state = DECRYPTED(this->parent);
        return ErrorStruct<FileDataStruct>{result};
    } catch (const std::exception& e) {
        // something went wrong inside of one of these functions, read what message for more information
        PLOG_ERROR << "Something went wrong while decrypting the data (getDecryptedData) (what: " << e.what() << ")";
        return ErrorStruct<FileDataStruct>{SuccessType::FAIL, ErrorCode::ERR, "In getDecryptedData: Something went wrong while decrypting the data", e.what()};
    }
}

ErrorStruct<Bytes> API::DECRYPTED::getEncryptedData(const FileDataStruct file_data) noexcept {
    // encrypts the data and returns the encrypted data
    // uses the password and data header that were passed to verifyPassword
    PLOG_VERBOSE << "Getting encrypted data";
    ErrorStruct<Bytes> err{SuccessType::FAIL, ErrorCode::ERR, ""};
    if (file_data.file_mode != this->parent->file_data_struct.file_mode) {
        // the user wants to encrypt data with a different file mode
        PLOG_ERROR << "The provided file mode does not match with the selected file data mode (getEncryptedData) (provided file mode: " << +file_data.file_mode
                   << ", selected file mode: " << +this->parent->file_data_struct.file_mode << ")";
        err.errorCode = ErrorCode::ERR_FILEMODE_INVALID;
        err.errorInfo = " In getEncryptedData: The provided file mode does not match with the given file data mode";
        return err;
    }
    try {
        // get the hash ptr
        std::unique_ptr<Hash> hash = std::move(HashModes::getHash(this->parent->dh.getDataHeaderParts().hash_mode));
        // construct the blockchain
        EncryptBlockChain ebc{std::move(hash), this->parent->correct_password_hash, this->parent->dh.getDataHeaderParts().enc_salt};
        // add the data onto the blockchain
        ebc.addData(file_data.dec_data);
        // get the encrypted data
        Bytes encrypted = ebc.getResult();

        this->parent->file_data_struct = file_data;
        this->parent->encrypted_data = encrypted;
        // change the state
        this->parent->current_state = ENCRYPTED(this->parent);
        // returns the result
        return ErrorStruct<Bytes>{encrypted};
    } catch (const std::exception& e) {
        // something went wrong inside of one of these functions, read what message for more information
        PLOG_ERROR << "Something went wrong while encrypting the data (getEncryptedData) (what: " << e.what() << ")";
        err.errorInfo = "In getEncryptedData: Something went wrong while encrypting the data";
        err.what = e.what();
        return err;
    }
}

ErrorStruct<FileDataStruct> API::DECRYPTED::getFileData() noexcept { return ErrorStruct<FileDataStruct>{this->parent->file_data_struct}; }

ErrorStruct<DataHeader> API::DECRYPTED::changeSalt() noexcept {
    // creates data header with the current settings and password, just changes the salt
    // this call is not expensive because it does not have to chainhash the password
    PLOG_VERBOSE << "Changing salt";
    DataHeaderParts dhp = this->parent->dh.getDataHeaderParts();
    // only changes salt
    dhp.enc_salt = Bytes(dhp.enc_salt.getLen());
    // dataheader parts is now ready to create the dataheader object
    ErrorStruct<DataHeader> err = DataHeader::setHeaderParts(dhp);
    if (err.isSuccess()) {
        this->parent->dh = err.returnValue();
    } else {
        PLOG_ERROR << "The data header could not be created (changeSalt) (success: " << +err.success << ", errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what
                   << ")";
    }
    return err;
}

ErrorStruct<DataHeader> API::DECRYPTED::createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout) noexcept {
    // the new generated header will be used for the next encryption, that means the old password is not valid anymore
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations
    PLOG_VERBOSE << "Creating data header with iterations settings (file_mode: " << +ds.file_mode << ", ch1mode: " << +ds.chainhash1_mode << ", ch1iters: " << ds.chainhash1_iters
                 << ", ch2mode: " << +ds.chainhash2_mode << ", ch2iters: " << ds.chainhash2_iters << ", timeout: " << timeout << ")";
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderIters(password, ds, timeout);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return dhhs.errorStruct;
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
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
    PLOG_VERBOSE << "Creating data header with time settings (file_mode: " << +ds.file_mode << ", ch1mode: " << +ds.chainhash1_mode << ", ch1time: " << ds.chainhash1_time
                 << ", ch2mode: " << +ds.chainhash2_mode << ", ch2time: " << ds.chainhash2_time << ")";
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->createDataHeaderTime(password, ds);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return dhhs.errorStruct;
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
    // updating the data header and the password hash
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnValue();
    return dhhs.errorStruct;
}

ErrorStruct<bool> API::ENCRYPTED::writeToFile(const std::filesystem::path file_path) noexcept {
    // writes encrypted data to a file adds the dataheader, uses the encrypted data from getEncryptedData
    PLOG_VERBOSE << "Writing to file (file_path: " << file_path << ")";
    // checks if the selected file exists
    ErrorStruct<FileHandler> err_file = this->parent->getFileHandler(file_path);
    if (!err_file.isSuccess()) {
        PLOG_ERROR << "The provided file path is invalid (writeToFile) (errorCode: " << +err_file.errorCode << ", errorInfo: " << err_file.errorInfo << ", what: " << err_file.what << ")";
        return ErrorStruct<bool>{err_file.success, err_file.errorCode, err_file.errorInfo, err_file.what};
    }
    Bytes full_data;
    full_data.addBytes(this->parent->dh.getHeaderBytes());  // adds the data header
    full_data.addBytes(this->parent->encrypted_data);       // adds the encrypted data
    ErrorStruct<bool> err = err_file.returnValue().writeBytesIfEmpty(full_data);
    if (err.isSuccess()) {
        this->parent->current_state = FINISHED(this->parent);
    } else {
        PLOG_ERROR << "Some error occurred while writing to the file (writeToFile) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
    }
    return err;
}

ErrorStruct<bool> API::ENCRYPTED::writeToFile() noexcept {
    // writes encrypted data to the selected file adds the dataheader, uses the encrypted data from getEncryptedData
    PLOG_VERBOSE << "Writing to selected file (file_path: " << this->parent->selected_file.value().getPath().c_str() << ")";
    // checks if the selected file exists (it could be deleted in the meantime)
    Bytes full_data;
    full_data.addBytes(this->parent->dh.getHeaderBytes());  // adds the data header
    full_data.addBytes(this->parent->encrypted_data);       // adds the encrypted data
    ErrorStruct<bool> err = this->parent->selected_file.value().writeBytes(full_data);
    if (err.isSuccess()) {
        this->parent->current_state = FINISHED(this->parent);
    } else {
        PLOG_FATAL << "Some error occurred while writing to the selected file (writeToFile) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
    }
    return err;
}
