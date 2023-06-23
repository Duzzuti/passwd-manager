/*
This Api class is the interface between the front-end and the back-end.
It encapsulates the back-end implementations and provides a simple interface for the front-end.

for full documenation see the documentation of the API class in the documentation folder
*/
#pragma once

#include <filesystem>
#include <iostream>

#include "dataheader.h"
#include "error.h"
#include "file_data.h"

// tells the API which workflow to use
enum Workflow {
    WORK_WITH_OLD_FILE,  // WORKFLOW 1: WORK WITH EXISTING FILE
    WORK_WITH_NEW_FILE,  // WORKFLOW 2: WORK WITH NEW FILE
    DELETE_FILE          // WORKFLOW 3: DELETE FILE
};

// struct that is returned by the API if you decode a file
struct WorkflowDecStruct {
    ErrorStruct<bool> errorStruct;  // information about the success of the decoding
    // only contains value if no error occured
    std::optional<FileDataStruct> file_data;  // file data struct that contains the decrypted content
    std::optional<DataHeader> dh;             // data header of the file
};

enum WorkflowState {
    INIT = 1,     // the API is initialized
    FILE_GOTTEN,  // the file was gotten
};

class API {
    /*
    API class between the front-end and the back-end
    encapsulates the backend implementations
    */
   private:
    Workflow current_workflow;    // the current workflow of the API (see documentation for more info)
    WorkflowState current_state;  // the current state of the API (makes sure that the API is used correctly)
    // the file mode that should be used for the file is stored in the FileDataStruct
    FileDataStruct file_data_struct;  // the user can construct a file data object from this struct
    Bytes correct_password_hash;      // the correct password hash for the dataheader
    // setting standard data header
    DataHeader dh = DataHeader(HModes(STANDARD_HASHMODE));  // the data header for the correct password
    // stores the file content that was encrypted by the algorithm
    // only this file content is valid to write to a file
    Bytes encrypted_data;
    // stores the path to the file from that the data was read
    // or an different file if the user wants to write to a different file (setFile())
    std::filesystem::path valid_file;

   public:
    // constructs the API in a given worflow mode and initializes the private variables
    // also takes the file mode that should be worked with
    API(Workflow workflow, const FModes file_mode);

    //*************** WORKFLOW 1 *****************
    // decoder: gets the data header and the file data struct from the file
    ErrorStruct<WorkflowDecStruct> runWorflow1dec(const std::filesystem::path file_path, const std::string password) noexcept;
    // you can work with the file data object gotten from the file data struct gotten from the WorkflowDecStruct
    // call createDataHeader in between to change the encryption settings
    // encoder: writes the file data (gotten from the object.getFileData()) and data header to the file
    // this can fail due to a non valid file. You have to call setFile() manually before calling this function again
    ErrorStruct<bool> runWorflow1enc(const std::filesystem::path file_path, const FileDataStruct file_data) noexcept;

    //*************** WORKFLOW 2 *****************
    // create a new file with createEncFile()
    // create a new dataheader first with createDataHeader()
    // call getNewFileData() to get an empty file struct
    // you can create the corresponding file data object from the struct
    // you can work with the file data object
    // encoder: writes the file data (gotten from the object.getFileData()) and data header to the file
    // this fails if file_path is not the same as in createEncFile(). You have to call setFile() manually before calling this function again
    ErrorStruct<bool> runWorflow2enc(const std::filesystem::path file_path, const FileDataStruct file_data) noexcept;

    //*************** WORKFLOW 3 *****************
    // get the file location of the file to delete
    // delete the file with deleteEncFile()

    // gets the path of the current working directory
    static std::filesystem::path getCurrentDirPath() noexcept;

    // gets the path of the directory where the .enc files are stored at default
    static ErrorStruct<std::filesystem::path> getEncDirPath() noexcept;

    // gets the names of all .enc files in the given directory
    static ErrorStruct<std::vector<std::string>> getAllEncFileNames(std::filesystem::path dir) noexcept;

    // gets the names of all .enc files in the given directory which are storing the wished file data (file mode) or are empty
    ErrorStruct<std::vector<std::string>> getRelevantFileNames(std::filesystem::path dir) noexcept;

    // creates a new .enc file at the given path (path contains the name of the file)
    ErrorStruct<bool> createEncFile(std::filesystem::path file_path) noexcept;

    // deletes the given .enc file
    ErrorStruct<bool> deleteEncFile(std::filesystem::path file_path) const noexcept;

    // gets the content of a given file path (e.g. enc_dir_path/file_name or enc_dir_path/file_name.enc)
    // fails if the file contains data that is not encrypted by the algorithm or does not belong to the templated file data type
    ErrorStruct<Bytes> getFileContent(const std::filesystem::path file_path) noexcept;

    // extract the data header from the file content
    static ErrorStruct<DataHeader> getDataHeader(const Bytes file_content) noexcept;

    // extract the content of the file without the data header
    static ErrorStruct<Bytes> getData(const Bytes file_content) noexcept;

    // checks if a password (given from the user to decrypt) is valid for this file and returns its hash.
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
    ErrorStruct<Bytes> verifyPassword(const std::string password, const DataHeader dh, const u_int64_t timeout = 0) noexcept;

    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations or the time (the chainhash runs until the time is reached to get the iterations)
    ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout = 0) noexcept;
    ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept;

    // decrypts the data (requires successful verifyPassword or createDataHeader run)
    // returns the decrypted content (without the data header)
    // uses the password and data header that were passed to verifyPassword
    ErrorStruct<Bytes> getDecryptedData(const Bytes enc_data) const noexcept;
    ErrorStruct<Bytes> getDecryptedData(const FileDataStruct file_data) const noexcept;

    // gets the file data struct (requires successful verifyPassword or createDataHeader run)
    // returns the FileDataStruct with the decrypted content
    ErrorStruct<FileDataStruct> getFileData(const Bytes dec_data) const noexcept;
    // gets a new file data object with no content
    ErrorStruct<FileDataStruct> getNewFileData() const noexcept;

    // encrypts the data (requires successful verifyPassword or createDataHeader run) returns the encrypted data
    // uses the password and data header that were passed to verifyPassword
    ErrorStruct<Bytes> getEncryptedData(const Bytes dec_data) noexcept;

    // checks if the file dataheader matches the dataheader of the API object (requires successful verifyPassword or createDataHeader run)
    // adds that file to valid files if it matches, empty files are also valid
    // as well as files with a older dataheader (the dataheader was changed)
    ErrorStruct<bool> checkFile(const std::filesystem::path file_path) noexcept;

    // sets the valid file (note that this does not check if the file is valid)
    // the user should be asked before setting a valid file
    ErrorStruct<bool> setFile(const std::filesystem::path file_path) noexcept;

    // writes encrypted data to a file adds the dataheader (requires successful verifyPassword or createDataHeader run)
    ErrorStruct<bool> writeToFile(const std::filesystem::path file_path, const Bytes enc_data) const noexcept;

    // deletes saved data (password hash, data header, encrypted data)
    // after this call, the API object is in the same state as after the constructor call
    void logout(Workflow new_workflow) noexcept;
};