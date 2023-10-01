#pragma once

#include <fstream>
#include <optional>
#include <vector>

#include "bytes.h"
#include "chainhash_modes.h"
#include "file_modes.h"
#include "hash_modes.h"

struct DataBlock {
   private:
    Bytes data = Bytes(0);  // the decrypted data
   public:
    DatablockType type = DatablockType::DEFAULT;

    DataBlock() = default;

    DataBlock(const DatablockType type, const Bytes data) {
        // basic constructor
        this->type = type;
        this->setData(data);
    }

    void setData(const Bytes& data) {
        // sets the data
        if (data.getLen() > 0 && data.getLen() <= 255)  // the size has to be one byte
            this->data = data;
        else {
            PLOG_ERROR << "the given data has an invalid length: " << data.getLen();
            throw std::invalid_argument("data has an invalid length");
        }
    }
    Bytes getData() const noexcept {
        // gets the data
        return this->data;
    }
};

struct EncDataBlock {
   private:
    DataBlock data_block;  // the enc data
    unsigned char enc_len;
    EncDataBlock() = default;

   public:
    static EncDataBlock createEncBlock(const unsigned char enc_type, const Bytes enc_data, const unsigned char enc_len) {
        // creates an EncDataBlock from the given data
        if(enc_data.getLen() != 255){
            PLOG_ERROR << "the given data has an invalid length: " << enc_data.getLen();
            throw std::invalid_argument("data has an invalid length");
        }
        EncDataBlock enc_block;
        enc_block.enc_len = enc_len;
        enc_block.data_block.type = DatablockType(enc_type);
        enc_block.data_block.setData(enc_data);
        return enc_block;
    }

    EncDataBlock(DataBlock datablock, const std::unique_ptr<Hash>&& hash, Bytes pwhash, Bytes enc_salt) {
        // basic constructor
        pwhash = hash->hash(pwhash);
        this->data_block.type = DatablockType((unsigned char)(datablock.type + pwhash.copySubBytes(0, 1).toLong()));
        this->enc_len = (unsigned char)(datablock.getData().getLen() + pwhash.copySubBytes(1, 2).toLong());
        u_int16_t written = 0;
        u_int16_t end;
        Bytes enc_data(255);
        while (enc_data.getLen() != datablock.getData().getLen()) {
            // encrypt the data
            enc_salt = hash->hash(enc_salt - pwhash);
            end = std::min<int>(written + enc_salt.getLen(), datablock.getData().getLen());
            (datablock.getData().copySubBytes(written, end) + enc_salt.copySubBytes(0, end - written)).addcopyToBytes(enc_data);
            written = end;
        }
        enc_data.addSize(255 - enc_data.getLen());  // fill the rest with random data
        enc_data.fillrandom();
        datablock.setData(enc_data);
        this->data_block = datablock;
    }

    Bytes getEnc() const noexcept {
        // gets the data
        return this->data_block.getData();
    }

    Bytes getDec(const std::unique_ptr<Hash>&& hash, Bytes pwhash, Bytes enc_salt) const noexcept {
        // decrypts the data
        pwhash = hash->hash(pwhash);
        unsigned char len = (unsigned char)(this->enc_len - pwhash.copySubBytes(1, 2).toLong());
        u_int16_t written = 0;
        u_int16_t end;
        Bytes dec_data(len);
        while (dec_data.getLen() < len) {
            // decrypt the data
            enc_salt = hash->hash(enc_salt - pwhash);
            end = std::min<int>(written + enc_salt.getLen(), len);
            (this->data_block.getData().copySubBytes(written, end) - enc_salt.copySubBytes(0, end - written)).addcopyToBytes(dec_data);
            written = end;
        }
        return dec_data;
    }

    DatablockType getEncType() const noexcept {
        // gets the type
        return this->data_block.type;
    }

    DatablockType getDecType(const std::unique_ptr<Hash>&& hash, Bytes pwhash) const noexcept {
        // gets the type
        pwhash = hash->hash(pwhash);
        return DatablockType((unsigned char)(this->data_block.type - pwhash.copySubBytes(0, 1).toLong()));
    }

    unsigned char getEncLen() const noexcept {
        // gets the length
        return this->enc_len;
    }
};

struct DataHeaderParts {
    // holds the variables used for the dataheader
   private:
    int hash_size;                            // the size of the hash provided by the hash function (in Bytes)
    std::optional<FModes> file_mode;          // the file data mode that is choosen (content of the file)
    std::optional<HModes> hash_mode;          // the hash mode that is choosen (hash function)
    std::optional<Bytes> valid_passwordhash;  // saves the hash that should be the result of the second chainhash
    std::optional<Bytes> enc_salt;            // saves the encoded salt
   public:
    std::vector<DataBlock> dec_data_blocks;     // the decrypted data blocks
    std::vector<EncDataBlock> enc_data_blocks;  // the encrypted data blocks
    ChainHash chainhash1;                       // chainhash data for the first chainhash (password -> passwordhash)
    ChainHash chainhash2;                       // chainhash data for the second chainhash (passwordhash -> validate password)

    bool isFileDataModeSet() const noexcept {
        // checks if the file data mode is set
        return this->file_mode.has_value();
    }
    bool isHashModeSet() const noexcept {
        // checks if the hash mode is set
        return this->hash_mode.has_value();
    }
    bool isValidPasswordHashSet() const noexcept {
        // checks if the valid password hash is set
        return this->valid_passwordhash.has_value();
    }
    bool isEncSaltSet() const noexcept {
        // checks if the encoded salt is set
        return this->enc_salt.has_value();
    }
    FModes getFileDataMode() const {
        // gets the file data mode
        if (this->file_mode.has_value()) {
            return this->file_mode.value();
        } else {
            PLOG_ERROR << "file data mode is not set";
            throw std::runtime_error("file data mode is not set");
        }
    }
    HModes getHashMode() const {
        // gets the hash mode
        if (this->hash_mode.has_value()) {
            return this->hash_mode.value();
        } else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    Bytes getValidPasswordHash() const {
        // gets the valid password hash
        if (this->valid_passwordhash.has_value()) {
            return this->valid_passwordhash.value();
        } else {
            PLOG_ERROR << "valid password hash is not set";
            throw std::runtime_error("valid password hash is not set");
        }
    }
    Bytes getEncSalt() const {
        // gets the encoded salt
        if (this->enc_salt.has_value()) {
            return this->enc_salt.value();
        } else {
            PLOG_ERROR << "encoded salt is not set";
            throw std::runtime_error("encoded salt is not set");
        }
    }
    int getHashSize() const {
        // gets the hash size
        if (this->hash_mode.has_value()) {
            return this->hash_size;
        } else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    void setFileDataMode(const FModes file_mode) {
        // sets the file data mode
        if (FileModes::isModeValid(file_mode))
            this->file_mode = file_mode;
        else {
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::invalid_argument("file mode is not valid");
        }
    }
    void setHashMode(const HModes hash_mode) {
        // sets the hash mode
        if (HashModes::isModeValid(hash_mode)) {
            this->hash_mode = hash_mode;
            this->hash_size = HashModes::getHash(hash_mode)->getHashSize();
        } else {
            PLOG_ERROR << "the given hash mode is not valid: " << +hash_mode;
            throw std::invalid_argument("hash mode is not valid");
        }
    }
    void setValidPasswordHash(const Bytes valid_passwordhash) {
        // sets the valid password hash
        if (this->getHashSize() == valid_passwordhash.getLen())
            this->valid_passwordhash = valid_passwordhash;
        else {
            PLOG_ERROR << "the given valid password hash has the wrong length: " << valid_passwordhash.getLen() << " != " << this->getHashSize();
            throw std::invalid_argument("valid password hash has the wrong length");
        }
    }
    void setEncSalt(const Bytes enc_salt) {
        // sets the encoded salt
        if (this->getHashSize() == enc_salt.getLen())
            this->enc_salt = enc_salt;
        else {
            PLOG_ERROR << "the given encoded salt has the wrong length: " << enc_salt.getLen() << " != " << this->getHashSize();
            throw std::invalid_argument("encoded salt has the wrong length");
        }
    }

    bool isComplete(const unsigned char hash_size) const noexcept {
        // checks if everything is set correctly
        try {
            if (this->chainhash1.valid() && this->chainhash2.valid() && this->isValidPasswordHashSet() && this->isFileDataModeSet() && this->isHashModeSet() && this->getHashSize() == hash_size &&
                this->isEncSaltSet())
                return true;  // everything is set correctly
        } catch (std::exception& e) {
            PLOG_WARNING << "isComplete thrown: " << e.what();
        }
        // something is not set correctly, log a debug message and return false
        PLOG_DEBUG << "isComplete failed: \n" << *this;
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const DataHeaderParts& dhp) {
        // prints the dataheader parts
        return os << "[DataHeaderParts] "
                  << "file_mode: " << (dhp.isFileDataModeSet() ? std::to_string(+dhp.getFileDataMode()) : "not set") << ", "
                  << "hash_mode: " << (dhp.isHashModeSet() ? std::to_string(+dhp.getHashMode()) : "not set") << ", "
                  << "valid_hash: " << (dhp.isValidPasswordHashSet() ? dhp.getValidPasswordHash().toHex() : "not set") << ", "
                  << "enc_salt: " << (dhp.isEncSaltSet() ? dhp.getEncSalt().toHex() : "not set") << ", "
                  << "chainhash1: " << dhp.chainhash1 << ", "
                  << "chainhash2: " << dhp.chainhash2;
    }
};

struct DataHeaderSettingsIters {
    // holds the settings used for the dataheader
    // the values that should not be choosen randomly when generating a new header
   private:
    std::optional<FModes> file_mode;            // the file data mode that is choosen (content of the file)
    std::optional<HModes> hash_mode;            // the hash mode that is choosen (hash function)
    std::optional<CHModes> chainhash1_mode;     // chainhash mode for the first chainhash (password -> passwordhash)
    std::optional<CHModes> chainhash2_mode;     // chainhash mode for the second chainhash (passwordhash -> validate password)
    std::optional<u_int64_t> chainhash1_iters;  // iterations for the first chainhash
    std::optional<u_int64_t> chainhash2_iters;  // iterations for the second chainhash
   public:
    std::vector<DataBlock> dec_data_blocks;     // the decrypted data blocks
    std::vector<EncDataBlock> enc_data_blocks;  // the encrypted data blocks

   public:
    bool isFileDataModeSet() const noexcept {
        // checks if the file data mode is set
        return this->file_mode.has_value();
    }
    bool isHashModeSet() const noexcept {
        // checks if the hash mode is set
        return this->hash_mode.has_value();
    }
    bool isChainHash1ModeSet() const noexcept {
        // checks if the chainhash mode for the first chainhash is set
        return this->chainhash1_mode.has_value();
    }
    bool isChainHash2ModeSet() const noexcept {
        // checks if the chainhash mode for the second chainhash is set
        return this->chainhash2_mode.has_value();
    }
    bool isChainHash1ItersSet() const noexcept {
        // checks if the iterations for the first chainhash are set
        return this->chainhash1_iters.has_value();
    }
    bool isChainHash2ItersSet() const noexcept {
        // checks if the iterations for the second chainhash are set
        return this->chainhash2_iters.has_value();
    }
    FModes getFileDataMode() const {
        // gets the file data mode
        if (this->file_mode.has_value())
            return this->file_mode.value();
        else {
            PLOG_ERROR << "file data mode is not set";
            throw std::runtime_error("file data mode is not set");
        }
    }
    HModes getHashMode() const {
        // gets the hash mode
        if (this->hash_mode.has_value())
            return this->hash_mode.value();
        else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    CHModes getChainHash1Mode() const {
        // gets the chainhash mode for the first chainhash
        if (this->chainhash1_mode.has_value())
            return this->chainhash1_mode.value();
        else {
            PLOG_ERROR << "chainhash mode for the first chainhash is not set";
            throw std::runtime_error("chainhash mode for the first chainhash is not set");
        }
    }
    CHModes getChainHash2Mode() const {
        // gets the chainhash mode for the second chainhash
        if (this->chainhash2_mode.has_value())
            return this->chainhash2_mode.value();
        else {
            PLOG_ERROR << "chainhash mode for the second chainhash is not set";
            throw std::runtime_error("chainhash mode for the second chainhash is not set");
        }
    }
    u_int64_t getChainHash1Iters() const {
        // gets the iterations for the first chainhash
        if (this->chainhash1_iters.has_value())
            return this->chainhash1_iters.value();
        else {
            PLOG_ERROR << "iterations for the first chainhash are not set";
            throw std::runtime_error("iterations for the first chainhash are not set");
        }
    }
    u_int64_t getChainHash2Iters() const {
        // gets the iterations for the second chainhash
        if (this->chainhash2_iters.has_value())
            return this->chainhash2_iters.value();
        else {
            PLOG_ERROR << "iterations for the second chainhash are not set";
            throw std::runtime_error("iterations for the second chainhash are not set");
        }
    }
    void setFileDataMode(const FModes file_mode) {
        // sets the file data mode
        if (FileModes::isModeValid(file_mode))
            this->file_mode = file_mode;
        else {
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::invalid_argument("file mode is not valid");
        }
    }
    void setHashMode(const HModes hash_mode) {
        // sets the hash mode
        if (HashModes::isModeValid(hash_mode))
            this->hash_mode = hash_mode;
        else {
            PLOG_ERROR << "the given hash mode is not valid: " << +hash_mode;
            throw std::invalid_argument("hash mode is not valid");
        }
    }
    void setChainHash1Mode(const CHModes chainhash1_mode) {
        // sets the chainhash mode for the first chainhash
        if (ChainHashModes::isModeValid(chainhash1_mode))
            this->chainhash1_mode = chainhash1_mode;
        else {
            PLOG_ERROR << "the given chainhash mode for the first chainhash is not valid: " << +chainhash1_mode;
            throw std::invalid_argument("chainhash mode for the first chainhash is not valid");
        }
    }
    void setChainHash2Mode(const CHModes chainhash2_mode) {
        // sets the chainhash mode for the second chainhash
        if (ChainHashModes::isModeValid(chainhash2_mode))
            this->chainhash2_mode = chainhash2_mode;
        else {
            PLOG_ERROR << "the given chainhash mode for the second chainhash is not valid: " << +chainhash2_mode;
            throw std::invalid_argument("chainhash mode for the second chainhash is not valid");
        }
    }
    void setChainHash1Iters(const u_int64_t chainhash1_iters) {
        // sets the iterations for the first chainhash
        if (chainhash1_iters > 0 && chainhash1_iters <= MAX_ITERATIONS)
            this->chainhash1_iters = chainhash1_iters;
        else {
            PLOG_ERROR << "the given iterations for the first chainhash are not valid: " << chainhash1_iters;
            throw std::invalid_argument("iterations for the first chainhash are not valid");
        }
    }
    void setChainHash2Iters(const u_int64_t chainhash2_iters) {
        // sets the iterations for the second chainhash
        if (chainhash2_iters > 0 && chainhash2_iters <= MAX_ITERATIONS)
            this->chainhash2_iters = chainhash2_iters;
        else {
            PLOG_ERROR << "the given iterations for the second chainhash are not valid: " << chainhash2_iters;
            throw std::invalid_argument("iterations for the second chainhash are not valid");
        }
    }

    bool isComplete() const noexcept {
        // checks if everything is set correctly
        try {
            if (this->isFileDataModeSet() && this->isHashModeSet() && this->isChainHash1ModeSet() && this->isChainHash1ItersSet() && this->isChainHash2ModeSet() && this->isChainHash2ItersSet())
                return true;  // everything is set correctly
        } catch (std::exception& e) {
            PLOG_WARNING << "isComplete thrown: " << e.what();
        }
        // something is not set correctly, log a debug message and return false
        PLOG_DEBUG << "isComplete failed: \n" << *this;
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const DataHeaderSettingsIters& ds) {
        // prints the dataheader settings
        return os << "[DataHeaderSettingsIters] "
                  << "file_mode: " << (ds.isFileDataModeSet() ? std::to_string(+ds.getFileDataMode()) : "not set") << ", "
                  << "hash_mode: " << (ds.isHashModeSet() ? std::to_string(+ds.getHashMode()) : "not set") << ", "
                  << "ch1_mode: " << (ds.isChainHash1ModeSet() ? std::to_string(+ds.getChainHash1Mode()) : "not set") << ", "
                  << "ch1_iters: " << (ds.isChainHash1ItersSet() ? std::to_string(ds.getChainHash1Iters()) : "not set") << ", "
                  << "ch2_mode: " << (ds.isChainHash2ModeSet() ? std::to_string(+ds.getChainHash2Mode()) : "not set") << ", "
                  << "ch2_iters: " << (ds.isChainHash2ItersSet() ? std::to_string(ds.getChainHash2Iters()) : "not set");
    }
};

struct DataHeaderSettingsTime {
    // dataheader iterations are calculated by a time limit
    // holds the settings used for the dataheader
    // the values that should not be choosen randomly when generating a new header
   private:
    std::optional<FModes> file_mode;           // the file data mode that is choosen (content of the file)
    std::optional<HModes> hash_mode;           // the hash mode that is choosen (hash function)
    std::optional<CHModes> chainhash1_mode;    // chainhash mode for the first chainhash (password -> passwordhash)
    std::optional<CHModes> chainhash2_mode;    // chainhash mode for the second chainhash (passwordhash -> validate password)
    std::optional<u_int64_t> chainhash1_time;  // max miliseconds for the first chainhash
    std::optional<u_int64_t> chainhash2_time;  // max miliseconds for the second chainhash
   public:
    std::vector<DataBlock> dec_data_blocks;     // the decrypted data blocks
    std::vector<EncDataBlock> enc_data_blocks;  // the encrypted data blocks

   public:
    bool isFileDataModeSet() const noexcept {
        // checks if the file data mode is set
        return this->file_mode.has_value();
    }
    bool isHashModeSet() const noexcept {
        // checks if the hash mode is set
        return this->hash_mode.has_value();
    }
    bool isChainHash1ModeSet() const noexcept {
        // checks if the chainhash mode for the first chainhash is set
        return this->chainhash1_mode.has_value();
    }
    bool isChainHash2ModeSet() const noexcept {
        // checks if the chainhash mode for the second chainhash is set
        return this->chainhash2_mode.has_value();
    }
    bool isChainHash1TimeSet() const noexcept {
        // checks if the time for the first chainhash is set
        return this->chainhash1_time.has_value();
    }
    bool isChainHash2TimeSet() const noexcept {
        // checks if the time for the second chainhash is set
        return this->chainhash2_time.has_value();
    }
    FModes getFileDataMode() const {
        // gets the file data mode
        if (this->file_mode.has_value())
            return this->file_mode.value();
        else {
            PLOG_ERROR << "file data mode is not set";
            throw std::runtime_error("file data mode is not set");
        }
    }
    HModes getHashMode() const {
        // gets the hash mode
        if (this->hash_mode.has_value())
            return this->hash_mode.value();
        else {
            PLOG_ERROR << "hash mode is not set";
            throw std::runtime_error("hash mode is not set");
        }
    }
    CHModes getChainHash1Mode() const {
        // gets the chainhash mode for the first chainhash
        if (this->chainhash1_mode.has_value())
            return this->chainhash1_mode.value();
        else {
            PLOG_ERROR << "chainhash mode for the first chainhash is not set";
            throw std::runtime_error("chainhash mode for the first chainhash is not set");
        }
    }
    CHModes getChainHash2Mode() const {
        // gets the chainhash mode for the second chainhash
        if (this->chainhash2_mode.has_value())
            return this->chainhash2_mode.value();
        else {
            PLOG_ERROR << "chainhash mode for the second chainhash is not set";
            throw std::runtime_error("chainhash mode for the second chainhash is not set");
        }
    }
    u_int64_t getChainHash1Time() const {
        // gets the time for the first chainhash
        if (this->chainhash1_time.has_value())
            return this->chainhash1_time.value();
        else {
            PLOG_ERROR << "run time for the first chainhash is not set";
            throw std::runtime_error("run time for the first chainhash is not set");
        }
    }
    u_int64_t getChainHash2Time() const {
        // gets the time for the second chainhash
        if (this->chainhash2_time.has_value())
            return this->chainhash2_time.value();
        else {
            PLOG_ERROR << "run time for the second chainhash is not set";
            throw std::runtime_error("run time for the second chainhash is not set");
        }
    }
    void setFileDataMode(const FModes file_mode) {
        // sets the file data mode
        if (FileModes::isModeValid(file_mode))
            this->file_mode = file_mode;
        else {
            PLOG_ERROR << "the given file data mode is not valid: " << +file_mode;
            throw std::invalid_argument("file mode is not valid");
        }
    }
    void setHashMode(const HModes hash_mode) {
        // sets the hash mode
        if (HashModes::isModeValid(hash_mode))
            this->hash_mode = hash_mode;
        else {
            PLOG_ERROR << "the given hash mode is not valid: " << +hash_mode;
            throw std::invalid_argument("hash mode is not valid");
        }
    }
    void setChainHash1Mode(const CHModes chainhash1_mode) {
        // sets the chainhash mode for the first chainhash
        if (ChainHashModes::isModeValid(chainhash1_mode))
            this->chainhash1_mode = chainhash1_mode;
        else {
            PLOG_ERROR << "the given chainhash mode for the first chainhash is not valid: " << +chainhash1_mode;
            throw std::invalid_argument("chainhash mode for the first chainhash is not valid");
        }
    }
    void setChainHash2Mode(const CHModes chainhash2_mode) {
        // sets the chainhash mode for the second chainhash
        if (ChainHashModes::isModeValid(chainhash2_mode))
            this->chainhash2_mode = chainhash2_mode;
        else {
            PLOG_ERROR << "the given chainhash mode for the second chainhash is not valid: " << +chainhash2_mode;
            throw std::invalid_argument("chainhash mode for the second chainhash is not valid");
        }
    }
    void setChainHash1Time(const u_int64_t chainhash1_time) {
        // sets the run time for the first chainhash
        if (chainhash1_time > 0 && chainhash1_time <= MAX_RUNTIME)
            this->chainhash1_time = chainhash1_time;
        else {
            PLOG_ERROR << "the given run time for the first chainhash is not valid: " << chainhash1_time;
            throw std::invalid_argument("run time for the first chainhash is not valid");
        }
    }
    void setChainHash2Time(const u_int64_t chainhash2_time) {
        // sets the run time for the second chainhash
        if (chainhash2_time > 0 && chainhash2_time <= MAX_RUNTIME)
            this->chainhash2_time = chainhash2_time;
        else {
            PLOG_ERROR << "the given run time for the second chainhash is not valid: " << chainhash2_time;
            throw std::invalid_argument("run time for the second chainhash is not valid");
        }
    }

    bool isComplete() const noexcept {
        // checks if everything is set correctly
        try {
            if (this->isFileDataModeSet() && this->isHashModeSet() && this->isChainHash1ModeSet() && this->isChainHash1TimeSet() && this->isChainHash2ModeSet() && this->isChainHash2TimeSet())
                return true;  // everything is set correctly
        } catch (std::exception& e) {
            PLOG_WARNING << "isComplete thrown: " << e.what();
        }
        // something is not set correctly, log a debug message and return false
        PLOG_DEBUG << "isComplete failed: \n" << *this;
        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const DataHeaderSettingsTime& ds) {
        // prints the dataheader settings
        return os << "[DataHeaderSettingsTime] "
                  << "file_mode: " << (ds.isFileDataModeSet() ? std::to_string(+ds.getFileDataMode()) : "not set") << ", "
                  << "hash_mode: " << (ds.isHashModeSet() ? std::to_string(+ds.getHashMode()) : "not set") << ", "
                  << "ch1_mode: " << (ds.isChainHash1ModeSet() ? std::to_string(+ds.getChainHash1Mode()) : "not set") << ", "
                  << "ch1_time: " << (ds.isChainHash1TimeSet() ? std::to_string(ds.getChainHash1Time()) : "not set") << ", "
                  << "ch2_mode: " << (ds.isChainHash2ModeSet() ? std::to_string(+ds.getChainHash2Mode()) : "not set") << ", "
                  << "ch2_time: " << (ds.isChainHash2TimeSet() ? std::to_string(ds.getChainHash2Time()) : "not set");
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
    DataHeaderParts dh;                  // saves every part of the header
    unsigned char hash_size;             // the size of the hash provided by the hash function (in Bytes)
    Bytes header_bytes = Bytes(0);       // bytes that are in the header
    std::optional<u_int64_t> file_size;  // the size of the file that is encrypted
    u_int32_t datablocks_len = 0;        // the length of the datablocks

   private:
    // checks if all data is set correctly
    bool isComplete() const noexcept;
    void setEncSalt(const Bytes& salt);  // sets the salt

   public:
    // sets up the data header with a hash mode which is necessary to
    // calculate the length of the header, get the hash function and it is also const and cannot be changed anyways
    DataHeader(const HModes hash_mode);
    // setter for the data that is needed for the header
    void setFileDataMode(const FModes file_mode);  // sets the file data mode (semantic of content)
    // setter for the chainhashes, takes a mode, number of iters, a datablock length (to verify datablock) and the datablock
    // which contains data for the chainhash
    void setChainHash1(const ChainHash chainhash);
    void setChainHash2(const ChainHash chainhash);
    void setValidPasswordHashBytes(const Bytes& validBytes);  // sets the passwordhashhash to validate the password hash
    void clearDataBlocks() noexcept;                          // clears the data blocks
    void addDataBlock(const DataBlock datablock);             // adds a data block
    void addEncDataBlock(const EncDataBlock encdatablock);    // adds an encrypted data block

    void setFileSize(const u_int64_t file_size);            // sets the file size
    void setDataSize(const u_int32_t data_size);            // sets the file size by adding the header size to the data size
    std::optional<u_int64_t> getFileSize() const noexcept;  // gets the file size
    // calculates the header bytes with all information that is set, throws if not enough information is set (or not valid)
    // verifies the pwhash with the previous set pwhash validator
    void calcHeaderBytes(const Bytes& passwordhash = Bytes(0));
    Bytes getHeaderBytes() const;  // gets the current set header bytes, calcHeaderBytes overwrites this variable

    // creates a new DataHeader object with the given header bytes
    // after this call the fileBytes are the data that is not part of the header
    static ErrorStruct<std::unique_ptr<DataHeader>> setHeaderBytes(Bytes& fileBytes) noexcept;
    static ErrorStruct<std::unique_ptr<DataHeader>> setHeaderBytes(std::ifstream& file) noexcept;
    // creates a new DataHeader object with the given data header parts
    static ErrorStruct<std::unique_ptr<DataHeader>> setHeaderParts(const DataHeaderParts& dhp) noexcept;

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
