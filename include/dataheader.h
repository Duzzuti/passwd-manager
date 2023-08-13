#pragma once

#include <optional>

#include "bytes.h"
#include "chainhash_data.h"
#include "chainhash_modes.h"
#include "hash_modes.h"
#include "settings.h"

struct DataHeaderParts {
    // holds the variables used for the dataheader
   private:
    std::optional<FModes> file_mode;                        // the file data mode that is choosen (content of the file)
    std::optional<HModes> hash_mode;                        // the hash mode that is choosen (hash function)
   public:
    ChainHash chainhash1;                    // chainhash data for the first chainhash (password -> passwordhash)
    ChainHash chainhash2;                    // chainhash data for the second chainhash (passwordhash -> validate password)
    unsigned char chainhash1_datablock_len;  // the length of the first datablock
    unsigned char chainhash2_datablock_len;  // the length of the second datablock
    Bytes valid_passwordhash;                // saves the hash that should be the result of the second chainhash
    Bytes enc_salt;                          // saves the encoded salt

    bool isFileDataModeSet() const noexcept{
        // checks if the file data mode is set
        return file_mode.has_value();
    }
    bool isHashModeSet() const noexcept{
        // checks if the hash mode is set
        return hash_mode.has_value();
    }
    FModes getFileDataMode() const{
        // gets the file data mode
        if (file_mode.has_value()) {
            return file_mode.value();
        } else {
            PLOG_ERROR << "file data mode is not set";
            throw std::runtime_error("file data mode is not set");
        }
    }
    HModes getHashMode() const{
        // gets the hash mode
        if (hash_mode.has_value()) {
            return hash_mode.value();
        } else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    void setFileDataMode(const FModes file_mode){
        // sets the file data mode
        if(0 < hash_mode && file_mode <= MAX_FILEMODE_NUMBER)
            this->file_mode = file_mode;
        else{
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::logic_error("file mode is not valid");
        }
    }
    void setHashMode(const HModes hash_mode){
        // sets the hash mode
        if(0 < hash_mode && hash_mode <= MAX_HASHMODE_NUMBER)
            this->hash_mode = hash_mode;
        else{
            PLOG_ERROR << "the given hash mode is not valid: " << +hash_mode;
            throw std::logic_error("hash mode is not valid");
        }
        this->hash_mode = hash_mode;
    }

};

struct DataHeaderSettingsIters {
    // holds the settings used for the dataheader
    // the values that should not be choosen randomly when generating a new header
   private:
    std::optional<FModes> file_mode;                        // the file data mode that is choosen (content of the file)
    std::optional<HModes> hash_mode;                        // the hash mode that is choosen (hash function)
    std::optional<CHModes> chainhash1_mode;                 // chainhash mode for the first chainhash (password -> passwordhash)
    std::optional<CHModes> chainhash2_mode;                 // chainhash mode for the second chainhash (passwordhash -> validate password)
   public:
    u_int64_t chainhash1_iters;                                 // iterations for the first chainhash
    u_int64_t chainhash2_iters;                                 // iterations for the second chainhash

    bool isFileDataModeSet() const noexcept{
        // checks if the file data mode is set
        return file_mode.has_value();
    }
    bool isHashModeSet() const noexcept{
        // checks if the hash mode is set
        return hash_mode.has_value();
    }
    bool isChainHash1ModeSet() const noexcept{
        // checks if the chainhash mode for the first chainhash is set
        return chainhash1_mode.has_value();
    }
    bool isChainHash2ModeSet() const noexcept{
        // checks if the chainhash mode for the second chainhash is set
        return chainhash2_mode.has_value();
    }
    FModes getFileDataMode() const{
        // gets the file data mode
        if (file_mode.has_value()) {
            return file_mode.value();
        } else {
            PLOG_ERROR << "file data mode is not set";
            throw std::runtime_error("file data mode is not set");
        }
    }
    HModes getHashMode() const{
        // gets the hash mode
        if (hash_mode.has_value()) {
            return hash_mode.value();
        } else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    CHModes getChainHash1Mode() const{
        // gets the chainhash mode for the first chainhash
        if (chainhash1_mode.has_value()) {
            return chainhash1_mode.value();
        } else {
            PLOG_ERROR << "chainhash mode for the first chainhash is not set";
            throw std::runtime_error("chainhash mode for the first chainhash is not set");
        }
    }
    CHModes getChainHash2Mode() const{
        // gets the chainhash mode for the second chainhash
        if (chainhash2_mode.has_value()) {
            return chainhash2_mode.value();
        } else {
            PLOG_ERROR << "chainhash mode for the second chainhash is not set";
            throw std::runtime_error("chainhash mode for the second chainhash is not set");
        }
    }
    void setFileDataMode(const FModes file_mode){
        // sets the file data mode
        if(0 < hash_mode && file_mode <= MAX_FILEMODE_NUMBER)
            this->file_mode = file_mode;
        else{
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::logic_error("file mode is not valid");
        }
    }
    void setHashMode(const HModes hash_mode){
        // sets the hash mode
        if(0 < hash_mode && hash_mode <= MAX_HASHMODE_NUMBER)
            this->hash_mode = hash_mode;
        else{
            PLOG_ERROR << "the given hash mode is not valid: " << +hash_mode;
            throw std::logic_error("hash mode is not valid");
        }
        this->hash_mode = hash_mode;
    }
    void setChainHash1Mode(const CHModes chainhash1_mode){
        // sets the chainhash mode for the first chainhash
        if(0 < chainhash1_mode && chainhash1_mode <= MAX_CHAINHASHMODE_NUMBER)
            this->chainhash1_mode = chainhash1_mode;
        else{
            PLOG_ERROR << "the given chainhash mode for the first chainhash is not valid: " << +chainhash1_mode;
            throw std::logic_error("chainhash mode for the first chainhash is not valid");
        }
    }
    void setChainHash2Mode(const CHModes chainhash2_mode){
        // sets the chainhash mode for the second chainhash
        if(0 < chainhash2_mode && chainhash2_mode <= MAX_CHAINHASHMODE_NUMBER)
            this->chainhash2_mode = chainhash2_mode;
        else{
            PLOG_ERROR << "the given chainhash mode for the second chainhash is not valid: " << +chainhash2_mode;
            throw std::logic_error("chainhash mode for the second chainhash is not valid");
        }
    }
};

struct DataHeaderSettingsTime {
    // dataheader iterations are calculated by a time limit
    // holds the settings used for the dataheader
    // the values that should not be choosen randomly when generating a new header
   private:
    std::optional<FModes> file_mode;                        // the file data mode that is choosen (content of the file)
    std::optional<HModes> hash_mode;                        // the hash mode that is choosen (hash function)
    std::optional<CHModes> chainhash1_mode;                 // chainhash mode for the first chainhash (password -> passwordhash)
    std::optional<CHModes> chainhash2_mode;                 // chainhash mode for the second chainhash (passwordhash -> validate password)
   public:
    u_int64_t chainhash1_time;                                  // max miliseconds for the first chainhash
    u_int64_t chainhash2_time;                                  // max miliseconds for the second chainhash

    bool isFileDataModeSet() const noexcept{
        // checks if the file data mode is set
        return file_mode.has_value();
    }
    bool isHashModeSet() const noexcept{
        // checks if the hash mode is set
        return hash_mode.has_value();
    }
    bool isChainHash1ModeSet() const noexcept{
        // checks if the chainhash mode for the first chainhash is set
        return chainhash1_mode.has_value();
    }
    bool isChainHash2ModeSet() const noexcept{
        // checks if the chainhash mode for the second chainhash is set
        return chainhash2_mode.has_value();
    }
    FModes getFileDataMode() const{
        // gets the file data mode
        if (file_mode.has_value()) {
            return file_mode.value();
        } else {
            PLOG_ERROR << "file data mode is not set";
            throw std::runtime_error("file data mode is not set");
        }
    }
    HModes getHashMode() const{
        // gets the hash mode
        if (hash_mode.has_value()) {
            return hash_mode.value();
        } else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    CHModes getChainHash1Mode() const{
        // gets the chainhash mode for the first chainhash
        if (chainhash1_mode.has_value()) {
            return chainhash1_mode.value();
        } else {
            PLOG_ERROR << "chainhash mode for the first chainhash is not set";
            throw std::runtime_error("chainhash mode for the first chainhash is not set");
        }
    }
    CHModes getChainHash2Mode() const{
        // gets the chainhash mode for the second chainhash
        if (chainhash2_mode.has_value()) {
            return chainhash2_mode.value();
        } else {
            PLOG_ERROR << "chainhash mode for the second chainhash is not set";
            throw std::runtime_error("chainhash mode for the second chainhash is not set");
        }
    }
    void setFileDataMode(const FModes file_mode){
        // sets the file data mode
        if(0 < hash_mode && file_mode <= MAX_FILEMODE_NUMBER)
            this->file_mode = file_mode;
        else{
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::logic_error("file mode is not valid");
        }
    }
    void setHashMode(const HModes hash_mode){
        // sets the hash mode
        if(0 < hash_mode && hash_mode <= MAX_HASHMODE_NUMBER)
            this->hash_mode = hash_mode;
        else{
            PLOG_ERROR << "the given hash mode is not valid: " << +hash_mode;
            throw std::logic_error("hash mode is not valid");
        }
        this->hash_mode = hash_mode;
    }
    void setChainHash1Mode(const CHModes chainhash1_mode){
        // sets the chainhash mode for the first chainhash
        if(0 < chainhash1_mode && chainhash1_mode <= MAX_CHAINHASHMODE_NUMBER)
            this->chainhash1_mode = chainhash1_mode;
        else{
            PLOG_ERROR << "the given chainhash mode for the first chainhash is not valid: " << +chainhash1_mode;
            throw std::logic_error("chainhash mode for the first chainhash is not valid");
        }
    }
    void setChainHash2Mode(const CHModes chainhash2_mode){
         // sets the chainhash mode for the second chainhash
        if(0 < chainhash2_mode && chainhash2_mode <= MAX_CHAINHASHMODE_NUMBER)
            this->chainhash2_mode = chainhash2_mode;
        else{
            PLOG_ERROR << "the given chainhash mode for the second chainhash is not valid: " << +chainhash2_mode;
            throw std::logic_error("chainhash mode for the second chainhash is not valid");
        }
    }
};

class DataHeader {
    /*
    this class stores the functionalities of the dataheader
    the header is some byte information about the file that is written at the beginning of the file
    the program can read this header and knows everything about the encryption that is used
    this DataHeader class is handling this information
    more information about the header: docs/dataheader.md
    */
   private:
    DataHeaderParts dh;       // saves every part of the header
    unsigned char hash_size;  // the size of the hash provided by the hash function (in Bytes)
    Bytes header_bytes;       // bytes that are in the header

   private:
    // checks if all data is set correctly
    bool isComplete() const noexcept;
    void setEncSalt(const Bytes salt);  // sets the salt

   public:
    // sets up the data header with a hash mode which is necessary to
    // calculate the length of the header, get the hash function and it is also const and cannot be changed anyways
    DataHeader(const HModes hash_mode);
    // setter for the data that is needed for the header
    void setFileDataMode(const FModes file_mode);  // sets the file data mode (semantic of content)
    // setter for the chainhashes, takes a mode, number of iters, a datablock length (to verify datablock) and the datablock
    // which contains data for the chainhash
    void setChainHash1(const ChainHash chainhash, const unsigned char len);
    void setChainHash2(const ChainHash chainhash, const unsigned char len);
    void setValidPasswordHashBytes(const Bytes validBytes);  // sets the passwordhashhash to validate the password hash
    // calculates the header bytes with all information that is set, throws if not enough information is set (or not valid)
    // verifies the pwhash with the previous set pwhash validator
    void calcHeaderBytes(const Bytes passwordhash, const bool verify_pwhash = true);
    Bytes getHeaderBytes() const;  // gets the current set header bytes, calcHeaderBytes overwrites this variable

    // creates a new DataHeader object with the given header bytes
    // after this call the fileBytes are the data that is not part of the header
    static ErrorStruct<DataHeader> setHeaderBytes(Bytes& fileBytes) noexcept;
    // creates a new DataHeader object with the given data header parts
    static ErrorStruct<DataHeader> setHeaderParts(const DataHeaderParts dhp) noexcept;

    // gets the length of the currently set header bytes, if its not currently set, we try to calculate the expected len
    // all set data, such as chainhash data etc. are used to calculate this expected len. If the data is not enough we return 0
    unsigned int getHeaderLength() const noexcept;
    // gets the hash size of the hash function that is used
    int getHashSize() const noexcept;
    // gets the dataheader parts if they are complete
    DataHeaderParts getDataHeaderParts() const;
    // WORK
    DataHeaderSettingsIters getSettings() const;  // gets the settings that are used for the dataheader
};
