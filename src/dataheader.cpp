/*
this file contains the implementations of Data header class
*/
#include "dataheader.h"

#include "logger.h"
#include "rng.h"
#include "utility.h"

DataHeader::DataHeader(const HModes hash_mode) {
    PLOG_VERBOSE << "new DataHeader created (hash mode: " << +hash_mode << ")";
    // initialize the hash mode
    this->dh.setHashMode(hash_mode);
    // initialize the hash size
    this->hash_size = HashModes::getHash(hash_mode)->getHashSize();
}

bool DataHeader::isComplete() const noexcept {
    // checks if the dataheader has everything set
    if (this->dh.isComplete(this->hash_size)) return true;
    return false;
}

unsigned int DataHeader::getHeaderLength() const noexcept {
    // gets the header len, if there is no header set try to calculate the len, else return 0
    if (this->dh.chainhash1.valid() && this->dh.chainhash2.valid())                                                                             // all data set to calculate the header length
        return 38 + 2 * this->hash_size + this->dh.chainhash1.getChainHashData()->getLen() + this->dh.chainhash2.getChainHashData()->getLen();  // dataheader.md
    if (this->header_bytes.getLen() > 0) return this->header_bytes.getLen();                                                                    // header bytes are set, so we get this length
    return 0;                                                                                                                                   // not enough infos to get the header length
}

int DataHeader::getHashSize() const noexcept {
    // gets the hash size
    return this->hash_size;
}

void DataHeader::setChainHash1(const ChainHash chainhash) {
    // sets the information about the first chainhash
    PLOG_VERBOSE << "setting chainhash1: " << chainhash;
    if (!chainhash.valid()) {
        PLOG_ERROR << "got invalid chainhash1: " << chainhash;
        throw std::invalid_argument("got invalid chainhash1");
    }
    // set the information to the object
    this->dh.chainhash1 = chainhash;
    this->header_bytes.setLen(0); // clear header bytes because they have to be recalculated
}

void DataHeader::setChainHash2(const ChainHash chainhash) {
    // sets the information about the second chainhash
    PLOG_VERBOSE << "setting chainhash2: " << chainhash;
    if (!chainhash.valid()) {
        PLOG_ERROR << "got invalid chainhash2: " << chainhash;
        throw std::invalid_argument("got invalid chainhash2");
    }
    // set the information to the object
    this->dh.chainhash2 = chainhash;
    this->header_bytes.setLen(0); // clear header bytes because they have to be recalculated
}

void DataHeader::setFileDataMode(const FModes file_mode) {
    // sets the file data mode
    PLOG_VERBOSE << "setting file mode: " << +file_mode;
    this->dh.setFileDataMode(file_mode);
    this->header_bytes.setLen(0); // clear header bytes because they have to be recalculated
}

void DataHeader::setValidPasswordHashBytes(const Bytes& validBytes) {
    // set the passwordhash validator hash
    PLOG_VERBOSE << "setting password validator hash: " << validBytes.toHex();
    this->dh.setValidPasswordHash(validBytes);
    this->header_bytes.setLen(0); // clear header bytes because they have to be recalculated
}

void DataHeader::setFileSize(const u_int64_t file_size) {
    // sets the file size
    PLOG_VERBOSE << "setting file size: " << file_size;
    if (!this->isComplete()) {
        // header bytes cannot be calculated (data is missing)
        PLOG_ERROR << "all dataheader parts have to be set to set the file size";
        throw std::logic_error("all dataheader parts have to be set to set the file size");
    }
    if(file_size < this->getHeaderLength()){
        // file size is to small
        PLOG_ERROR << "file size is to small (file_size: " << file_size << ", header_length: " << this->getHeaderLength() << ")";
        throw std::invalid_argument("file size is to small");
    }
    this->header_bytes.setLen(0); // clear header bytes because they have to be recalculated
    this->file_size = file_size;
}

std::optional<u_int64_t> DataHeader::getFileSize() const noexcept { return this->file_size;}

void DataHeader::setEncSalt(const Bytes& salt) {
    // sets the salt
    PLOG_VERBOSE << "setting salt: " << salt.toHex();
    this->dh.setEncSalt(salt);
    this->header_bytes.setLen(0); // clear header bytes because they have to be recalculated
}

Bytes DataHeader::getHeaderBytes() const {
    // gets the header bytes
    if (this->getHeaderLength() == 0) {
        // no header is set and there are missing information
        PLOG_ERROR << "no header is set and there are even missing information to calculate the header bytes length";
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    if (this->getHeaderLength() != this->header_bytes.getLen()) {
        // current header has not the length that it should have (current header is out of date)
        PLOG_ERROR << "the calculated header length is not equal to the set header length. Call calcHeaderBytes() first. (set: " << +this->header_bytes.getLen()
                   << ", calculated: " << +this->getHeaderLength() << ")";
        throw std::logic_error("the calculated header length is not equal to the set header length. Call calcHeaderBytes() first");
    }
    return this->header_bytes;
}

void DataHeader::calcHeaderBytes(const Bytes& passwordhash) {
    // calculates the header
    PLOG_VERBOSE << "calculating header bytes " << (passwordhash.isEmpty() ? "without verifying" : "with verifying");
    if (!this->isComplete()) {
        // header bytes cannot be calculated (data is missing)
        PLOG_ERROR << "not all required data is set to calculate the header bytes";
        throw std::logic_error("not all required data is set to calculate the header bytes");
    }
    if(!this->file_size.has_value()){
        // file size is not set
        PLOG_ERROR << "file size is not set";
        throw std::logic_error("file size is not set");
    }
    // file size is set
    if(this->file_size.value() < this->getHeaderLength()){
        // file size is to small
        PLOG_ERROR << "file size is to small (file_size: " << this->file_size.value() << ", header_length: " << this->getHeaderLength() << ")";
        throw std::invalid_argument("file size is to small");
    }
    if (!passwordhash.isEmpty()) {
        // verifies the given pwhash with the currently set validator
        std::unique_ptr<Hash> hash = std::move(HashModes::getHash(this->dh.getHashMode()));  // gets the right hash function
        // is the chainhash from the given hash equal to the validator
        const bool isOkay = (this->dh.getValidPasswordHash() == ChainHashModes::performChainHash(this->dh.chainhash2, std::move(hash), passwordhash).returnValue());
        if (!isOkay) {
            // given pwhash is not valid
            PLOG_ERROR << "provided passwordhash is not valid (against validator)";
            throw std::invalid_argument("provided passwordhash is not valid (against validator)");
        }
    }
    // saves the expected length to validate the result
    // need to clear header bytes to calculate the length. If it is not clear getHeaderLength() may return the current length
    this->header_bytes.setLen(0);
    unsigned int len = this->getHeaderLength();

    Bytes dataheader = Bytes(len);
    try {
        Bytes::fromLong(this->file_size.value(), true).addcopyToBytes(dataheader);  // add file size
        Bytes::fromLong(len, true).addcopyToBytes(dataheader);         // add hash size
        dataheader.addByte(this->dh.getFileDataMode());     // add file mode byte
        dataheader.addByte(this->dh.getHashMode());         // add hash mode byte
        dataheader.addByte(this->dh.chainhash1.getMode());  // add first chainhash mode byte
        Bytes::fromLong(this->dh.chainhash1.getIters(), true).addcopyToBytes(dataheader);   // add iterations for the first chainhash
        dataheader.addByte(this->dh.chainhash1.getChainHashData()->getLen());               // add datablock length byte
        this->dh.chainhash1.getChainHashData()->getDataBlock().addcopyToBytes(dataheader);  // add first datablock
        dataheader.addByte(this->dh.chainhash2.getMode());                                  // add second chainhash mode
        Bytes::fromLong(this->dh.chainhash2.getIters(), true).addcopyToBytes(dataheader);   // add iterations for the second chainhash
        dataheader.addByte(this->dh.chainhash2.getChainHashData()->getLen());               // add datablock length byte
        this->dh.chainhash2.getChainHashData()->getDataBlock().addcopyToBytes(dataheader);  // add second datablock
        this->dh.getValidPasswordHash().addcopyToBytes(dataheader);                         // add password validator
        // generate the salt with random bytes
        Bytes rand_salt(this->hash_size);
        rand_salt.fillrandom();
        this->dh.setEncSalt(rand_salt);                    // set the random generated encrypted salt
        this->dh.getEncSalt().addcopyToBytes(dataheader);  // add encrypted salt
    } catch (std::length_error& ex) {
        // trying to add more bytes than previously calculated
        PLOG_ERROR << "trying to add more bytes to the header than previously calculated (error msg: " << ex.what() << ")";
        throw std::logic_error("trying to add more bytes to the header than previously calculated");
    }
    if (dataheader.getLen() != len) {  // checks if the length is equal to the expected length
        PLOG_FATAL << "calculated header has not the expected length (expected: " << +len << ", actual: " << +dataheader.getLen() << ")";
        throw std::logic_error("calculated header has not the expected length");
    }
    this->header_bytes = dataheader;
}

DataHeaderParts DataHeader::getDataHeaderParts() const {
    // getter for the dataheader parts
    if (!this->isComplete()) {
        // dataheader parts are not ready
        PLOG_ERROR << "not all required data is set to get DataHeaderParts";
        throw std::logic_error("Cannot get DataHeaderParts because its not complete");
    }
    return this->dh;
}

ErrorStruct<std::unique_ptr<DataHeader>> DataHeader::setHeaderBytes(Bytes& fileBytes) noexcept {
    // sets the header bytes by taking the first bytes of the file
    // init error struct
    ErrorStruct<std::unique_ptr<DataHeader>> err{FAIL, ERR, "An error occurred while reading the header", "setHeaderBytes"};
    std::unique_ptr<DataHeader> dh = nullptr;
    // setting the header parts
    //********************* FILEMODE *********************
    unsigned char fmode;
    u_int16_t index = 0;  // index of the current byte
    try {
        // loading file mode
        fmode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enogh data to read the file mode (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "File mode";
        err.what = ex.what();
        return err;
    }
    index += 1;

    // ********************* HASH FUNCTION *********************
    unsigned char hmode;
    try {
        // loading and setting hash mode
        hmode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        dh = std::make_unique<DataHeader>(HModes(hmode));
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the hash mode (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Hash mode";
        err.what = ex.what();
        return err;
    } catch (const std::invalid_argument& ex) {
        // the hash mode is invalid
        PLOG_ERROR << "invalid hash mode found in data (hash_mode: " << +hmode << ") (error msg: " << ex.what() << ")";
        err.errorCode = ERR_HASHMODE_INVALID;
        err.errorInfo = hmode;
        err.what = ex.what();
        return err;
    }
    index += 1;

    // setting the file data mode
    try {
        dh->setFileDataMode(FModes(fmode));
    } catch (const std::invalid_argument& ex) {
        // the file mode is invalid
        PLOG_ERROR << "invalid file mode found in data (file_mode: " << +fmode << ") (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_FILEMODE_INVALID;
        err.errorInfo = fmode;
        err.what = ex.what();
        return err;
    }

    // ********************* CHAINHASH1 *********************
    std::shared_ptr<Format> format1 = nullptr;
    std::shared_ptr<ChainHashData> chd1 = nullptr;
    unsigned char ch1mode;
    try {
        ch1mode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        format1 = std::make_shared<Format>(CHModes(ch1mode));
        chd1 = std::make_shared<ChainHashData>(*format1);
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the chainhash mode 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash mode 1";
        err.what = ex.what();
        return err;
    } catch (const std::invalid_argument& ex) {
        // the chainhash mode is invalid
        PLOG_ERROR << "invalid chainhash mode 1 found in data (chainhash_mode 1: " << +ch1mode << ") (error msg: " << ex.what() << ")";
        err.errorCode = ERR_CHAINHASHMODE_FORMAT_INVALID;
        err.errorInfo = ch1mode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash mode 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash mode 1";
        err.what = ex.what();
        return err;
    }
    index += 1;
    // chainhash 1 iters
    u_int64_t ch1iters;
    try {
        ch1iters = fileBytes.copySubBytes(index, index + 8).toLong();
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the chainhash iters 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash iters 1";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash iters 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash iters 1";
        err.what = ex.what();
        return err;
    }
    index += 8;
    // chainhash 1 data block len
    unsigned char ch1datablocklen;
    try {
        ch1datablocklen = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the chainhash data block len 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash data block len 1";
        err.what = ex.what();
        return err;
    }
    index += 1;
    // chainhash 1 data block
    int data_len = 0;
    bool copied = false;
    Bytes tmp{255};
    for (NameLen nl : format1->getNameLenList()) {
        try {
            bool copied = false;
            if (nl.len != 0) {
                // got a data part with set length
                tmp = fileBytes.copySubBytes(index, index + nl.len);
                copied = true;
                chd1->addBytes(tmp);
                index += nl.len;
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                tmp = fileBytes.copySubBytes(index, index + (ch1datablocklen - data_len));
                copied = true;
                chd1->addBytes(tmp);
                data_len = ch1datablocklen;
                index += (ch1datablocklen - data_len);
                break;
            }

        } catch (const std::length_error& ex) {
            if (!copied) {
                // copySubBytes failed because the length is reached
                PLOG_ERROR << "not enough data to read the chainhash data block 1 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_NOT_ENOUGH_DATA;
                err.errorInfo = "Chainhash data block 1";
                err.what = ex.what();
                return err;
            } else {
                // adding to much bytes
                PLOG_ERROR << "adding to much bytes to the chainhash data block 1 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
                err.errorInfo = tmp.toHex();
                err.what = ex.what();
                return err;
            }
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            PLOG_ERROR << "invalid format of the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            PLOG_ERROR << "adding bytes to the chainhash data block 1 but its already completed (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while reading the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while reading the chainhash data block 1";
            err.what = ex.what();
            return err;
        }
    }
    assert(chd1->getLen() == ch1datablocklen);
    assert(data_len == ch1datablocklen);
    // chainhash 1
    try {
        dh->setChainHash1(ChainHash{CHModes(ch1mode), ch1iters, chd1});
    } catch (const std::invalid_argument& ex) {
        // the chainhash is invalid
        PLOG_ERROR << "invalid chainhash 1 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_CHAINHASH1_INVALID;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting the chainhash 1 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting the chainhash data block 1";
        err.what = ex.what();
        return err;
    }

    // ********************* CHAINHASH2 *********************
    std::shared_ptr<Format> format2 = nullptr;
    std::shared_ptr<ChainHashData> chd2 = nullptr;
    unsigned char ch2mode;
    try {
        ch2mode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        format2 = std::make_shared<Format>(CHModes(ch2mode));
        chd2 = std::make_shared<ChainHashData>(*format2);
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the chainhash mode 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash mode 2";
        err.what = ex.what();
        return err;
    } catch (const std::invalid_argument& ex) {
        // the chainhash mode is invalid
        PLOG_ERROR << "invalid chainhash mode 2 found in data (chainhash_mode 2: " << +ch2mode << ") (error msg: " << ex.what() << ")";
        err.errorCode = ERR_CHAINHASHMODE_FORMAT_INVALID;
        err.errorInfo = ch2mode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash mode 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash mode 2";
        err.what = ex.what();
        return err;
    }
    index += 1;
    // chainhash 2 iters
    u_int64_t ch2iters;
    try {
        ch2iters = fileBytes.copySubBytes(index, index + 8).toLong();
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the chainhash iters 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash iters 2";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash iters 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash iters 2";
        err.what = ex.what();
        return err;
    }
    index += 8;
    // chainhash 2 data block len
    unsigned char ch2datablocklen;
    try {
        ch2datablocklen = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the chainhash data block len 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash data block len 2";
        err.what = ex.what();
        return err;
    }
    index += 1;
    // chainhash 2 data block
    data_len = 0;
    copied = false;
    for (NameLen nl : format2->getNameLenList()) {
        try {
            bool copied = false;
            if (nl.len != 0) {
                // got a data part with set length
                tmp = fileBytes.copySubBytes(index, index + nl.len);
                copied = true;
                chd2->addBytes(tmp);
                index += nl.len;
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                tmp = fileBytes.copySubBytes(index, index + (ch2datablocklen - data_len));
                copied = true;
                chd2->addBytes(tmp);
                data_len = ch2datablocklen;
                index += (ch2datablocklen - data_len);
                break;
            }

        } catch (const std::length_error& ex) {
            if (!copied) {
                // popFirstBytes returned an empty optional
                PLOG_ERROR << "not enough data to read the chainhash data block 2 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_NOT_ENOUGH_DATA;
                err.errorInfo = "Chainhash data block 2";
                err.what = ex.what();
                return err;
            } else {
                // adding to much bytes
                PLOG_ERROR << "adding to much bytes to the chainhash data block 2 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
                err.errorInfo = tmp.toHex();
                err.what = ex.what();
                return err;
            }
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            PLOG_ERROR << "invalid format of the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            PLOG_ERROR << "adding bytes to the chainhash data block 2 but its already completed (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while reading the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while reading the chainhash data block 2";
            err.what = ex.what();
            return err;
        }
    }
    assert(chd2->getLen() == ch2datablocklen);
    assert(data_len == ch2datablocklen);
    // chainhash 2
    try {
        dh->setChainHash2(ChainHash{CHModes(ch2mode), ch2iters, chd2});
    } catch (const std::invalid_argument& ex) {
        // the chainhash is invalid
        PLOG_ERROR << "invalid chainhash 2 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_CHAINHASH2_INVALID;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting the chainhash 2 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting the chainhash data block 2";
        err.what = ex.what();
        return err;
    }

    // password validator hash
    try {
        dh->setValidPasswordHashBytes(fileBytes.copySubBytes(index, index + dh->hash_size));
        index += dh->hash_size;
    } catch (const std::length_error& ex) {
        // copySubBytes fauiled because the length is reached
        PLOG_ERROR << "not enough data to read the password validator hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password validator hash";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting and reading the password validator hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting and reading the password validator hash";
        err.what = ex.what();
        return err;
    }

    // encrypted salt
    try {
        dh->setEncSalt(fileBytes.copySubBytes(index, index + dh->hash_size));
        index += dh->hash_size;
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the salt hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password salt hash";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting and reading the salt hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting and reading the salt hash";
        err.what = ex.what();
        return err;
    }
    assert(index == fileBytes.getLen());
    return ErrorStruct<std::unique_ptr<DataHeader>>::createMove(std::move(dh));
}

ErrorStruct<std::unique_ptr<DataHeader>> DataHeader::setHeaderBytes(std::ifstream& file) noexcept {
    // sets the header bytes by taking the first bytes of the file
    // init error struct
    auto start = file.tellg();
    ErrorStruct<std::unique_ptr<DataHeader>> err{FAIL, ERR, "An error occurred while reading the header", "setHeaderBytes"};
    std::unique_ptr<DataHeader> dh = nullptr;
    // setting the header parts
    //********************* FILEMODE *********************
    unsigned char fmode;
    // loading file mode
    if (file.readsome(reinterpret_cast<char*>(&fmode), 1) != 1) {
        // readsome could not read the file mode
        PLOG_ERROR << "not enogh data to read the file mode";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "File mode";
        return err;
    }

    // ********************* HASH FUNCTION *********************
    unsigned char hmode;

    // loading and setting hash mode
    if (file.readsome(reinterpret_cast<char*>(&hmode), 1) != 1) {
        // readsome could not read the hash mode
        PLOG_ERROR << "not enogh data to read the hash mode";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Hash mode";
        return err;
    }
    try {
        dh = std::make_unique<DataHeader>(HModes(hmode));
    } catch (const std::invalid_argument& ex) {
        // the hash mode is invalid
        PLOG_ERROR << "invalid hash mode found in data (hash_mode: " << +hmode << ") (error msg: " << ex.what() << ")";
        err.errorCode = ERR_HASHMODE_INVALID;
        err.errorInfo = hmode;
        err.what = ex.what();
        return err;
    }

    // setting the file data mode
    try {
        dh->setFileDataMode(FModes(fmode));
    } catch (const std::invalid_argument& ex) {
        // the file mode is invalid
        PLOG_ERROR << "invalid file mode found in data (file_mode: " << +fmode << ") (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_FILEMODE_INVALID;
        err.errorInfo = fmode;
        err.what = ex.what();
        return err;
    }

    // ********************* CHAINHASH1 *********************
    std::shared_ptr<Format> format1 = nullptr;
    std::shared_ptr<ChainHashData> chd1 = nullptr;
    unsigned char ch1mode;
    // loading and setting chainhash1 mode
    if (file.readsome(reinterpret_cast<char*>(&ch1mode), 1) != 1) {
        // readsome could not read the hash mode
        PLOG_ERROR << "not enogh data to read the chainhash1 mode";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash1 mode";
        return err;
    }
    try {
        format1 = std::make_shared<Format>(CHModes(ch1mode));
        chd1 = std::make_shared<ChainHashData>(*format1);
    } catch (const std::invalid_argument& ex) {
        // the chainhash mode is invalid
        PLOG_ERROR << "invalid chainhash mode 1 found in data (chainhash_mode 1: " << +ch1mode << ") (error msg: " << ex.what() << ")";
        err.errorCode = ERR_CHAINHASHMODE_FORMAT_INVALID;
        err.errorInfo = ch1mode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting up the format1 and chainhashdata1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting up the format1 and chainhashdata1";
        err.what = ex.what();
        return err;
    }
    // chainhash 1 iters
    u_int64_t ch1iters;
    Bytes tmp{8};
    // loading and chainhash1 iters
    if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), 8) != 8) {
        // readsome could not read the chainhash1 iters
        PLOG_ERROR << "not enogh data to read the chainhash1 iters";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash1 iters";
        return err;
    }
    try {
        tmp.setLen(8);
        ch1iters = tmp.toLong();
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash iters 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash iters 1";
        err.what = ex.what();
        return err;
    }
    // chainhash 1 data block len
    unsigned char ch1datablocklen;
    // loading and chainhash1 blocklen
    if (file.readsome(reinterpret_cast<char*>(&ch1datablocklen), 1) != 1) {
        // readsome could not read the chainhash1 blocklen
        PLOG_ERROR << "not enogh data to read the chainhash1 blocklen";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash1 blocklen";
        return err;
    }
    // chainhash 1 data block
    int data_len = 0;
    bool copied = false;
    tmp = Bytes(255);
    for (NameLen nl : format1->getNameLenList()) {
        try {
            bool copied = false;
            if (nl.len != 0) {
                // got a data part with set length
                if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), nl.len) != nl.len) {
                    throw std::length_error("not enough data to read the chainhash data block 1");
                }
                tmp.setLen(nl.len);
                copied = true;
                chd1->addBytes(tmp);
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), ch1datablocklen - data_len) != ch1datablocklen - data_len) {
                    throw std::length_error("not enough data to read the chainhash data block 1");
                }
                tmp.setLen(ch1datablocklen - data_len);
                copied = true;
                chd1->addBytes(tmp);
                data_len = ch1datablocklen;
                break;
            }

        } catch (const std::length_error& ex) {
            if (!copied) {
                // readsome could not read the chainhash1 block
                PLOG_ERROR << "not enough data to read the chainhash data block 1";
                err.errorCode = ERR_NOT_ENOUGH_DATA;
                err.errorInfo = "Chainhash data block 1";
                return err;
            } else {
                // adding to much bytes
                PLOG_ERROR << "adding to much bytes to the chainhash data block 1 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
                err.errorInfo = tmp.toHex();
                err.what = ex.what();
                return err;
            }
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            PLOG_ERROR << "invalid format of the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            PLOG_ERROR << "adding bytes to the chainhash data block 1 but its already completed (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while reading the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while reading the chainhash data block 1";
            err.what = ex.what();
            return err;
        }
    }
    assert(chd1->getLen() == ch1datablocklen);
    assert(data_len == ch1datablocklen);
    // chainhash 1
    try {
        dh->setChainHash1(ChainHash{CHModes(ch1mode), ch1iters, chd1});
    } catch (const std::invalid_argument& ex) {
        // the chainhash is invalid
        PLOG_ERROR << "invalid chainhash 1 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_CHAINHASH1_INVALID;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting the chainhash 1 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting the chainhash data block 1";
        err.what = ex.what();
        return err;
    }

    // ********************* CHAINHASH2 *********************
    std::shared_ptr<Format> format2 = nullptr;
    std::shared_ptr<ChainHashData> chd2 = nullptr;
    unsigned char ch2mode;
    // loading and setting chainhash2 mode
    if (file.readsome(reinterpret_cast<char*>(&ch2mode), 1) != 1) {
        // readsome could not read the hash mode
        PLOG_ERROR << "not enogh data to read the chainhash2 mode";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash2 mode";
        return err;
    }
    try {
        format2 = std::make_shared<Format>(CHModes(ch2mode));
        chd2 = std::make_shared<ChainHashData>(*format2);
    } catch (const std::invalid_argument& ex) {
        // the chainhash mode is invalid
        PLOG_ERROR << "invalid chainhash mode 2 found in data (chainhash_mode 2: " << +ch2mode << ") (error msg: " << ex.what() << ")";
        err.errorCode = ERR_CHAINHASHMODE_FORMAT_INVALID;
        err.errorInfo = ch2mode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash mode 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash mode 2";
        err.what = ex.what();
        return err;
    }
    // chainhash 2 iters
    u_int64_t ch2iters;
    tmp = Bytes(8);
    // loading and chainhash2 iters
    if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), 8) != 8) {
        // readsome could not read the chainhash2 iters
        PLOG_ERROR << "not enogh data to read the chainhash2 iters";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash2 iters";
        return err;
    }
    try {
        tmp.setLen(8);
        ch2iters = tmp.toLong();
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash iters 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash iters 2";
        err.what = ex.what();
        return err;
    }
    // chainhash 2 data block len
    unsigned char ch2datablocklen;
    // loading and chainhash1 blocklen
    if (file.readsome(reinterpret_cast<char*>(&ch2datablocklen), 1) != 1) {
        // readsome could not read the chainhash2 blocklen
        PLOG_ERROR << "not enogh data to read the chainhash2 blocklen";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash2 blocklen";
        return err;
    }
    // chainhash 2 data block
    data_len = 0;
    copied = false;
    for (NameLen nl : format2->getNameLenList()) {
        try {
            bool copied = false;
            if (nl.len != 0) {
                // got a data part with set length
                if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), nl.len) != nl.len) {
                    throw std::length_error("not enough data to read the chainhash data block 2");
                }
                tmp.setLen(nl.len);
                copied = true;
                chd2->addBytes(tmp);
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), ch2datablocklen - data_len) != ch2datablocklen - data_len) {
                    throw std::length_error("not enough data to read the chainhash data block 2");
                }
                tmp.setLen(ch2datablocklen - data_len);
                copied = true;
                chd2->addBytes(tmp);
                data_len = ch2datablocklen;
                break;
            }

        } catch (const std::length_error& ex) {
            if (!copied) {
                // popFirstBytes returned an empty optional
                PLOG_ERROR << "not enough data to read the chainhash data block 2 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_NOT_ENOUGH_DATA;
                err.errorInfo = "Chainhash data block 2";
                err.what = ex.what();
                return err;
            } else {
                // adding to much bytes
                PLOG_ERROR << "adding to much bytes to the chainhash data block 2 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
                err.errorInfo = tmp.toHex();
                err.what = ex.what();
                return err;
            }
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            PLOG_ERROR << "invalid format of the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            PLOG_ERROR << "adding bytes to the chainhash data block 2 but its already completed (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = tmp.toHex();
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while reading the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while reading the chainhash data block 2";
            err.what = ex.what();
            return err;
        }
    }
    assert(chd2->getLen() == ch2datablocklen);
    assert(data_len == ch2datablocklen);
    // chainhash 2
    try {
        dh->setChainHash2(ChainHash{CHModes(ch2mode), ch2iters, chd2});
    } catch (const std::invalid_argument& ex) {
        // the chainhash is invalid
        PLOG_ERROR << "invalid chainhash 2 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_CHAINHASH2_INVALID;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting the chainhash 2 (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting the chainhash data block 2";
        err.what = ex.what();
        return err;
    }

    // password validator hash
    try {
        Bytes tmp = Bytes(dh->hash_size);
        if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), dh->hash_size) != dh->hash_size) {
            // readsome could not read the password validator hash
            PLOG_ERROR << "not enogh data to read the password validator hash";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Password validator hash";
            return err;
        }
        tmp.setLen(dh->hash_size);
        dh->setValidPasswordHashBytes(tmp);
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting and reading the password validator hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting and reading the password validator hash";
        err.what = ex.what();
        return err;
    }

    // encrypted salt
    try {
        Bytes tmp = Bytes(dh->hash_size);
        if (file.readsome(reinterpret_cast<char*>(tmp.getBytes()), dh->hash_size) != dh->hash_size) {
            // readsome could not read the encrypted salt
            PLOG_ERROR << "not enogh data to read the encrypted salt";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Encrypted salt";
            return err;
        }
        tmp.setLen(dh->hash_size);
        dh->setEncSalt(tmp);
    } catch (const std::length_error& ex) {
        // copySubBytes failed because the length is reached
        PLOG_ERROR << "not enough data to read the salt hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password salt hash";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting and reading the salt hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting and reading the salt hash";
        err.what = ex.what();
        return err;
    }
    assert(start - file.tellg() == dh->getHeaderLength());
    return ErrorStruct<std::unique_ptr<DataHeader>>::createMove(std::move(dh));
}

ErrorStruct<std::unique_ptr<DataHeader>> DataHeader::setHeaderParts(const DataHeaderParts& dhp) noexcept {
    // creating a new header by setting the header parts
    ErrorStruct<std::unique_ptr<DataHeader>> err{FAIL, ERR, ""};
    try {
        // these methods throw exceptions if the data is invalid
        std::unique_ptr<DataHeader> dh = std::make_unique<DataHeader>(dhp.getHashMode());  // setting the hash mode
        dh->setFileDataMode(dhp.getFileDataMode());                                        // setting the file data mode
        dh->setChainHash1(dhp.chainhash1);                                                 // setting the chainhash 1
        dh->setChainHash2(dhp.chainhash2);                                                 // setting the chainhash 2
        dh->setValidPasswordHashBytes(dhp.getValidPasswordHash());                         // setting the password validator hash
        if (dhp.isEncSaltSet())                                                            // enc salt is not necessary
            dh->setEncSalt(dhp.getEncSalt());                                              // setting the encrypted salt

        // success
        return ErrorStruct<std::unique_ptr<DataHeader>>::createMove(std::move(dh));

    } catch (const std::exception& ex) {
        // some error occurred
        PLOG_ERROR << "An error occurred while setting the header parts (error msg: " << ex.what() << ")";
        err.errorInfo = "An error occurred while setting the header parts";
        err.what = ex.what();
        return err;
    }
}
