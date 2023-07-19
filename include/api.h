/*
This Api class is the interface between the front-end and the back-end.
It encapsulates the back-end implementations and provides a simple interface for the front-end.

for full documentation see the documentation of the API class in the documentation folder
*/
#pragma once

#include <filesystem>
#include <iostream>

#include "logger.h"

#include "dataheader.h"
#include "error.h"
#include "file_data.h"

// struct that is returned by the API if you decode a file
struct WorkflowDecStruct {
    ErrorStruct<bool> errorStruct;  // information about the success of the decoding
    // only contains value if no error occurred
    std::optional<FileDataStruct> file_data;  // file data struct that contains the decrypted content
    std::optional<DataHeader> dh;             // data header of the file
};

// helper struct that is returned internally the API if you create a data header
struct DataHeaderHelperStruct {
    ErrorStruct<DataHeader> errorStruct;  // contains the actual data header
    DataHeaderHelperStruct(const ErrorStruct<DataHeader> errorStruct) : errorStruct(errorStruct){};
    Bytes Password_hash() {
        // returns the password hash if the chainhash was successful
        if (this->errorStruct.isSuccess()) {
            return this->password_hash;
        } else {
            throw std::logic_error("cannot get password hash from error struct that is not a success");
        }
    };
    void Password_hash(const Bytes password_hash) {
        // sets the password hash
        this->password_hash = password_hash;
    };

   private:
    Bytes password_hash;  // contains the password hash
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
            parent->correct_password_hash = Bytes();
            parent->dh = DataHeader(HModes(STANDARD_HASHMODE));
            parent->encrypted_data = Bytes();
            parent->selected_file = std::filesystem::path();
            parent->file_data_struct = FileDataStruct{file_mode, Bytes()};
            parent->current_state = INIT{parent};
        }
        // this logout preserves the file mode
        void logout() noexcept {
            parent->correct_password_hash = Bytes();
            parent->dh = DataHeader(HModes(STANDARD_HASHMODE));
            parent->encrypted_data = Bytes();
            parent->selected_file = std::filesystem::path();
            parent->file_data_struct = FileDataStruct{this->parent->file_data_struct.file_mode, Bytes()};
            parent->current_state = INIT{parent};
        }

        // holds all non static functionality of the api
        // gets the names of all .enc files in the given directory which are storing the wished file data (file mode) or are empty
        virtual ErrorStruct<std::vector<std::string>> getRelevantFileNames(const std::filesystem::path dir) noexcept {
            return ErrorStruct<std::vector<std::string>>{FAIL, ERR_API_STATE_INVALID, "getRelevantFileNames is only available in the INIT state"};
        };
        // creates a new .enc file at the given path (path contains the name of the file)
        virtual ErrorStruct<bool> createFile(const std::filesystem::path file_path) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "createEncFile is only available in the INIT state"};
        };
        // selects a file (this is now the working file)
        virtual ErrorStruct<bool> selectFile(const std::filesystem::path file_path) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "selectFile is only available in the INIT state"};
        };

        // gets if the selected file is empty
        virtual ErrorStruct<bool> isFileEmpty() const noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "isFileEmpty is only available in the FILE_SELECTED state"}; };

        // deletes the given .enc file
        virtual ErrorStruct<bool> deleteFile() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "deleteEncFile is only available in the FILE_SELECTED state"}; };
        // gets the content of the selected file
        // fails if the file contains data that is not encrypted by the algorithm or does not belong to the given file data type
        virtual ErrorStruct<Bytes> getFileContent() noexcept { return ErrorStruct<Bytes>{FAIL, ERR_API_STATE_INVALID, "getFileContent is only available in the FILE_SELECTED state"}; };
        // unselects a file (this is not longer the working file)
        virtual ErrorStruct<bool> unselectFile() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "selectFile is only available in the FILE_SELECTED state"}; };
        // checks if a password (given from the user to decrypt) is valid for this file and returns its hash.
        // This call is expensive
        // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
        // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
        virtual ErrorStruct<Bytes> verifyPassword(const std::string password, const u_int64_t timeout = 0) noexcept {
            return ErrorStruct<Bytes>{FAIL, ERR_API_STATE_INVALID, "verifyPassword is only available in the FILE_SELECTED state"};
        };
        // creates a data header for a given password and settings by randomizing the salt and chainhash data
        // This call is expensive because it has to chainhash the password twice to generate a validator.
        // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
        // you can specify the iterations or the time (the chainhash runs until the time is reached to get the iterations)
        virtual ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout = 0) noexcept {
            return ErrorStruct<DataHeader>{FAIL, ERR_API_STATE_INVALID, "createDataHeader is only available in the FILE_SELECTED or DECRYPT state"};
        };
        virtual ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept {
            return ErrorStruct<DataHeader>{FAIL, ERR_API_STATE_INVALID, "createDataHeader is only available in the FILE_SELECTED or DECRYPTED state"};
        };
        // creates data header with the current settings and password, just changes the salt
        // this call is not expensive because it does not have to chainhash the password
        virtual ErrorStruct<DataHeader> changeSalt() noexcept { return ErrorStruct<DataHeader>{FAIL, ERR_API_STATE_INVALID, "changeSalt is only available in the DECRYPTED state"}; };

        // decrypts the data
        // returns the decrypted content (without the data header)
        // uses the password and data header that were passed to verifyPassword
        virtual ErrorStruct<FileDataStruct> getDecryptedData() noexcept {
            return ErrorStruct<FileDataStruct>{FAIL, ERR_API_STATE_INVALID, "getDecryptedData is only available in the PASSWORD_VERIFIED state"};
        };
        // gets the file data struct
        // it stores the file mode as well as the decrypted file content
        virtual ErrorStruct<FileDataStruct> getFileData() noexcept { return ErrorStruct<FileDataStruct>{FAIL, ERR_API_STATE_INVALID, "getFileData is only available in the DECRYPTED state"}; }
        // encrypts the data and returns the encrypted data
        // uses the password and data header that were passed to verifyPassword
        virtual ErrorStruct<Bytes> getEncryptedData(const FileDataStruct file_data) noexcept {
            return ErrorStruct<Bytes>{FAIL, ERR_API_STATE_INVALID, "getEncryptedData is only available in the DECRYPTED state"};
        };
        // writes encrypted data to the selected file adds the dataheader, uses the encrypted data from getEncryptedData
        virtual ErrorStruct<bool> writeToFile() noexcept { return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "writeToFile is only available in the ENCRYPTED state"}; };
        // writes encrypted data to a file adds the dataheader, uses the encrypted data from getEncryptedData
        virtual ErrorStruct<bool> writeToFile(const std::filesystem::path file_path) noexcept {
            return ErrorStruct<bool>{FAIL, ERR_API_STATE_INVALID, "writeToFile is only available in the ENCRYPTED state"};
        };

       public:
        WorkflowState(API* x) : parent(x){};
    };

    class INIT : public WorkflowState {
       public:
        INIT(API* x) : WorkflowState(x){
            PLOG_DEBUG << "API state changed to INIT";
        };
        ErrorStruct<std::vector<std::string>> getRelevantFileNames(const std::filesystem::path dir) noexcept override;
        ErrorStruct<bool> createFile(const std::filesystem::path file_path) noexcept override;
        ErrorStruct<bool> selectFile(const std::filesystem::path file_path) noexcept override;
    };

    class FILE_SELECTED : public WorkflowState {
       public:
        FILE_SELECTED(API* x) : WorkflowState(x){
            PLOG_DEBUG << "API state changed to FILE_SELECTED";
        };
        ErrorStruct<bool> isFileEmpty() const noexcept override;
        ErrorStruct<bool> deleteFile() noexcept override;
        ErrorStruct<Bytes> getFileContent() noexcept override;
        ErrorStruct<bool> unselectFile() noexcept override;
        ErrorStruct<Bytes> verifyPassword(const std::string password, const u_int64_t timeout = 0) noexcept override;
        ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout = 0) noexcept override;
        ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept override;
    };

    class PASSWORD_VERIFIED : public WorkflowState {
       public:
        PASSWORD_VERIFIED(API* x) : WorkflowState(x){
            PLOG_DEBUG << "API state changed to PASSWORD_VERIFIED";
        };
        ErrorStruct<FileDataStruct> getDecryptedData() noexcept override;
    };

    class DECRYPTED : public WorkflowState {
       public:
        DECRYPTED(API* x) : WorkflowState(x){
            PLOG_DEBUG << "API state changed to DECRYPTED";
        };
        ErrorStruct<Bytes> getEncryptedData(const FileDataStruct file_data) noexcept override;
        ErrorStruct<FileDataStruct> getFileData() noexcept override;
        ErrorStruct<DataHeader> changeSalt() noexcept override;
        ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout = 0) noexcept override;
        ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept override;
    };

    class ENCRYPTED : public WorkflowState {
       public:
        ENCRYPTED(API* x) : WorkflowState(x){
            PLOG_DEBUG << "API state changed to ENCRYPTED";
        };
        ErrorStruct<bool> writeToFile(const std::filesystem::path file_path) noexcept override;
        ErrorStruct<bool> writeToFile() noexcept override;
    };

    class FINISHED : public WorkflowState {
       public:
        FINISHED(API* x) : WorkflowState(x){
            PLOG_DEBUG << "API state changed to FINISHED";
        };
    };

    // the current state of the API (makes sure that the API is used correctly)
    WorkflowState current_state;
    // the file mode that should be used for the file is stored in the FileDataStruct
    FileDataStruct file_data_struct;  // the user can construct a file data object from this struct
    Bytes correct_password_hash;      // the correct password hash for the dataheader
    // setting standard data header
    DataHeader dh;  // the data header for the correct password
    // stores the file content that was encrypted by the algorithm
    // only this file content is valid to write to a file
    Bytes encrypted_data;
    // stores the path to the file from that the data was read
    // or an different file if the user wants to write to a different file (setFile())
    std::filesystem::path selected_file;
    bool file_empty = false;  // selected file is empty?

    // utility functions
    // checks if an file path is valid
    ErrorStruct<bool> checkFilePath(const std::filesystem::path file_path, const bool should_exist = false) const noexcept;
    // checks if the file data header is valid or the file is empty
    // that means there has to be a valid header with the file mode provided from the api
    // the return value is false if the file is empty
    ErrorStruct<bool> checkFileData(const std::filesystem::path file_path) const noexcept;

    // adds the extension to the file path if it is not already there
    ErrorStruct<std::filesystem::path> addExtension(const std::filesystem::path file_path) const noexcept;

    // gets the file content from the file
    ErrorStruct<Bytes> getFileContent(const std::filesystem::path file_path) const noexcept;

    // does the most work for creating a new dataheader from DataHeaderSettingsIters
    DataHeaderHelperStruct createDataHeaderIters(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout = 0) const noexcept;

    // does the most work for creating a new dataheader from DataHeaderSettingsTime
    DataHeaderHelperStruct createDataHeaderTime(const std::string password, const DataHeaderSettingsTime ds) const noexcept;

    // writes the file content to the file without checking if the file is valid
    ErrorStruct<bool> writeFile(const std::filesystem::path file_path) const noexcept;

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

    // gets the path of the current working directory
    static std::filesystem::path getCurrentDirPath() noexcept;

    // gets the path of the directory where the .enc files are stored at default
    static ErrorStruct<std::filesystem::path> getEncDirPath() noexcept;

    // gets the names of all .enc files in the given directory
    static ErrorStruct<std::vector<std::string>> getAllEncFileNames(const std::filesystem::path dir) noexcept;

    // gets the names of all .enc files in the given directory which are storing the wished file data (file mode) or are empty
    ErrorStruct<std::vector<std::string>> getRelevantFileNames(const std::filesystem::path dir) noexcept {
        PLOG_DEBUG << "API call made (getRelevantFileNames) with dir: " << dir;
        return this->current_state.getRelevantFileNames(dir); 
    }

    // creates a new .enc file at the given path (path contains the name of the file)
    ErrorStruct<bool> createFile(const std::filesystem::path file_path) noexcept { 
        PLOG_DEBUG << "API call made (createFile) with file_path: " << file_path;
        return this->current_state.createFile(file_path);
    }

    // selects the file that should be worked with
    // fails if the file contains data that is not encrypted by the algorithm or does not belong to the given file data type
    ErrorStruct<bool> selectFile(const std::filesystem::path file_path) noexcept {
        PLOG_DEBUG << "API call made (selectFile) with file_path: " << file_path;
        return this->current_state.selectFile(file_path);
    }

    // checks if the selected file is empty
    ErrorStruct<bool> isFileEmpty() const noexcept { 
        PLOG_DEBUG << "API call made (isFileEmpty)";
        return this->current_state.isFileEmpty();
    }

    // unselects the current file (it is no longer worked with this file)
    ErrorStruct<bool> unselectFile() noexcept {
        PLOG_DEBUG << "API call made (unselectFile)";
        return this->current_state.unselectFile();
    }

    // deletes the selected .enc file
    ErrorStruct<bool> deleteFile() noexcept { 
        PLOG_DEBUG << "API call made (deleteFile)";
        return this->current_state.deleteFile(); 
    }

    // gets the content of the selected file
    ErrorStruct<Bytes> getFileContent() noexcept { 
        PLOG_DEBUG << "API call made (getFileContent)";
        return this->current_state.getFileContent();
    }

    // extract the data header from the file content
    static ErrorStruct<DataHeader> getDataHeader(const Bytes file_content) noexcept;

    // extract the content of the file without the data header
    static ErrorStruct<Bytes> getData(const Bytes file_content) noexcept;

    // only call this function on non empty files
    // checks if a password (given from the user to decrypt) is valid for this file and returns its hash.
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
    ErrorStruct<Bytes> verifyPassword(const std::string password, const u_int64_t timeout = 0) noexcept {
        PLOG_DEBUG << "API call made (verifyPassword) with timeout: " << timeout;
        return this->current_state.verifyPassword(password, timeout); 
    }

    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations or the time (the chainhash runs until the time is reached to get the iterations)
    ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout = 0) noexcept {
        PLOG_DEBUG << "API call made (createDataHeader) with timeout: " << timeout << " and Iterations" << ds.chainhash1_iters << " and " << ds.chainhash2_iters;
        return this->current_state.createDataHeader(password, ds, timeout);
    }
    ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept {
        PLOG_DEBUG << "API call made (createDataHeader) with Time" << ds.chainhash1_time << " and " << ds.chainhash2_time;
        return this->current_state.createDataHeader(password, ds);
    }

    // creates data header with the current settings and password, just changes the salt
    // this call is not expensive because it does not have to chainhash the password
    ErrorStruct<DataHeader> changeSalt() noexcept {
        PLOG_DEBUG << "API call made (changeSalt)";
        return this->current_state.changeSalt();
    }

    // decrypts the data (requires successful verifyPassword run)
    // returns the decrypted content (without the data header)
    // uses the password and data header from verifyPassword
    ErrorStruct<FileDataStruct> getDecryptedData() noexcept {
        PLOG_DEBUG << "API call made (getDecryptedData)";
        return this->current_state.getDecryptedData();
    }

    // gets the file data struct
    // it stores the file mode as well as the decrypted file content
    ErrorStruct<FileDataStruct> getFileData() noexcept {
        PLOG_DEBUG << "API call made (getFileData)";
        return this->current_state.getFileData();
    }

    // encrypts the data (requires successful getDecryptedData or createDataHeader run) returns the encrypted data
    // uses the password and data header from verifyPassword or createDataHeader
    ErrorStruct<Bytes> getEncryptedData(const FileDataStruct file_data) noexcept {
        PLOG_DEBUG << "API call made (getEncryptedData)";
        return this->current_state.getEncryptedData(file_data);
    }

    // writes encrypted data to a file adds the dataheader (requires successful getEncryptedData run)
    ErrorStruct<bool> writeToFile() noexcept {
        // writes to selected file
        PLOG_DEBUG << "API call made (writeToFile)";
        return this->current_state.writeToFile();
    }
    ErrorStruct<bool> writeToFile(const std::filesystem::path file_path) noexcept {
        // writes to file at file_path (has to be empty)
        PLOG_DEBUG << "API call made (writeToFile) with file_path: " << file_path;
        return this->current_state.writeToFile(file_path);
    }

    // deletes saved data (password hash, data header, encrypted data)
    // after this call, the API object is in the same state as after the constructor call
    void logout(const FModes file_mode) noexcept { 
        PLOG_DEBUG << "API call made (logout) with new file_mode" << +file_mode;
        this->current_state.logout(file_mode); 
    }
    // this call preserves the file mode
    void logout() noexcept {
        PLOG_DEBUG << "API call made (logout)";
        this->current_state.logout();
    }
};