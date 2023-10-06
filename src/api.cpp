/*
implementation of api.h
*/

#include "api.h"

#include "blockchain_decrypt.h"
#include "blockchain_encrypt.h"
#include "blockchain_stream_decrypt.h"
#include "blockchain_stream_encrypt.h"
#include "file_modes.h"
#include "rng.h"
#include "timer.h"
#include "utility.h"

ErrorStruct<std::filesystem::path> API::encrypt(std::filesystem::path& path, const std::string& password, const APIConfEncrypt& config) noexcept {
    // encrypts the file at the given path with the given password
    PLOG_VERBOSE << "Encrypting file...";
    if (!std::filesystem::is_regular_file(path)) {
        PLOG_ERROR << "The given path is not a regular file (path: " << path.c_str() << ")";
        return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_FOUND, path.c_str()};
    }
    std::filesystem::path top_dir = config.enc_top_dir == "" ? path.parent_path() : config.enc_top_dir;
    std::string file_name = path.stem().string();
    std::string file_extension = path.extension().string();
    std::filesystem::path enc_path = RNG::get_random_string(10) + ".enc";
    if(config.enc_filename == "")
        while (std::filesystem::exists(top_dir / enc_path)) {
            enc_path = RNG::get_random_string(10) + ".enc";
        }
    else 
        enc_path = config.enc_filename;

    if(std::filesystem::exists(top_dir / enc_path)){
        PLOG_ERROR << "The given path already exists (path: " << (top_dir / enc_path).c_str() << ")";
        return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_EXISTS, (top_dir / enc_path).c_str()};
    }

    std::filesystem::path enc_path_full = top_dir / enc_path;
    
    DataHeaderSettingsIters ds;
    {
        ds.setFileDataMode(FILEMODE_BYTES);
        ds.setHashMode(config.hash_mode);
        ds.setChainHash1Mode(config.chainhash_mode1);
        ds.setChainHash2Mode(config.chainhash_mode2);
        ds.setChainHash1Iters(config.iters1);
        ds.setChainHash2Iters(config.iters2);
        if(config.include_filename)
            ds.enc_data_blocks.push_back(EncDataBlock(DataBlock(DatablockType::FILENAME, stringToBytes(file_name))));
        if(config.include_extension)
            ds.enc_data_blocks.push_back(EncDataBlock(DataBlock(DatablockType::FILEEXTENSION, stringToBytes(file_extension))));
    }

    API api{FILEMODE_BYTES};
    ErrorStruct<bool> err_create = api.createFile(enc_path_full);
    if (!err_create.isSuccess()) {
        PLOG_ERROR << "The file could not be created (errorCode: " << +err_create.errorCode << ", errorInfo: " << err_create.errorInfo << ", what: " << err_create.what << ")";
        return ErrorStruct<std::filesystem::path>{err_create.success, err_create.errorCode, err_create.errorInfo, err_create.what};
    }
    ErrorStruct<bool> err_select = api.selectFile(enc_path_full);
    if (!err_select.isSuccess()) {
        PLOG_ERROR << "The file could not be selected (errorCode: " << +err_select.errorCode << ", errorInfo: " << err_select.errorInfo << ", what: " << err_select.what << ")";
        return ErrorStruct<std::filesystem::path>{err_select.success, err_select.errorCode, err_select.errorInfo, err_select.what};
    }
    ErrorStruct<bool> err_dh = api.createDataHeader(password, ds, config.timeout);
    if (!err_dh.isSuccess()) {
        PLOG_ERROR << "The data header could not be created (errorCode: " << +err_dh.errorCode << ", errorInfo: " << err_dh.errorInfo << ", what: " << err_dh.what << ")";
        return ErrorStruct<std::filesystem::path>{err_dh.success, err_dh.errorCode, err_dh.errorInfo, err_dh.what};
    }

    std::ifstream ext_file{path};
    if (!ext_file) {
        PLOG_ERROR << "The file could not be opened (path: " << path.c_str() << ")";
        return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_OPEN, path.c_str()};
    }
    if (!api.encryptData(ext_file).isSuccess()) {
        PLOG_ERROR << "The file could not be encrypted (path: " << path.c_str() << ")";
        return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR, path.c_str()};
    }
    ext_file.close();
    if (config.delete_file) {
        if (!std::filesystem::remove(path)) {
            PLOG_ERROR << "The file could not be deleted (path: " << path.c_str() << ")";
            return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_DELETED, path.c_str()};
        }
    }

    return ErrorStruct<std::filesystem::path>{enc_path_full};
}

ErrorStruct<std::filesystem::path> API::decrypt(std::filesystem::path& path, const std::string& password, const APIConfDecrypt& config) noexcept {
    // decrypts the file at the given path with the given password
    PLOG_VERBOSE << "Decrypting file...";
    std::filesystem::path top_dir = config.dec_top_dir == "" ? path.parent_path() : config.dec_top_dir;
    try {
        API api{FILEMODE_BYTES};
        ErrorStruct<bool> err_select = api.selectFile(path);
        if (!err_select.isSuccess()) {
            PLOG_ERROR << "The file could not be selected (errorCode: " << +err_select.errorCode << ", errorInfo: " << err_select.errorInfo << ", what: " << err_select.what << ")";
            return ErrorStruct<std::filesystem::path>{err_select.success, err_select.errorCode, err_select.errorInfo, err_select.what};
        }
        ErrorStruct<bool> err_dh = api.verifyPassword(password, config.timeout);
        if (!err_dh.isSuccess()) {
            PLOG_ERROR << "The password could not be verified (errorCode: " << +err_dh.errorCode << ", errorInfo: " << err_dh.errorInfo << ", what: " << err_dh.what << ")";
            return ErrorStruct<std::filesystem::path>{err_dh.success, err_dh.errorCode, err_dh.errorInfo, err_dh.what};
        }
        ErrorStruct<std::filesystem::path> err_dec = api.decryptData(top_dir);
        if (!err_dec.isSuccess()) {
            PLOG_ERROR << "The file could not be decrypted (path: " << path.c_str() << ")";
            return err_dec;
        }
        if (config.delete_file) {
            if (!std::filesystem::remove(path)) {
                PLOG_ERROR << "The file could not be deleted (path: " << path.c_str() << ")";
                return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_DELETED, path.c_str()};
            }
        }
        return err_dec;

    } catch (std::exception& e) {
        PLOG_ERROR << "Some error occured while decrypting (path: " << path.c_str() << ", what: " << e.what() << ")";
        return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR, path.c_str(), e.what()};
    }
}

ErrorStruct<std::unique_ptr<FileHandler>> API::_getFileHandler(const std::filesystem::path& file_path) const noexcept {
    // checks if the given file path is valid and sets the file handler
    ErrorStruct<bool> err1 = FileHandler::isValidPath(file_path, true);
    if (!err1.isSuccess()) {
        // the file path is invalid
        PLOG_ERROR << "The file path is invalid (file path: " << file_path.c_str() << ", errorCode: " << +err1.errorCode << ", errorInfo: " << err1.errorInfo << ", what: " << err1.what << ")";
        return ErrorStruct<std::unique_ptr<FileHandler>>{err1.success, err1.errorCode, err1.errorInfo, err1.what};
    }
    try {
        return ErrorStruct<std::unique_ptr<FileHandler>>::createMove(std::move(std::make_unique<FileHandler>(file_path)));
    } catch (const std::exception& e) {
        // something went wrong while creating the file handler
        PLOG_ERROR << "Something went wrong while creating the file handler (what: " << e.what() << ")";
        return ErrorStruct<std::unique_ptr<FileHandler>>{SuccessType::FAIL, ErrorCode::ERR_FILEHANDLER_CREATION, "", e.what()};
    }
}

DataHeaderHelperStruct API::_createDataHeaderIters(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout) const noexcept {
    // creates a DataHeader with the given settings (helper function for createDataHeader)
    if (!ds.isComplete()) {
        PLOG_ERROR << "The given DataHeaderSettingsIters is not complete";
        DataHeaderHelperStruct dhhs = DataHeaderHelperStruct::createMove(ErrorStruct<std::unique_ptr<DataHeader>>{SuccessType::FAIL, ErrorCode::ERR_DATAHEADERSETTINGS_INCOMPLETE, ""});
        return dhhs;
    }
    PLOG_VERBOSE << "Creating data header with iter settings (" << ds << ", timeout: " << timeout << ")";
    DataHeaderHelperStruct dhhs = DataHeaderHelperStruct::createMove(ErrorStruct<std::unique_ptr<DataHeader>>{SuccessType::FAIL, ErrorCode::ERR, ""});
    if (ds.getFileDataMode() != this->file_mode) {
        // the file mode does not match with the file data mode
        PLOG_ERROR << "The provided file mode does not match with selected file (file_mode: " << +ds.getFileDataMode() << ", selected file file_mode: " << +this->file_mode << ")";
        dhhs.errorStruct.errorCode = ErrorCode::ERR_FILEMODE_INVALID;
        dhhs.errorStruct.errorInfo = "The file mode does not match with the file data mode";
        return dhhs;
    }
    DataHeaderParts dhp;
    // copy the datablocks
    dhp.dec_data_blocks = ds.dec_data_blocks;
    dhp.enc_data_blocks = ds.enc_data_blocks;
    try {
        // trying to get the chainhashes
        std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{ds.getChainHash1Mode()});
        std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{ds.getChainHash2Mode()});
        // setting random data for the chainhashes datablocks (salts that are used by the chainhash)
        chd1->generateRandomData();
        chd2->generateRandomData();
        // creating ChainHash structures
        dhp.chainhash1 = ChainHash{ds.getChainHash1Mode(), ds.getChainHash1Iters(), chd1};
        dhp.chainhash2 = ChainHash{ds.getChainHash2Mode(), ds.getChainHash2Iters(), chd2};
        if (dhp.chainhash1.getChainHashData()->getLen() != chd1->getLen() || dhp.chainhash2.getChainHashData()->getLen() != chd2->getLen()) {
            throw std::logic_error("The chainhash data length does not match");
        }

    } catch (const std::exception& e) {
        // something went wrong while creating the chainhashes
        PLOG_ERROR << "Something went wrong while creating the chainhashes (what: " << e.what() << ")";
        dhhs.errorStruct.errorInfo = "Something went wrong while creating the chainhashes";
        dhhs.errorStruct.what = e.what();
        return dhhs;
    }
    // setting up the other parts
    dhp.setFileDataMode(ds.getFileDataMode());
    dhp.setHashMode(ds.getHashMode());
    // create a new shared hash ptr
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(dhp.getHashMode()));

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
    ErrorStruct<Bytes> ch2_err = ChainHashModes::performChainHash(dhp.chainhash2, hash, ch1_err.returnRef(), timeout_copy);
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
    dhp.setValidPasswordHash(ch2_err.returnValue());
    Bytes salt(hash->getHashSize());
    salt.fillrandom();
    dhp.setEncSalt(salt);

    // dataheader parts is now ready to create the dataheader object
    dhhs.errorStruct = DataHeader::setHeaderParts(dhp);
    dhhs.Password_hash(ch1_err.returnValue());  // adding the password hash to the dhhs struct
    return dhhs;
}

DataHeaderHelperStruct API::_createDataHeaderTime(const std::string& password, const DataHeaderSettingsTime& ds) const noexcept {
    // creates a DataHeader with the given settings (helper function for createDataHeader)
    // sets up the return struct
    if (!ds.isComplete()) {
        PLOG_ERROR << "The given DataHeaderSettingsTime is not complete";
        DataHeaderHelperStruct dhhs{ErrorStruct<std::unique_ptr<DataHeader>>{SuccessType::FAIL, ErrorCode::ERR_DATAHEADERSETTINGS_INCOMPLETE, ""}};
        return dhhs;
    }
    PLOG_VERBOSE << "Creating data header with time settings (" << ds << ")";
    DataHeaderHelperStruct dhhs{ErrorStruct<std::unique_ptr<DataHeader>>{SuccessType::FAIL, ErrorCode::ERR, "", ""}};

    if (ds.getFileDataMode() != this->file_mode) {
        // the file mode does not match with the file data mode
        PLOG_ERROR << "The provided file mode does not match with selected file (file_mode: " << +ds.getFileDataMode() << ", selected file file_mode: " << +this->file_mode << ")";
        dhhs.errorStruct.errorCode = ErrorCode::ERR_FILEMODE_INVALID;
        dhhs.errorStruct.errorInfo = "The file mode does not match with the file data mode";
        return dhhs;
    }

    DataHeaderParts dhp;
    // copy the datablocks
    dhp.dec_data_blocks = ds.dec_data_blocks;
    dhp.enc_data_blocks = ds.enc_data_blocks;
    ChainHashTimed ch1;
    ChainHashTimed ch2;
    try {
        // trying to get the chainhashes
        std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{ds.getChainHash1Mode()});
        std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{ds.getChainHash2Mode()});
        // setting random data for the chainhashes datablocks (salts that are used by the chainhash)
        chd1->generateRandomData();
        chd2->generateRandomData();
        // creating ChainHash structures
        ch1 = ChainHashTimed{ds.getChainHash1Mode(), ds.getChainHash1Time(), chd1};
        ch2 = ChainHashTimed{ds.getChainHash2Mode(), ds.getChainHash2Time(), chd2};
        if (ch1.getChainHashData()->getLen() != chd1->getLen() || ch2.getChainHashData()->getLen() != chd2->getLen()) {
            throw std::logic_error("The chainhash data length does not match");
        }

    } catch (const std::exception& e) {
        // something went wrong while creating the chainhashes
        PLOG_ERROR << "Something went wrong while creating the chainhashes (what: " << e.what() << ")";
        dhhs.errorStruct.errorInfo = "Something went wrong while creating the chainhashes";
        dhhs.errorStruct.what = e.what();
        return dhhs;
    }
    // setting up the other parts
    dhp.setFileDataMode(ds.getFileDataMode());
    dhp.setHashMode(ds.getHashMode());
    // create new shared hash ptr
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(dhp.getHashMode()));

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
    ErrorStruct<ChainHashResult> ch2_err = ChainHashModes::performChainHash(ch2, hash, ch1_err.returnRef().result);
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
    dhp.setValidPasswordHash(ch2_err.returnValue().result);
    Bytes salt(hash->getHashSize());
    salt.fillrandom();
    dhp.setEncSalt(salt);

    // dataheader parts is now ready to create the dataheader object
    dhhs.errorStruct = DataHeader::setHeaderParts(dhp);
    dhhs.Password_hash(ch1_err.returnValue().result);  // adding the password hash to the dhhs struct
    return dhhs;
}

ErrorStruct<bool> API::_select_empty_file(std::unique_ptr<FileHandler>&& file) noexcept {
    // select an empty file (no need to set a dataheader etc)
    PLOG_INFO << "File selected (file_path: " << file->getPath() << ")";
    // set the selected file
    this->selected_file = std::move(file);
    this->current_state = std::make_unique<EMPTY_FILE_SELECTED>(this);
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::_select_non_empty_file(std::unique_ptr<FileHandler>&& file) noexcept {
    // sets the file data
    ErrorStruct<std::unique_ptr<DataHeader>> err_dataheader = file->getDataHeader();
    if (!err_dataheader.isSuccess()) {
        // the data header could not be read
        PLOG_ERROR << "The data header could not be read (errorCode: " << +err_dataheader.errorCode << ", errorInfo: " << err_dataheader.errorInfo << ", what: " << err_dataheader.what << ")";
        return ErrorStruct<bool>{err_dataheader.success, err_dataheader.errorCode, err_dataheader.errorInfo, err_dataheader.what};
    }
    this->dh = err_dataheader.returnMove();
    PLOG_INFO << "File selected (file_path: " << file->getPath().c_str() << ")";
    // set the selected file
    this->selected_file = std::move(file);
    this->current_state = std::make_unique<FILE_SELECTED>(this);
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::_deleteFile() noexcept {
    // deletes the working file
    if (!std::filesystem::remove(this->selected_file->getPath())) {
        // the file could not be deleted
        PLOG_ERROR << "The file could not be deleted (deleteFile)";
        return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_DELETED, this->selected_file->getPath().c_str()};
    }
    // reset the selected file
    return this->current_state->unselectFile();
}

ErrorStruct<bool> API::_unselectFile() noexcept {
    // unselects the working file
    this->selected_file = nullptr;
    this->dh = nullptr;
    this->current_state = std::make_unique<INIT>(this);
    return ErrorStruct<bool>{true};
}

API::API(const FModes file_mode) : current_state(std::make_unique<INIT>(this)), file_mode(file_mode), correct_password_hash(Bytes(0)) {
    // constructs the API in a given workflow mode and initializes the private variables
    PLOG_VERBOSE << "API object created (file_mode: " << +file_mode << ")";
    if (!FileModes::isModeValid(file_mode)) {
        PLOG_ERROR << "Invalid file mode passed to API constructor (file_mode: " << +file_mode << ")";
        throw std::invalid_argument("Invalid file mode");
    }
    this->file_data_struct = nullptr;
    this->encrypted = nullptr;
    this->selected_file = nullptr;
    this->dh = nullptr;
}

std::filesystem::path API::getCurrentDirPath() noexcept {
    // gets the current directory path
    return std::filesystem::current_path();
}

ErrorStruct<std::filesystem::path> API::getEncDirPath() noexcept {
    // gets the path of the directory where the .enc files are stored at default
    std::filesystem::path enc_path = "data/enc";
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

ErrorStruct<std::vector<std::string>> API::getAllEncFileNames(const std::filesystem::path& dir) noexcept {
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

ErrorStruct<std::vector<std::string>> API::INIT::getRelevantFileNames(const std::filesystem::path& dir) noexcept {
    // only gets the file names that have the same file mode as the given file data or are empty
    // gets all file names
    PLOG_VERBOSE << "Getting relevant file names (dir: " << dir << ")";
    ErrorStruct<std::vector<std::string>> err = API::getAllEncFileNames(dir);
    if (!err.isSuccess()) {
        PLOG_ERROR << "Could not get all file names (getRelevantFileNames) (err.errorCode: " << +err.errorCode << ", err.errorInfo: " << err.errorInfo << ")";
        return err;
    }
    ErrorStruct<std::vector<std::string>> ret{std::vector<std::string>()};
    for (int i = 0; i < err.returnRef().size(); i++) {
        // checks for every file if it has the same file mode as the given file data
        // build the complete file path
        std::filesystem::path fp = dir / err.returnRef()[i];
        // checks if the file data mode matches with the file mode of the file is empty
        ErrorStruct<std::unique_ptr<FileHandler>> err2 = this->parent->_getFileHandler(fp);
        if (err2.isSuccess()) {
            if (err2.returnRef()->isEmtpy() || err2.returnRef()->isDataHeader(this->parent->file_mode).isSuccess()) {
                // dataheader is valid or file is empty
                // we include the file to the relevant files
                std::vector<std::string> tmp = ret.returnRef();
                tmp.push_back(err.returnValue()[i]);
            }
        }
    }
    return ret;
}

ErrorStruct<bool> API::INIT::createFile(const std::filesystem::path& file_path) noexcept {
    // creates a new .enc file at the given path and validates it
    return FileHandler::createFile(file_path);
}

ErrorStruct<bool> API::INIT::selectFile(const std::filesystem::path& file_path) noexcept {
    // selects the given file as the working file
    PLOG_VERBOSE << "Selecting file (file_path: " << file_path << ")";
    ErrorStruct<std::unique_ptr<FileHandler>> err = this->parent->_getFileHandler(file_path);
    if (!err.isSuccess()) {
        // file path is invalid
        PLOG_ERROR << "The file path is invalid (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
        return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
    }
    if (err.returnRef()->isEmtpy())
        return this->parent->_select_empty_file(err.returnMove());
    else
        return this->parent->_select_non_empty_file(err.returnMove());
}

ErrorStruct<bool> API::FILE_SELECTED::isFileEmpty() const noexcept {
    // file selected is always not empty
    return ErrorStruct<bool>{false};
}

ErrorStruct<bool> API::EMPTY_FILE_SELECTED::isFileEmpty() const noexcept {
    // empty file selected is always empty
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::FILE_SELECTED::deleteFile() noexcept {
    PLOG_VERBOSE << "Deleting file (file_path: " << this->parent->selected_file->getPath().c_str() << ")";
    return this->parent->_deleteFile();
}

ErrorStruct<bool> API::EMPTY_FILE_SELECTED::deleteFile() noexcept {
    PLOG_VERBOSE << "Deleting file (file_path: " << this->parent->selected_file->getPath().c_str() << ")";
    return this->parent->_deleteFile();
}

ErrorStruct<Bytes> API::FILE_SELECTED::getFileContent() noexcept {
    // gets the content of the working file
    try {
        return ErrorStruct<Bytes>{this->parent->selected_file->getAllBytes()};
    } catch (std::exception& e) {
        // the data could not be read
        PLOG_ERROR << "The data could not be read (what: " << e.what() << ")";
        return ErrorStruct<Bytes>{SuccessType::FAIL, ErrorCode::ERR_FILE_READ, this->parent->selected_file->getPath().c_str(), e.what()};
    }
}

ErrorStruct<Bytes> API::EMPTY_FILE_SELECTED::getFileContent() noexcept {
    // empty files have no content
    return ErrorStruct<Bytes>{Bytes(0)};
}

ErrorStruct<bool> API::FILE_SELECTED::unselectFile() noexcept {
    PLOG_VERBOSE << "Unselecting file (file_path: " << this->parent->selected_file->getPath().c_str() << ")";
    return this->parent->_unselectFile();
}

ErrorStruct<bool> API::EMPTY_FILE_SELECTED::unselectFile() noexcept {
    PLOG_VERBOSE << "Unselecting file (file_path: " << this->parent->selected_file->getPath().c_str() << ")";
    return this->parent->_unselectFile();
}

ErrorStruct<bool> API::FILE_SELECTED::verifyPassword(const std::string& password, const u_int64_t timeout) noexcept {
    // timeout=0 means no timeout
    // checks if a password (given from the user to decrypt) is valid for the selected file and returns its hash.
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
    PLOG_VERBOSE << "Verifying password (timeout: " << timeout << ")";
    try {
        // gets the header parts to work with (could throw)
        DataHeaderParts dhp = this->parent->dh->getDataHeaderParts();
        // gets the hash function (could throw)
        std::shared_ptr<Hash> hash = std::move(HashModes::getHash(dhp.getHashMode()));
        // perform the first chain hash (password -> passwordhash)
        Timer timer;
        timer.start();
        ErrorStruct<Bytes> err1 = ChainHashModes::performChainHash(dhp.chainhash1, hash, password, timeout);
        if (!err1.isSuccess()) {
            // the first chain hash failed (due to timeout or other error)
            PLOG_ERROR << "The first chain hash failed (verifyPassword) (err1.success: " << +err1.success << ", err1.errorCode: " << +err1.errorCode << ", err1.errorInfo: " << err1.errorInfo
                       << ", err1.what: " << err1.what << ")";
            return ErrorStruct<bool>{err1.success, err1.errorCode, err1.errorInfo, err1.what};
        }
        // perform the second chain hash (passwordhash -> passwordhashhash = validation hash)
        u_int64_t new_timeout = timeout - timer.peekTime();
        timer.stop();
        if (timeout != 0 && new_timeout <= 0) {
            PLOG_WARNING << "Timeout reached after the first chainhash";
            ErrorStruct<bool> err;
            err.success = SuccessType::TIMEOUT;
            err.errorCode = ErrorCode::ERR_TIMEOUT;
            err.errorInfo = "Timeout reached after the first chainhash";
            return err;
        }
        ErrorStruct<Bytes> err2 = ChainHashModes::performChainHash(dhp.chainhash2, hash, err1.returnRef(), new_timeout);
        if (!err2.isSuccess()) {
            // the second chain hash failed (due to timeout or other error)
            PLOG_ERROR << "The second chain hash failed (verifyPassword) (err2.success: " << +err2.success << ", err2.errorCode: " << +err2.errorCode << ", err2.errorInfo: " << err2.errorInfo
                       << ", err2.what: " << err2.what << ")";
            return ErrorStruct<bool>{err2.success, err2.errorCode, err2.errorInfo, err2.what};
        }
        if (err2.returnValue() == dhp.getValidPasswordHash()) {
            // the password is valid (because the validation hashes match)
            // updating the state
            // setting the correct password hash and dataheader to the application
            PLOG_INFO << "The given password is valid (verifyPassword)";
            this->parent->correct_password_hash = err1.returnValue();
            this->parent->current_state = std::make_unique<PASSWORD_VERIFIED>(this->parent);
            return ErrorStruct<bool>{true};
        }
        // the password is invalid
        PLOG_WARNING << "The given password is invalid (verifyPassword)";
        ErrorStruct<bool> err;
        err.success = SuccessType::FAIL;
        err.errorCode = ErrorCode::ERR_PASSWORD_INVALID;
        return err;

    } catch (const std::exception& e) {
        // some error occurred
        PLOG_ERROR << "Some error occurred while verifying the password (verifyPassword) (what: " << e.what() << ")";
        ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR, "Some error occurred while verifying the password", e.what()};
        return err;
    }
}

ErrorStruct<bool> API::EMPTY_FILE_SELECTED::createDataHeader(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout) noexcept {
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations
    if (!ds.isComplete()) {
        PLOG_ERROR << "The given DataHeaderSettingsIters is not complete";
        ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR_DATAHEADERSETTINGS_INCOMPLETE, ""};
        return err;
    }
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->_createDataHeaderIters(password, ds, timeout);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return ErrorStruct<bool>{dhhs.errorStruct.success, dhhs.errorStruct.errorCode, dhhs.errorStruct.errorInfo, dhhs.errorStruct.what};
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
    // updating the state
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnMove();
    this->parent->file_data_struct = std::make_unique<FileDataStruct>(this->parent->file_mode, std::make_unique<Bytes>(0));
    this->parent->current_state = std::make_unique<DECRYPTED>(this->parent);
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::EMPTY_FILE_SELECTED::createDataHeader(const std::string& password, const DataHeaderSettingsTime& ds) noexcept {
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // you can specify the time (in ms) that the chainhashes should take in the settings
    if (!ds.isComplete()) {
        PLOG_ERROR << "The given DataHeaderSettingsTime is not complete";
        ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR_DATAHEADERSETTINGS_INCOMPLETE, ""};
        return err;
    }
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->_createDataHeaderTime(password, ds);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return ErrorStruct<bool>{dhhs.errorStruct.success, dhhs.errorStruct.errorCode, dhhs.errorStruct.errorInfo, dhhs.errorStruct.what};
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
    // updating the state
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnMove();
    this->parent->file_data_struct = std::make_unique<FileDataStruct>(this->parent->file_mode, std::make_unique<Bytes>(0));
    this->parent->current_state = std::make_unique<DECRYPTED>(this->parent);
    return ErrorStruct<bool>{true};
}

ErrorStruct<std::unique_ptr<FileDataStruct>> API::PASSWORD_VERIFIED::getDecryptedData() noexcept {
    // decrypts the data (requires successful verifyPassword or createDataHeader run)
    // returns the decrypted content (without the data header)
    // uses the password and data header that were passed to verifyPassword (or createDataHeader for new files)
    PLOG_VERBOSE << "Getting decrypted data";
    try {
        // construct the blockchain
        DecryptBlockChain dbc{HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash, this->parent->dh->getDataHeaderParts().getEncSalt()};
        // add the data onto the blockchain
        dbc.addData(this->parent->selected_file->getDataStream(), this->parent->selected_file->getDataSize());
        // get the decrypted data
        std::unique_ptr<FileDataStruct> result = std::make_unique<FileDataStruct>(this->parent->file_mode, std::move(dbc.getResult()));
        this->parent->file_data_struct = nullptr;
        // changes the state
        this->parent->current_state = std::make_unique<DECRYPTED>(this->parent);
        return ErrorStruct<std::unique_ptr<FileDataStruct>>::createMove(std::move(result));
    } catch (const std::exception& e) {
        // something went wrong inside of one of these functions, read what message for more information
        PLOG_ERROR << "Something went wrong while decrypting the data (getDecryptedData) (what: " << e.what() << ")";
        return ErrorStruct<std::unique_ptr<FileDataStruct>>{SuccessType::FAIL, ErrorCode::ERR, "In getDecryptedData: Something went wrong while decrypting the data", e.what()};
    }
}

ErrorStruct<std::filesystem::path> API::PASSWORD_VERIFIED::decryptData(std::filesystem::path dest_dir) noexcept {
    // decrypts the data (requires successful verifyPassword or createDataHeader run)
    // writes the decrypted content to a file in the given directory
    PLOG_VERBOSE << "Decrypting data to file...";
    try {
        std::vector<DataBlock> dec_db = this->parent->dh->getDataHeaderParts().dec_data_blocks;
        std::vector<EncDataBlock> enc_db = this->parent->dh->getDataHeaderParts().enc_data_blocks;

        std::string filename = RNG::get_random_string(10);
        std::string extension = ".dec";

        for (int i = 0; i < dec_db.size(); i++) {
            switch (dec_db[i].type) {
                case DatablockType::FILENAME:
                    filename = bytesToString(dec_db[i].getData());
                    break;
                case DatablockType::FILEEXTENSION:
                    extension = bytesToString(dec_db[i].getData());
                    if (extension[0] != '.') extension = "." + extension;
                    break;
            }
        }
        for (int i = 0; i < enc_db.size(); i++) {
            if (!enc_db[i].isEncrypted()) {
                PLOG_ERROR << "The data is not encrypted (decryptData) but should be";
                return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_DATA_NOT_ENCRYPTED, "In decryptData: The data is not encrypted but should be"};
            }
            switch (enc_db[i].getDecType(HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash)) {
                case DatablockType::FILENAME:
                    filename = bytesToString(enc_db[i].getDec(HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash,
                                                              this->parent->dh->getDataHeaderParts().getEncSalt()));
                    break;
                case DatablockType::FILEEXTENSION:
                    extension = bytesToString(enc_db[i].getDec(HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash,
                                                               this->parent->dh->getDataHeaderParts().getEncSalt()));
                    if (extension[0] != '.') extension = "." + extension;
                    break;
            }
        }

        std::filesystem::path ext_file = dest_dir / (filename + extension);
        if (std::filesystem::exists(ext_file)) {
            PLOG_ERROR << "The file already exists (decryptData) (file_path: " << ext_file << ")";
            return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_EXISTS, "In decryptData: The file already exists"};
        }
        std::ofstream ext_file_stream(ext_file, std::ios::binary);
        if (!ext_file_stream.is_open()) {
            PLOG_ERROR << "The file could not be created (decryptData) (file_path: " << ext_file << ")";
            return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR_FILE_NOT_CREATED, "In decryptData: The file could not be created"};
        }

        // construct the blockchain
        DecryptBlockChainStream dbc{HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash, this->parent->dh->getDataHeaderParts().getEncSalt()};
        // add the data onto the blockchain
        dbc.enc_stream(this->parent->selected_file->getDataStream(), std::move(ext_file_stream));
        this->parent->file_data_struct = nullptr;
        // changes the state
        this->parent->current_state = std::make_unique<FINISHED>(this->parent);
        return ErrorStruct<std::filesystem::path>{ext_file};
    } catch (const std::exception& e) {
        // something went wrong inside of one of these functions, read what message for more information
        PLOG_ERROR << "Something went wrong while decrypting the data (getDecryptedData) (what: " << e.what() << ")";
        return ErrorStruct<std::filesystem::path>{SuccessType::FAIL, ErrorCode::ERR, "In getDecryptedData: Something went wrong while decrypting the data", e.what()};
    }
}

ErrorStruct<bool> API::DECRYPTED::encryptData(std::unique_ptr<FileDataStruct>&& file_data) noexcept {
    // encrypts the data
    // uses the password and data header that were passed to verifyPassword
    PLOG_VERBOSE << "encrypt data...";
    if (!file_data->isComplete()) {
        PLOG_ERROR << "The given FileDataStruct is not complete";
        ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR_FILEDATASTRUCT_INCOMPLETE, ""};
        return err;
    }
    ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR, ""};
    if (file_data->getFileMode() != this->parent->file_mode) {
        // the user wants to encrypt data with a different file mode
        PLOG_ERROR << "The provided file mode does not match with the selected file data mode (encryptData) (provided file mode: " << +file_data->getFileMode()
                   << ", selected file mode: " << +this->parent->file_mode << ")";
        err.errorCode = ErrorCode::ERR_FILEMODE_INVALID;
        err.errorInfo = " In encryptData: The provided file mode does not match with the given file data mode";
        return err;
    }
    try {
        // construct the blockchain
        this->parent->dh->setDataSize(file_data->dec_data->getLen());
        this->parent->dh->calcHeaderBytes(this->parent->correct_password_hash);
        EncryptBlockChain ebc{HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash, this->parent->dh->getDataHeaderParts().getEncSalt()};
        // add the data onto the blockchain
        ebc.addData(std::move(file_data->dec_data));
        file_data->dec_data.reset();
        // get the encrypted data
        this->parent->encrypted = std::move(ebc.getResult());
        this->parent->file_data_struct = std::move(file_data);
        // change the state
        this->parent->current_state = std::make_unique<ENCRYPTED>(this->parent);
        // returns the result
        return ErrorStruct<bool>{true};
    } catch (const std::exception& e) {
        // something went wrong inside of one of these functions, read what message for more information
        PLOG_ERROR << "Something went wrong while encrypting the data (encryptData) (what: " << e.what() << ")";
        err.errorInfo = "In encryptData: Something went wrong while encrypting the data";
        err.what = e.what();
        return err;
    }
}

ErrorStruct<bool> API::DECRYPTED::encryptData(std::ifstream& decrypted_file) noexcept {
    // encrypts the data form the file
    // uses the password and data header that were passed to verifyPassword
    PLOG_VERBOSE << "encrypt data from file...";
    std::streamsize start = decrypted_file.tellg();
    decrypted_file.seekg(0, std::ios::end);
    std::streamsize size = decrypted_file.tellg() - start;
    decrypted_file.seekg(start);
    try {
        // construct the blockchain
        this->parent->dh->setDataSize(size);
        this->parent->dh->calcHeaderBytes(this->parent->correct_password_hash);
        EncryptBlockChainStream ebc{HashModes::getHash(this->parent->dh->getDataHeaderParts().getHashMode()), this->parent->correct_password_hash, this->parent->dh->getDataHeaderParts().getEncSalt()};
        // add the data onto the blockchain
        ErrorStruct<std::ofstream> err_file = this->parent->selected_file->getWriteStreamIfEmpty();
        if (!err_file.isSuccess()) {
            PLOG_ERROR << "Could not get write stream (encryptData) (err_file.success: " << +err_file.success << ", err_file.errorCode: " << +err_file.errorCode
                       << ", err_file.errorInfo: " << err_file.errorInfo << ", err_file.what: " << err_file.what << ")";
            return ErrorStruct<bool>{err_file.success, err_file.errorCode, err_file.errorInfo, err_file.what};
        }
        std::ofstream in_file = err_file.returnMove();
        in_file << this->parent->dh->getHeaderBytes();                  // write the header bytes
        ebc.enc_stream(std::move(decrypted_file), std::move(in_file));  // encrypt the data and write the data to the file
        // change the state
        this->parent->current_state = std::make_unique<FINISHED>(this->parent);
        // returns the result
        return ErrorStruct<bool>{true};
    } catch (const std::exception& e) {
        // something went wrong inside of one of these functions, read what message for more information
        PLOG_ERROR << "Something went wrong while encrypting the data (encryptData) (what: " << e.what() << ")";
        return ErrorStruct<bool>{SuccessType::FAIL, ErrorCode::ERR, "In encryptData: Something went wrong while encrypting the data", e.what()};
        ;
    }
}

ErrorStruct<std::unique_ptr<FileDataStruct>> API::DECRYPTED::getFileData() noexcept {
    if (this->parent->file_data_struct == nullptr) {
        PLOG_ERROR << "The file data struct is null (getFileData)";
        return ErrorStruct<std::unique_ptr<FileDataStruct>>{SuccessType::FAIL, ErrorCode::ERR_FILEDATASTRUCT_NULL, ""};
    }
    return ErrorStruct<std::unique_ptr<FileDataStruct>>::createMove(std::move(this->parent->file_data_struct));
}

ErrorStruct<bool> API::DECRYPTED::changeSalt() noexcept {
    // creates data header with the current settings and password, just changes the salt
    // this call is not expensive because it does not have to chainhash the password
    PLOG_VERBOSE << "Changing salt";
    DataHeaderParts dhp = this->parent->dh->getDataHeaderParts();
    // only changes salt
    Bytes b(dhp.getEncSalt().getLen());
    b.fillrandom();
    dhp.setEncSalt(std::move(b));
    // dataheader parts is now ready to create the dataheader object
    ErrorStruct<std::unique_ptr<DataHeader>> err = DataHeader::setHeaderParts(dhp);
    if (err.isSuccess()) {
        this->parent->dh = err.returnMove();
    } else {
        PLOG_ERROR << "The data header could not be created (changeSalt) (success: " << +err.success << ", errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what
                   << ")";
    }
    return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
}

ErrorStruct<bool> API::DECRYPTED::createDataHeader(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout) noexcept {
    // the new generated header will be used for the next encryption, that means the old password is not valid anymore
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations
    if (!ds.isComplete()) {
        PLOG_ERROR << "The given DataHeaderSettingsIters is not complete";
        ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR_DATAHEADERSETTINGS_INCOMPLETE, ""};
        return err;
    }
    PLOG_VERBOSE << "Creating data header with iterations settings (" << ds << ", timeout: " << timeout << ")";
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->_createDataHeaderIters(password, ds, timeout);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return ErrorStruct<bool>{dhhs.errorStruct.success, dhhs.errorStruct.errorCode, dhhs.errorStruct.errorInfo, dhhs.errorStruct.what};
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
    // updating the data header and the password hash
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnMove();
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::DECRYPTED::createDataHeader(const std::string& password, const DataHeaderSettingsTime& ds) noexcept {
    // the new generated header will be used for the next encryption, that means the old password is not valid anymore
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // you can specify the time (in ms) that the chainhashes should take in the settings
    if (!ds.isComplete()) {
        PLOG_ERROR << "The given DataHeaderSettingsTime is not complete";
        ErrorStruct<bool> err{SuccessType::FAIL, ErrorCode::ERR_DATAHEADERSETTINGS_INCOMPLETE, ""};
        return err;
    }
    PLOG_VERBOSE << "Creating data header with time settings (" << ds << ")";
    // calculates the data header (its a refactored function that is used more than once)
    DataHeaderHelperStruct dhhs = this->parent->_createDataHeaderTime(password, ds);
    if (!dhhs.errorStruct.isSuccess()) {
        // the data header could not be created
        PLOG_ERROR << "The data header could not be created (createDataHeader) (success: " << +dhhs.errorStruct.success << ", errorCode: " << +dhhs.errorStruct.errorCode
                   << ", errorInfo: " << dhhs.errorStruct.errorInfo << ", what: " << dhhs.errorStruct.what << ")";
        return ErrorStruct<bool>{dhhs.errorStruct.success, dhhs.errorStruct.errorCode, dhhs.errorStruct.errorInfo, dhhs.errorStruct.what};
    }

    PLOG_INFO << "The data header was created successfully (createDataHeader)";
    // updating the data header and the password hash
    this->parent->correct_password_hash = dhhs.Password_hash();
    this->parent->dh = dhhs.errorStruct.returnMove();
    return ErrorStruct<bool>{true};
}

ErrorStruct<bool> API::ENCRYPTED::writeToFile(const std::filesystem::path& file_path) noexcept {
    // writes encrypted data to a file adds the dataheader, uses the encrypted data from getEncryptedData
    PLOG_VERBOSE << "Writing to file (file_path: " << file_path << ")";
    // checks if the selected file exists
    ErrorStruct<std::unique_ptr<FileHandler>> err_file = this->parent->_getFileHandler(file_path);
    if (!err_file.isSuccess()) {
        PLOG_ERROR << "The provided file path is invalid (writeToFile) (errorCode: " << +err_file.errorCode << ", errorInfo: " << err_file.errorInfo << ", what: " << err_file.what << ")";
        return ErrorStruct<bool>{err_file.success, err_file.errorCode, err_file.errorInfo, err_file.what};
    }
    ErrorStruct<std::ofstream> err = err_file.returnRef()->getWriteStreamIfEmpty();
    if (err.isSuccess()) {
        Bytes full_data{(int64_t)(this->parent->dh->getHeaderBytes().getLen() + this->parent->encrypted->getLen())};
        this->parent->dh->getHeaderBytes().addcopyToBytes(full_data);  // adds the data header
        this->parent->encrypted->addcopyToBytes(full_data);            // adds the encrypted data
        this->parent->encrypted.reset();
        err.returnRef() << full_data;  // writes the data to the file
        this->parent->current_state = std::make_unique<FINISHED>(this->parent);
    } else {
        PLOG_ERROR << "Some error occurred while writing to the file (writeToFile) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
    }
    return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
}

ErrorStruct<bool> API::ENCRYPTED::writeToFile() noexcept {
    // writes encrypted data to the selected file adds the dataheader, uses the encrypted data from getEncryptedData
    PLOG_VERBOSE << "Writing to selected file (file_path: " << this->parent->selected_file->getPath().c_str() << ")";
    // checks if the selected file exists (it could be deleted in the meantime)
    ErrorStruct<std::ofstream> err = this->parent->selected_file->getWriteStream();
    if (err.isSuccess()) {
        Bytes full_data{(int64_t)(this->parent->dh->getHeaderBytes().getLen() + this->parent->encrypted->getLen())};
        this->parent->dh->getHeaderBytes().addcopyToBytes(full_data);  // adds the data header
        this->parent->encrypted->addcopyToBytes(full_data);            // adds the encrypted data
        this->parent->encrypted.reset();
        err.returnRef() << full_data;  // writes the data to the file
        this->parent->current_state = std::make_unique<FINISHED>(this->parent);
    } else {
        PLOG_FATAL << "Some error occurred while writing to the selected file (writeToFile) (errorCode: " << +err.errorCode << ", errorInfo: " << err.errorInfo << ", what: " << err.what << ")";
    }
    return ErrorStruct<bool>{err.success, err.errorCode, err.errorInfo, err.what};
}
