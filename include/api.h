/*
This Api class is the interface between the front-end and the back-end.
It encapsulates the back-end implementations and provides a simple interface for the front-end.

for full documenation see the documentation of the API class in the documentation folder
*/
#pragma once 

#include <iostream>
#include <filesystem>

#include "error.h"
#include "file_data.h"
#include "dataheader.h"

// tells the API which workflow to use
enum Workflow{
    WORK_WITH_OLD_FILE = 1,     // WORKFLOW 1: WORK WITH EXISTING FILE
    WORK_WITH_NEW_FILE,         // WORKFLOW 2: WORK WITH NEW FILE
    DELETE_FILE                 // WORKFLOW 3: DELETE FILE
};

// struct that is returned by the API when the if you decode a file
struct WorkflowDecStruct{
    ErrorStruct<bool> errorStruct;  // information about the success of the decoding
    FileData* file_data;    // file data object that contains the decrypted content
    DataHeader dh;  // data header of the file
};

enum WorkflowState{
    //WORK
};

class API {
    /*
    API class between the front-end and the back-end
    encapsulates the backend implementations
    */
private:
    Workflow current_workflow;    // the current workflow of the API (see documentation for more info)
    WorkflowState current_state;  // the current state of the API (makes sure that the API is used correctly)
    Bytes correct_password_hash;  // the correct password hash of the file
    DataHeader dh;                // the data header for the correct password
    // stores all file contents that were encrypted by the algorithm
    // only those are valid if you write to the file
    std::vector<Bytes> encrypted_data;  
    // stores pahts to files that have the same header as one of the valid_headers
    // the user can also add files to this approved files vector
    std::vector<DataHeader> valid_headers;
    std::vector<std::filesystem::path> valid_files; 

public:
    // constructs the API in a given worflow mode and initializes the private variables
    API(Workflow workflow) noexcept;

    //*************** WORKFLOW 1 *****************
    // decoder: gets the file data and data header from the file
    WorkflowDecStruct runWorflow1dec(const std::filesystem::path file_path, const std::string password) noexcept;
    // call createDataHeader in between to change the encryption settings
    // encoder: writes the file data and data header to the file
    // this can fail due to a non valid file. You have to call addFile() manually before calling this function again
    ErrorStruct<bool> runWorflow1enc(const std::filesystem::path file_path, const FileData* file_data) noexcept;

    //*************** WORKFLOW 2 *****************
    // create a new file with createEncFile()
    // create a new dataheader first with createDataHeader()
    // call getNewFileData() to get the file data object
    // encoder: writes the file data and data header to the file
    // this fails if file_path is not the same as in createEncFile(). You have to call addFile() manually before calling this function again
    WorkflowDecStruct runWorflow2enc(const std::filesystem::path file_path, const FileData* file_data) noexcept;

    //*************** WORKFLOW 3 *****************
    // get the file location of the file to delete
    // delete the file with deleteEncFile()

    // gets the path of the current working directory
    static ErrorStruct<std::filesystem::path> getCurrentDirPath() noexcept;

    // gets the path of the directory where the .enc files are stored at default
    static ErrorStruct<std::filesystem::path> getEncDirPath() noexcept;
    
    // gets the names of all .enc files in the given directory
    static ErrorStruct<std::vector<std::string>> getEncFileNames(std::filesystem::path dir) noexcept;

    // creates a new .enc file at the given path (path contains the name of the file)
    ErrorStruct<bool> createEncFile(std::filesystem::path file_path) noexcept;

    // deletes the given .enc file
    ErrorStruct<bool> deleteEncFile(std::filesystem::path file_path) const noexcept;

    // gets the content of a given file path (e.g. enc_dir_path/file_name or enc_dir_path/file_name.enc)
    ErrorStruct<Bytes> getFileContent(const std::filesystem::path file_path) noexcept;
    
    // extract the data header from the file content
    static ErrorStruct<DataHeader> getDataHeader(const Bytes file_content) noexcept;

    // extract the content of the file without the data header
    static ErrorStruct<Bytes> getData(const Bytes file_content) noexcept;

    // checks if a password (given from the user to decrypt) is valid for this file and returns its hash. 
    // This call is expensive
    // because it has to hash the password twice. A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // NOTE that if the timeout is reached, the function will return with a TIMEOUT SuccessType, but the password could be valid
    ErrorStruct<Bytes> verifyPassword(const std::string password, const DataHeader dh, const u_int64_t timeout=0) noexcept;
    
    // creates a data header for a given password and settings by randomizing the salt and chainhash data
    // This call is expensive because it has to chainhash the password twice to generate a validator.
    // A timeout (in ms) can be specified to limit the time of the call (0 means no timeout)
    // you can specify the iterations or the time (the chainhash runs until the time is reached to get the iterations)
    ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsIters ds, const u_int64_t timeout=0) noexcept;
    ErrorStruct<DataHeader> createDataHeader(const std::string password, const DataHeaderSettingsTime ds) noexcept;

    // decrypts the data (requires successful verifyPassword or createDataHeader run) 
    // returns the decrypted content (without the data header)
    // uses the password and data header that were passed to verifyPassword
    ErrorStruct<Bytes> getDecryptedData(const Bytes enc_data) const noexcept;
    ErrorStruct<Bytes> getDecryptedData(const FileData* file_data) const noexcept;

    // gets the file data (requires successful verifyPassword or createDataHeader run)
    // returns the correct file data object with the decrypted content already gotten
    ErrorStruct<FileData*> getFileData(const Bytes dec_data) const noexcept;
    //gets a new file data object with the no content
    ErrorStruct<FileData*> getNewFileData() const noexcept;

    // encrypts the data (requires successful verifyPassword or createDataHeader run) returns the encrypted data
    // uses the password and data header that were passed to verifyPassword
    ErrorStruct<Bytes> getEncryptedData(const Bytes dec_data) noexcept;

    // checks if the file dataheader matches the dataheader of the API object (requires successful verifyPassword or createDataHeader run)
    // adds that file to valid files if it matches, empty files are also valid
    // as well as files with a older dataheader (the dataheader was changed)
    ErrorStruct<bool> checkFile(const std::filesystem::path file_path) noexcept; 

    // adds a file to the valid files vector (note that this does not check if the file is valid)
    // the user should be asked before adding a file to the valid files vector
    ErrorStruct<bool> addFile(const std::filesystem::path file_path) noexcept;

    //writes encrypted data to a file adds the dataheader (requires successful verifyPassword or createDataHeader run)
    ErrorStruct<bool> writeToFile(const std::filesystem::path file_path, const Bytes enc_data) const noexcept;
    
    // deletes saved data (password hash, data header, encrypted data)
    // after this call, the API object is in the same state as after the constructor call
    void logout(Workflow new_workflow) noexcept;
};