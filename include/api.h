/*
This Api class is the interface between the front-end and the back-end.
It encapsulates the back-end implementations and provides a simple interface for the front-end.

for full documentation see the documentation of the API class in the documentation folder
*/
#pragma once

#include <filesystem>
#include <iostream>

#include "dataheader.h"
#include "error.h"
#include "file_data.h"
#include "filehandler.h"
#include "logger.h"

// struct that is returned by the API if you decode a file
struct WorkflowDecStruct {
    ErrorStruct<bool> errorStruct;  // information about the success of the decoding
                                    // only contains value if no error occurred
   private:
    std::unique_ptr<FileDataStruct> file_data = nullptr;  // file data struct that contains the decrypted content
    std::optional<DataHeader> dh;                         // data header of the file
    bool moved = false;                                   // if the ptr was moved out
   public:
    void setFileData(std::unique_ptr<FileDataStruct>&& file_data) {
        // sets the file data struct
        this->file_data = std::move(file_data);
    };
    void setDataHeader(const DataHeader dh) {
        // sets the data header
        this->dh = dh;
    };
    FileDataStruct& getFileDataRef() {
        // returns the file data struct
        if (!this->errorStruct.isSuccess()) {
            PLOG_FATAL << "trying to get file data struct while error occurred";
            throw std::logic_error("cannot get file data struct if error occurred");
        }
        if (this->moved) {
            PLOG_FATAL << "trying to get file data struct while it is already moved";
            throw std::logic_error("cannot get file data struct if it is already moved");
        }
        if (this->file_data == nullptr) {
            PLOG_FATAL << "trying to get file data struct while it is not set";
            throw std::logic_error("cannot get file data struct if it is not set");
        }
        return *(this->file_data);
    };

    std::unique_ptr<FileDataStruct> getFileData() {
        // returns the file data struct
        if (!this->errorStruct.isSuccess()) {
            PLOG_FATAL << "trying to get file data struct while error occurred";
            throw std::logic_error("cannot get file data struct if error occurred");
        }
        if (this->moved) {
            PLOG_FATAL << "trying to get file data struct while it is already moved";
            throw std::logic_error("cannot get file data struct if it is already moved");
        }
        if (this->file_data == nullptr) {
            PLOG_FATAL << "trying to get file data struct while it is not set";
            throw std::logic_error("cannot get file data struct if it is not set");
        }
        this->moved = true;
        return std::move(this->file_data);
    };

    DataHeader getDataHeader() {
        // returns the data header
        if (!this->errorStruct.isSuccess()) {
            PLOG_FATAL << "trying to get data header while error occurred";
            throw std::logic_error("cannot get data header if error occurred");
        }
        return this->dh.value();
    };
};

// helper struct that is returned internally the API if you create a data header
struct DataHeaderHelperStruct {
   private:
    DataHeaderHelperStruct() { this->errorStruct = ErrorStruct<std::unique_ptr<DataHeader>>{FAIL, ERR_API_STATE_INVALID, "DataHeaderHelperStruct should not be created without error struct"}; }

   public:
    ErrorStruct<std::unique_ptr<DataHeader>> errorStruct;  // contains the actual data header

    DataHeaderHelperStruct(ErrorStruct<std::unique_ptr<DataHeader>> errorStruct) {
        this->errorStruct = ErrorStruct<std::unique_ptr<DataHeader>>::createMove(std::make_unique<DataHeader>(*errorStruct.returnRef()));
    };
    static DataHeaderHelperStruct createMove(ErrorStruct<std::unique_ptr<DataHeader>>&& errorStruct) {
        DataHeaderHelperStruct dhhs;
        dhhs.errorStruct = std::move(errorStruct);
        return dhhs;
    };
    Bytes Password_hash() {
        // returns the password hash if the chainhash was successful
        if (!this->errorStruct.isSuccess()) {
            PLOG_FATAL << "trying to get password hash from error struct that is not a success";
            throw std::logic_error("cannot get password hash from error struct that is not a success");
        }
        return this->password_hash;
    };
    void Password_hash(const Bytes password_hash) {
        // sets the password hash
        this->password_hash = password_hash;
    };

   private:
    Bytes password_hash = Bytes(0);  // contains the password hash
};

// api config structs
struct APIConfEncrypt {
    HModes hash_mode = HASHMODE_SHA512;
    CHModes chainhash_mode1 = CHAINHASH_CONSTANT_COUNT_SALT;
    CHModes chainhash_mode2 = CHAINHASH_QUADRATIC;
    u_int64_t iters1 = 1000000;
    u_int64_t iters2 = 1000000;
    bool include_filename = true;
    bool include_extension = true;
    bool delete_file = true;
    u_int64_t timeout = 0;
    std::filesystem::path enc_filename = "";  // empty means random
    std::filesystem::path enc_top_dir = "";   // empty means the same as the decrypted file was
};

struct APIConfDecrypt {
    u_int64_t timeout = 0;
    std::filesystem::path dec_top_dir = "";  // empty means the same as the encrypted file was
    bool delete_file = true;
};

class API {
    /*
    API class between the front-end and the back-end
    encapsulates the backend implementations
    */
   private:
    class WorkflowState {
       protected:
        API* parent;
        // deletes saved data (password hash, data header, encrypted data)
        // after this call, the API object is in the same state as after the constructor call
       public:
        void logout(const FModes file_mode) noexcept {
            parent->correct_password_hash = Bytes(0);
            parent->dh = nullptr;
            parent->encrypted = nullptr;
            parent->selected_file = nullptr;
            parent->file_data_struct = nullptr;
            parent->current_state = std::make_unique<INIT>(parent);
        }
        // this logout preserves the file mode
        void logout() noexcept {
            parent->correct_password_hash = Bytes(0);
            parent->dh = nullptr;
            parent->encrypted = nullptr;
            parent->selected_file = nullptr;
            parent->file_data_struct = nullptr;
            parent->current_state = std::make_unique<INIT>(parent);
        }

        // holds all non static functionality of the api
        // gets the names of all .enc files in the given directory which are storing the wished file data (file mode) or are empty
        virtual ErrorStruct<std::vector<std::string>> getRelevantFileNames(const std::filesystem::path& dir) noexcept {
            return ErrorStruct<std::vector<std::string>>{FAIL, ERR_API_STATE_INVALID, "getRelevantFileNames is only available in the INIT state"};
        };
        // creates a new .enc file at the given path (path contains the name of the file)
        virtual ErrorStruct<bool> createFile(const std::filesystem::path& file_path) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "createEncFile is only available in the INIT state"};
        };
        // selects a file (this is now the working file)
        virtual ErrorStruct<bool> selectFile(const std::filesystem::path& file_path) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "selectFile is only available in the INIT state"};
        };

        // gets if the selected file is empty
        virtual ErrorStruct<bool> isFileEmpty() const noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "isFileEmpty is only available in the FILE_SELECTED or EMPTY_FILE_SELECTED state"};
        };

        // deletes the selected .enc file
        virtual ErrorStruct<bool> deleteFile() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "deleteEncFile is only available in the FILE_SELECTED or EMPTY_FILE_SELECTED state"}; };
        // gets the content of the selected file
        // fails if the file contains data that is not encrypted by the algorithm or does not belong to the given file data type
        virtual ErrorStruct<Bytes> getFileContent() noexcept {
            return ErrorStruct<Bytes>{FAIL, ERR_API_STATE_INVALID, "getFileContent is only available in the FILE_SELECTED or EMPTY_FILE_SELECTED state"};
        };
        // unselects a file (this is not longer the working file)
        virtual ErrorStruct<bool> unselectFile() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "selectFile is only available in the FILE_SELECTED or EMPTY_FILE_SELECTED state"}; };
        // checks if a password (given from the user to decrypt) is valid for this file and returns its hash.
        // This call is expensive
        // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
        // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
        virtual ErrorStruct<bool> verifyPassword(const std::string& password, const u_int64_t timeout = 0) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "verifyPassword is only available in the FILE_SELECTED state"};
        };
        // creates a data header for a given password and settings by randomizing the salt and chainhash data
        // This call is expensive because it has to chainhash the password twice to generate a validator.
        // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
        // you can specify the iterations or the time (the chainhash runs until the time is reached to get the iterations)
        virtual ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout = 0) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "createDataHeader is only available in the EMPTY_FILE_SELECTED or DECRYPT state"};
        };
        virtual ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsTime& ds) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "createDataHeader is only available in the EMPTY_FILE_SELECTED or DECRYPTED state"};
        };
        // creates data header with the current settings and password, just changes the salt
        // this call is not expensive because it does not have to chainhash the password
        virtual ErrorStruct<bool> changeSalt() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "changeSalt is only available in the DECRYPTED state"}; };

        // decrypts the data
        // returns the decrypted content (without the data header)
        // uses the password and data header that were passed to verifyPassword
        virtual ErrorStruct<std::unique_ptr<FileDataStruct>> getDecryptedData() noexcept {
            return ErrorStruct<std::unique_ptr<FileDataStruct>>{FAIL, ERR_API_STATE_INVALID, "getDecryptedData is only available in the PASSWORD_VERIFIED state"};
        };
        // decrypts the data and writes it to a file in the given directory (filename and extension are taken from the dataheader)
        // .dec is default extension and the name is randomized if not specified in the dataheader
        virtual ErrorStruct<std::filesystem::path> decryptData(std::filesystem::path dest_dir) noexcept {
            return ErrorStruct<std::filesystem::path>{FAIL, ERR_API_STATE_INVALID, "decryptData is only available in the PASSWORD_VERIFIED state"};
        }

        // gets the file data struct
        // it stores the file mode as well as the decrypted file content
        virtual ErrorStruct<std::unique_ptr<FileDataStruct>> getFileData() noexcept {
            return ErrorStruct<std::unique_ptr<FileDataStruct>>{FAIL, ERR_API_STATE_INVALID, "getFileData is only available in the DECRYPTED state"};
        };
        // encrypts the data and returns the encrypted data
        // uses the password and data header that were passed to verifyPassword
        virtual ErrorStruct<bool> encryptData(std::unique_ptr<FileDataStruct>&& file_data) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "encryptData is only available in the DECRYPTED state"};
        };
        virtual ErrorStruct<bool> encryptData(std::ifstream& decrypted_file) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "encryptData is only available in the DECRYPTED state"};
        };

        // writes encrypted data to the selected file adds the dataheader, uses the encrypted data from getEncryptedData
        virtual ErrorStruct<bool> writeToFile() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "writeToFile is only available in the ENCRYPTED state"}; };
        // writes encrypted data to a file adds the dataheader, uses the encrypted data from getEncryptedData
        virtual ErrorStruct<bool> writeToFile(const std::filesystem::path& file_path) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "writeToFile is only available in the ENCRYPTED state"};
        };

       public:
        WorkflowState(API* x) : parent(x){};
    };

    class INIT : public WorkflowState {
       public:
        INIT(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to INIT"; };
        ErrorStruct<std::vector<std::string>> getRelevantFileNames(const std::filesystem::path& dir) noexcept override;
        ErrorStruct<bool> createFile(const std::filesystem::path& file_path) noexcept override;
        ErrorStruct<bool> selectFile(const std::filesystem::path& file_path) noexcept override;
    };

    class FILE_SELECTED : public WorkflowState {
       public:
        FILE_SELECTED(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to FILE_SELECTED"; };
        ErrorStruct<bool> isFileEmpty() const noexcept override;
        ErrorStruct<bool> deleteFile() noexcept override;
        ErrorStruct<Bytes> getFileContent() noexcept override;
        ErrorStruct<bool> unselectFile() noexcept override;
        ErrorStruct<bool> verifyPassword(const std::string& password, const u_int64_t timeout = 0) noexcept override;
    };

    class EMPTY_FILE_SELECTED : public WorkflowState {
       public:
        EMPTY_FILE_SELECTED(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to EMPTY_FILE_SELECTED"; };
        ErrorStruct<bool> isFileEmpty() const noexcept override;
        ErrorStruct<bool> deleteFile() noexcept override;
        ErrorStruct<Bytes> getFileContent() noexcept override;
        ErrorStruct<bool> unselectFile() noexcept override;
        ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout = 0) noexcept override;
        ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsTime& ds) noexcept override;
    };

    class PASSWORD_VERIFIED : public WorkflowState {
       public:
        PASSWORD_VERIFIED(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to PASSWORD_VERIFIED"; };
        ErrorStruct<std::unique_ptr<FileDataStruct>> getDecryptedData() noexcept override;
        ErrorStruct<std::filesystem::path> decryptData(std::filesystem::path dest_dir) noexcept override;
    };

    class DECRYPTED : public WorkflowState {
       public:
        DECRYPTED(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to DECRYPTED"; };
        ErrorStruct<bool> encryptData(std::unique_ptr<FileDataStruct>&& file_data) noexcept override;
        ErrorStruct<bool> encryptData(std::ifstream& decrypted_file) noexcept override;
        ErrorStruct<std::unique_ptr<FileDataStruct>> getFileData() noexcept override;
        ErrorStruct<bool> changeSalt() noexcept override;
        ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout = 0) noexcept override;
        ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsTime& ds) noexcept override;
    };

    class ENCRYPTED : public WorkflowState {
       public:
        ENCRYPTED(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to ENCRYPTED"; };
        ErrorStruct<bool> writeToFile(const std::filesystem::path& file_path) noexcept override;
        ErrorStruct<bool> writeToFile() noexcept override;
    };

    class FINISHED : public WorkflowState {
       public:
        FINISHED(API* x) : WorkflowState(x) { PLOG_DEBUG << "API state changed to FINISHED"; };
    };

    // the current state of the API (makes sure that the API is used correctly)
    std::unique_ptr<WorkflowState> current_state;
    FModes file_mode;                                  // the file mode that should be used for the file
    std::unique_ptr<FileDataStruct> file_data_struct;  // the user can construct a file data object from this struct
    Bytes correct_password_hash;                       // the correct password hash for the dataheader
    std::unique_ptr<Bytes> encrypted;
    // setting standard data header
    std::unique_ptr<DataHeader> dh;  // the data header for the correct password
    // stores the handler to the working file
    std::unique_ptr<FileHandler> selected_file;

    // utility functions
    // gets the file handler for the given file path
    ErrorStruct<std::unique_ptr<FileHandler>> _getFileHandler(const std::filesystem::path& file_path) const noexcept;

    // does the most work for creating a new dataheader from DataHeaderSettingsIters
    DataHeaderHelperStruct _createDataHeaderIters(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout = 0) const noexcept;

    // does the most work for creating a new dataheader from DataHeaderSettingsTime
    DataHeaderHelperStruct _createDataHeaderTime(const std::string& password, const DataHeaderSettingsTime& ds) const noexcept;

    ErrorStruct<bool> _select_empty_file(std::unique_ptr<FileHandler>&& file) noexcept;

    ErrorStruct<bool> _select_non_empty_file(std::unique_ptr<FileHandler>&& file) noexcept;

    ErrorStruct<bool> _deleteFile() noexcept;

    ErrorStruct<bool> _unselectFile() noexcept;

   public:
    // constructs the api with the file mode that should be worked with
    API(const FModes file_mode);

    // WORK
    // //*************** WORKFLOW 1 *****************
    // // decoder: gets the data header and the file data struct from the file
    // ErrorStruct<WorkflowDecStruct> runWorkflow1dec(const std::filesystem::path file_path, const std::string password) noexcept;
    // // you can work with the file data object gotten from the file data struct gotten from the WorkflowDecStruct
    // // call createDataHeader in between to change the encryption settings
    // // encoder: writes the file data (gotten from the object.getFileData()) and data header to the file
    // // this can fail due to a non valid file. You have to call setFile() manually before calling this function again
    // ErrorStruct<bool> runWorkflow1enc(const std::filesystem::path file_path, const FileDataStruct file_data) noexcept;

    // //*************** WORKFLOW 2 *****************
    // // create a new file with createEncFile()
    // // create a new dataheader first with createDataHeader()
    // // call getNewFileData() to get an empty file struct
    // // you can create the corresponding file data object from the struct
    // // you can work with the file data object
    // // encoder: writes the file data (gotten from the object.getFileData()) and data header to the file
    // // this fails if file_path is not the same as in createEncFile(). You have to call setFile() manually before calling this function again
    // ErrorStruct<bool> runWorkflow2enc(const std::filesystem::path file_path, const FileDataStruct file_data) noexcept;

    // FILE STREAMS
    static ErrorStruct<std::filesystem::path> encrypt(std::filesystem::path& path, const std::string& password, const APIConfEncrypt& config = APIConfEncrypt()) noexcept;
    static ErrorStruct<std::filesystem::path> decrypt(std::filesystem::path& path, const std::string& password, const APIConfDecrypt& config = APIConfDecrypt()) noexcept;

    // gets the path of the current working directory
    static std::filesystem::path getCurrentDirPath() noexcept;

    // gets the path of the directory where the .enc files are stored at default
    static ErrorStruct<std::filesystem::path> getEncDirPath() noexcept;

    // gets the names of all .enc files in the given directory
    static ErrorStruct<std::vector<std::string>> getAllEncFileNames(const std::filesystem::path& dir) noexcept;

    // gets the names of all .enc files in the given directory which are storing the wished file data (file mode) or are empty
    ErrorStruct<std::vector<std::string>> getRelevantFileNames(const std::filesystem::path& dir) noexcept {
        PLOG_DEBUG << "API call made (getRelevantFileNames) with dir: " << dir;
        return this->current_state->getRelevantFileNames(dir);
    }

    // creates a new .enc file at the given path (path contains the name of the file)
    ErrorStruct<bool> createFile(const std::filesystem::path& file_path) noexcept {
        PLOG_DEBUG << "API call made (createFile) with file_path: " << file_path;
        return this->current_state->createFile(file_path);
    }

    // selects the file that should be worked with
    // fails if the file contains data that is not encrypted by the algorithm or does not belong to the given file data type
    ErrorStruct<bool> selectFile(const std::filesystem::path& file_path) noexcept {
        PLOG_DEBUG << "API call made (selectFile) with file_path: " << file_path;
        return this->current_state->selectFile(file_path);
    }

    // checks if the selected file is empty
    ErrorStruct<bool> isFileEmpty() const noexcept {
        PLOG_DEBUG << "API call made (isFileEmpty)";
        return this->current_state->isFileEmpty();
    }

    // unselects the current file (it is no longer worked with this file)
    ErrorStruct<bool> unselectFile() noexcept {
        PLOG_DEBUG << "API call made (unselectFile)";
        return this->current_state->unselectFile();
    }

    // deletes the selected .enc file
    ErrorStruct<bool> deleteFile() noexcept {
        PLOG_DEBUG << "API call made (deleteFile)";
        return this->current_state->deleteFile();
    }

    // gets the content of the selected file
    ErrorStruct<Bytes> getFileContent() noexcept {
        PLOG_DEBUG << "API call made (getFileContent)";
        return this->current_state->getFileContent();
    }

    // only call this function on non empty files
    // checks if a password (given from the user to decrypt) is valid for this file and returns its hash.
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
    ErrorStruct<bool> verifyPassword(const std::string& password, const u_int64_t timeout = 0) noexcept {
        PLOG_DEBUG << "API call made (verifyPassword) with timeout: " << timeout;
        return this->current_state->verifyPassword(password, timeout);
    }

    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations or the time (the chainhash runs until the time is reached to get the iterations)
    ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsIters& ds, const u_int64_t timeout = 0) noexcept {
        if (!ds.isComplete()) {
            PLOG_ERROR << "API call made (createDataHeader) with incomplete DataHeaderSettingsIters";
            return ErrorStruct<bool>{FAIL, ERR_DATAHEADERSETTINGS_INCOMPLETE, "API call made (createDataHeader) with incomplete DataHeaderSettingsIters"};
        }
        PLOG_DEBUG << "API call made (createDataHeader) with timeout: " << timeout << " and Iterations: " << ds.getChainHash1Iters() << " and " << ds.getChainHash2Iters();
        return this->current_state->createDataHeader(password, ds, timeout);
    }
    ErrorStruct<bool> createDataHeader(const std::string& password, const DataHeaderSettingsTime& ds) noexcept {
        if (!ds.isComplete()) {
            PLOG_ERROR << "API call made (createDataHeader) with incomplete DataHeaderSettingsTime";
            return ErrorStruct<bool>{FAIL, ERR_DATAHEADERSETTINGS_INCOMPLETE, "API call made (createDataHeader) with incomplete DataHeaderSettingsTime"};
        }
        PLOG_DEBUG << "API call made (createDataHeader) with Time: " << ds.getChainHash1Time() << " and " << ds.getChainHash2Time();
        return this->current_state->createDataHeader(password, ds);
    }

    // creates data header with the current settings and password, just changes the salt
    // this call is not expensive because it does not have to chainhash the password
    ErrorStruct<bool> changeSalt() noexcept {
        PLOG_DEBUG << "API call made (changeSalt)";
        return this->current_state->changeSalt();
    }

    // decrypts the data (requires successful verifyPassword run)
    // returns the decrypted content (without the data header)
    // uses the password and data header from verifyPassword
    ErrorStruct<std::unique_ptr<FileDataStruct>> getDecryptedData() noexcept {
        PLOG_DEBUG << "API call made (getDecryptedData)";
        return this->current_state->getDecryptedData();
    }
    // decrypts the data (requires successful verifyPassword run) and writes it to a file in the given directory (filename and extension are taken from the dataheader)
    ErrorStruct<std::filesystem::path> decryptData(std::filesystem::path dest_dir) noexcept {
        PLOG_DEBUG << "API call made (decryptData)";
        return this->current_state->decryptData(dest_dir);
    }

    // gets the file data struct
    // it stores the file mode as well as the decrypted file content
    ErrorStruct<std::unique_ptr<FileDataStruct>> getFileData() noexcept {
        PLOG_DEBUG << "API call made (getFileData)";
        return this->current_state->getFileData();
    }

    // encrypts the data (requires successful getDecryptedData or createDataHeader run) returns the encrypted data
    // uses the password and data header from verifyPassword or createDataHeader
    ErrorStruct<bool> encryptData(std::unique_ptr<FileDataStruct>&& file_data) noexcept {
        PLOG_DEBUG << "API call made (encryptData)";
        return this->current_state->encryptData(std::move(file_data));
    }
    ErrorStruct<bool> encryptData(std::ifstream& decrypted_file) noexcept {
        PLOG_DEBUG << "API call made (encryptData) with ifstream";
        return this->current_state->encryptData(decrypted_file);
    }

    // writes encrypted data to a file adds the dataheader (requires successful getEncryptedData run)
    ErrorStruct<bool> writeToFile() noexcept {
        // writes to selected file
        PLOG_DEBUG << "API call made (writeToFile)";
        return this->current_state->writeToFile();
    }
    ErrorStruct<bool> writeToFile(const std::filesystem::path& file_path) noexcept {
        // writes to file at file_path (has to be empty)
        PLOG_DEBUG << "API call made (writeToFile) with file_path: " << file_path;
        return this->current_state->writeToFile(file_path);
    }

    // deletes saved data (password hash, data header, encrypted data)
    // after this call, the API object is in the same state as after the constructor call
    void logout(const FModes file_mode) noexcept {
        PLOG_DEBUG << "API call made (logout) with new file_mode" << +file_mode;
        this->current_state->logout(file_mode);
    }
    // this call preserves the file mode
    void logout() noexcept {
        PLOG_DEBUG << "API call made (logout)";
        this->current_state->logout();
    }
};