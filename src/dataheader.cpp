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
    this->hash_size = HashModes::getHash(hash_mode)->getHashSize();  // gets the hash size of the hash that corresponds to the given mode
}

bool DataHeader::isComplete() const noexcept {
    // checks if the dataheader has everything set
    if (this->dh.isComplete(this->hash_size)) return true;
    return false;
}

unsigned int DataHeader::getHeaderLength() const noexcept {
    // gets the header len, if there is no header set try to calculate the len, else return 0
    if (this->header_bytes.getLen() > 0) return this->header_bytes.getLen();                                                                  // header bytes are set, so we get this length
    if (this->dh.chainhash1.valid() && this->dh.chainhash2.valid())                                                                           // all data set to calculate the header length
        return 22 + 2 * this->hash_size + this->dh.chainhash1.getChainHashData().getLen() + this->dh.chainhash2.getChainHashData().getLen();  // dataheader.md
    return 0;                                                                                                                                 // not enough infos to get the header length
}

int DataHeader::getHashSize() const noexcept {
    // gets the hash size
    return this->hash_size;
}

void DataHeader::setChainHash1(const ChainHash chainhash, const unsigned char len) {
    // sets the information about the first chainhash
    PLOG_VERBOSE << "setting chainhash1 (mode: " << +chainhash.getMode() << ", len: " << +len << ", iters: " << chainhash.getIters() << ")";
    if (len != chainhash.getChainHashData().getLen()) {  // validates the datablock length
        PLOG_ERROR << "length of the datablock does not match with the given length (len: " << +len << ", datablock len: " << +chainhash.getChainHashData().getLen() << ")";
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    // set the information to the object
    this->dh.chainhash1 = ChainHash(chainhash.getMode(), chainhash.getIters(), chainhash.getChainHashData());
}

void DataHeader::setChainHash2(const ChainHash chainhash, const unsigned char len) {
    // sets the information about the second chainhash
    PLOG_VERBOSE << "setting chainhash2 (mode: " << +chainhash.getMode() << ", len: " << +len << ", iters: " << chainhash.getIters() << ")";
    if (len != chainhash.getChainHashData().getLen()) {  // validates the datablock length
        PLOG_ERROR << "length of the datablock does not match with the given length (len: " << +len << ", datablock len: " << +chainhash.getChainHashData().getLen() << ")";
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    // set the information to the object
    this->dh.chainhash2 = ChainHash(chainhash.getMode(), chainhash.getIters(), chainhash.getChainHashData());
}

void DataHeader::setFileDataMode(const FModes file_mode) {
    // sets the file data mode
    PLOG_VERBOSE << "setting file mode: " << +file_mode;
    this->dh.setFileDataMode(file_mode);
}

void DataHeader::setValidPasswordHashBytes(const Bytes validBytes) {
    // set the passwordhash validator hash
    PLOG_VERBOSE << "setting password validator hash: " << toHex(validBytes);
    this->dh.setValidPasswordHash(validBytes);
}

void DataHeader::setEncSalt(const Bytes salt) {
    // sets the salt
    PLOG_VERBOSE << "setting salt: " << toHex(salt);
    this->dh.setEncSalt(salt);
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

void DataHeader::calcHeaderBytes(const Bytes passwordhash) {
    // calculates the header
    PLOG_VERBOSE << "calculating header bytes " << (passwordhash.isEmpty() ? "without verifying" : "with verifying");
    if (!this->isComplete()) {
        // header bytes cannot be calculated (data is missing)
        PLOG_ERROR << "not all required data is set to calculate the header bytes";
        throw std::logic_error("not all required data is set to calculate the header bytes");
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
    this->header_bytes.clear();
    unsigned int len = this->getHeaderLength();

    Bytes dataheader = Bytes();
    Bytes tmp = Bytes();
    dataheader.addByte(this->dh.getFileDataMode());     // add file mode byte
    dataheader.addByte(this->dh.getHashMode());         // add hash mode byte
    dataheader.addByte(this->dh.chainhash1.getMode());  // add first chainhash mode byte
    tmp.setBytes(LongToCharVec(this->dh.chainhash1.getIters()));
    dataheader.addBytes(tmp);                                                    // add iterations for the first chainhash
    dataheader.addByte(this->dh.chainhash1.getChainHashData().getLen());         // add datablock length byte
    dataheader.addBytes(this->dh.chainhash1.getChainHashData().getDataBlock());  // add first datablock
    dataheader.addByte(this->dh.chainhash2.getMode());                           // add second chainhash mode
    tmp.setBytes(LongToCharVec(this->dh.chainhash2.getIters()));
    dataheader.addBytes(tmp);                                                    // add iterations for the second chainhash
    dataheader.addByte(this->dh.chainhash2.getChainHashData().getLen());         // add datablock length byte
    dataheader.addBytes(this->dh.chainhash2.getChainHashData().getDataBlock());  // add second datablock
    dataheader.addBytes(this->dh.getValidPasswordHash());                        // add password validator
    // generate the salt with random bytes
    this->dh.setEncSalt(Bytes(this->hash_size));  // set the random generated encrypted salt
    dataheader.addBytes(this->dh.getEncSalt());   // add encrypted salt
    if (dataheader.getLen() != len) {             // checks if the length is equal to the expected length
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

ErrorStruct<DataHeader> DataHeader::setHeaderBytes(Bytes& fileBytes) noexcept {
    // sets the header bytes by taking the first bytes of the file
    // init error struct
    ErrorStruct<DataHeader> err{FAIL, ERR, "An error occurred while reading the header", "setHeaderBytes"};
    // setting the header parts
    //********************* FILEMODE *********************
    unsigned char fmode;
    Bytes tmp;
    try {
        // loading file mode
        tmp = fileBytes.popFirstBytes(1).value();
        fmode = tmp.getBytes()[0];
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        PLOG_ERROR << "not enough data to read the file mode (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "File mode";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the file mode (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the file mode";
        err.what = ex.what();
        return err;
    }

    // ********************* HASH FUNCTION *********************
    unsigned char hmode;
    try {
        // loading and setting hash mode
        tmp = fileBytes.popFirstBytes(1).value();
        hmode = tmp.getBytes()[0];
        err.setReturnValue(DataHeader(HModes(hmode)));
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
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
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the hash mode (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the hash mode";
        err.what = ex.what();
        return err;
    }

    // setting the file data mode
    try {
        err.success = SUCCESS;
        // need to set to success to access the return value
        DataHeader dh = err.returnValue();
        err.success = FAIL;
        dh.setFileDataMode(FModes(fmode));
        err.setReturnValue(dh);
    } catch (const std::invalid_argument& ex) {
        // the file mode is invalid
        PLOG_ERROR << "invalid file mode found in data (file_mode: " << +fmode << ") (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_FILEMODE_INVALID;
        err.errorInfo = fmode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting the file mode (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting the file mode";
        err.what = ex.what();
        return err;
    }

    // ********************* CHAINHASH1 *********************
    Format format1{CHModes(CHAINHASH_NORMAL)};
    ChainHashData chd1{format1};
    unsigned char ch1mode;
    try {
        tmp = fileBytes.popFirstBytes(1).value();
        ch1mode = tmp.getBytes()[0];
        format1 = Format(CHModes(ch1mode));
        chd1 = ChainHashData(format1);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
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
    // chainhash 1 iters
    u_int64_t ch1iters;
    try {
        tmp = fileBytes.popFirstBytes(8).value();
        ch1iters = toLong(tmp);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
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
    // chainhash 1 data block len
    unsigned char ch1datablocklen;
    try {
        tmp = fileBytes.popFirstBytes(1).value();
        ch1datablocklen = tmp.getBytes()[0];
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        PLOG_ERROR << "not enough data to read the chainhash data block len 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash data block len 1";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash data block len 1 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash data block len 1";
        err.what = ex.what();
        return err;
    }
    // chainhash 1 data block
    int data_len = 0;
    for (NameLen nl : format1.getNameLenList()) {
        try {
            if (nl.len != 0) {
                // got a data part with set length
                tmp = fileBytes.popFirstBytes(nl.len).value();
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                tmp = fileBytes.popFirstBytes(255 - data_len).value();
                chd1.addBytes(tmp);
                break;
            }
            chd1.addBytes(tmp);
        } catch (const std::bad_optional_access& ex) {
            // popFirstBytes returned an empty optional
            PLOG_ERROR << "not enough data to read the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Chainhash data block 1";
            err.what = ex.what();
            return err;
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            PLOG_ERROR << "invalid format of the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::length_error& ex) {
            // adding to much bytes
            PLOG_ERROR << "adding to much bytes to the chainhash data block 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            PLOG_ERROR << "adding bytes to the chainhash data block 1 but its already completed (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = toHex(tmp);
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
    // chainhash 1
    try {
        err.success = SUCCESS;
        // need to set to success to access the return value
        DataHeader dh = err.returnValue();
        err.success = FAIL;
        dh.setChainHash1(ChainHash{CHModes(ch1mode), ch1iters, chd1}, ch1datablocklen);
        err.setReturnValue(dh);
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
    Format format2{CHModes(CHAINHASH_NORMAL)};
    ChainHashData chd2{format2};
    unsigned char ch2mode;
    try {
        tmp = fileBytes.popFirstBytes(1).value();
        ch2mode = tmp.getBytes()[0];
        format2 = Format(CHModes(ch2mode));
        chd2 = ChainHashData(format2);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
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
    // chainhash 2 iters
    u_int64_t ch2iters;
    try {
        tmp = fileBytes.popFirstBytes(8).value();
        ch2iters = toLong(tmp);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
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
    // chainhash 2 data block len
    unsigned char ch2datablocklen;
    try {
        tmp = fileBytes.popFirstBytes(1).value();
        ch2datablocklen = tmp.getBytes()[0];
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        PLOG_ERROR << "not enough data to read the chainhash data block len 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash data block len 2";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while reading the chainhash data block len 2 (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the chainhash data block len 2";
        err.what = ex.what();
        return err;
    }
    // chainhash 2 data block
    data_len = 0;
    for (NameLen nl : format2.getNameLenList()) {
        try {
            if (nl.len != 0) {
                // got a data part with set length
                tmp = fileBytes.popFirstBytes(nl.len).value();
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                tmp = fileBytes.popFirstBytes(255 - data_len).value();
                chd2.addBytes(tmp);
                break;
            }
            chd2.addBytes(tmp);
        } catch (const std::bad_optional_access& ex) {
            // popFirstBytes returned an empty optional
            PLOG_ERROR << "not enough data to read the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Chainhash data block 2";
            err.what = ex.what();
            return err;
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            PLOG_ERROR << "invalid format of the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::length_error& ex) {
            // adding to much bytes
            PLOG_ERROR << "adding to much bytes to the chainhash data block 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            PLOG_ERROR << "adding bytes to the chainhash data block 2 but its already completed (error msg: " << ex.what() << ")";
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = toHex(tmp);
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
    // chainhash 2
    try {
        err.success = SUCCESS;
        // need to set to success to access the return value
        DataHeader dh = err.returnValue();
        err.success = FAIL;
        dh.setChainHash2(ChainHash{CHModes(ch2mode), ch2iters, chd2}, ch2datablocklen);
        err.setReturnValue(dh);
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
        err.success = SUCCESS;
        // need to set to success to access the return value
        DataHeader dh = err.returnValue();
        err.success = FAIL;
        tmp = fileBytes.popFirstBytes(dh.hash_size).value();
        dh.setValidPasswordHashBytes(tmp);
        err.setReturnValue(dh);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        PLOG_ERROR << "not enough data to read the password validator hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password validator hash";
        err.what = ex.what();
        return err;
    } catch (const std::length_error& ex) {
        // password validator hash has the wrong len
        PLOG_ERROR << "length of the password validator hash does not match with the hash size (validator hash len: " << +tmp.getLen() << ", hash size: " << +err.returnValue().hash_size
                   << ") (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_LEN_INVALID;
        err.errorInfo = toHex(tmp);
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
        err.success = SUCCESS;
        // need to set to success to access the return value
        DataHeader dh = err.returnValue();
        err.success = FAIL;
        tmp = fileBytes.popFirstBytes(dh.hash_size).value();
        dh.setEncSalt(tmp);
        err.setReturnValue(dh);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        PLOG_ERROR << "not enough data to read the salt hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password salt hash";
        err.what = ex.what();
        return err;
    } catch (const std::length_error& ex) {
        // salt hash has the wrong len
        PLOG_ERROR << "length of the salt hash does not match with the hash size (salt hash len: " << +tmp.getLen() << ", hash size: " << +err.returnValue().hash_size << ") (error msg: " << ex.what()
                   << ")";
        err.success = FAIL;
        err.errorCode = ERR_LEN_INVALID;
        err.errorInfo = toHex(tmp);
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
    err.success = SUCCESS;
    return err;
}

ErrorStruct<DataHeader> DataHeader::setHeaderParts(const DataHeaderParts dhp) noexcept {
    // creating a new header by setting the header parts
    ErrorStruct<DataHeader> err{FAIL, ERR, ""};
    try {
        // these methods throw exceptions if the data is invalid
        DataHeader dh{dhp.getHashMode()};                                              // setting the hash mode
        dh.setFileDataMode(dhp.getFileDataMode());                                     // setting the file data mode
        dh.setChainHash1(dhp.chainhash1, dhp.chainhash1.getChainHashData().getLen());  // setting the chainhash 1
        dh.setChainHash2(dhp.chainhash2, dhp.chainhash2.getChainHashData().getLen());  // setting the chainhash 2
        dh.setValidPasswordHashBytes(dhp.getValidPasswordHash());                      // setting the password validator hash
        if (dhp.isEncSaltSet())                                                        // enc salt is not necessary
            dh.setEncSalt(dhp.getEncSalt());                                           // setting the encrypted salt

        // success
        err.setReturnValue(dh);
        err.success = SUCCESS;
        return err;

    } catch (const std::exception& ex) {
        // some error occurred
        PLOG_ERROR << "An error occurred while setting the header parts (error msg: " << ex.what() << ")";
        err.errorInfo = "An error occurred while setting the header parts";
        err.what = ex.what();
        return err;
    }
}
