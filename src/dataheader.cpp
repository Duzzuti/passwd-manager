/*
this file contains the implementations of Data header class
*/
#include "dataheader.h"

#include "file_modes.h"
#include "rng.h"
#include "utility.h"

DataHeader::DataHeader(const HModes hash_mode) {
    // initialize the hash mode
    if (!HashModes::isModeValid(hash_mode)) {
        throw std::invalid_argument("invalid hash mode");
    }
    this->dh.hash_mode = hash_mode;
    Hash* hash = HashModes::getHash(hash_mode);
    this->hash_size = hash->getHashSize();  // gets the hash size of the hash that corresponds to the given mode
    delete hash;
}

bool DataHeader::isComplete() const noexcept {
    // checks if the dataheader has everything set
    if (!ChainHashModes::isModeValid(this->dh.chainhash1.mode) || !ChainHashModes::isModeValid(this->dh.chainhash2.mode) || this->dh.valid_passwordhash.getLen() != this->hash_size ||
        this->dh.valid_passwordhash.getLen() != this->hash_size || !FileModes::isModeValid(this->dh.file_mode)) {
        return false;
    }
    return true;
}

unsigned int DataHeader::getHeaderLength() const noexcept {
    // gets the header len, if there is not header set try to calculate the len, else return 0
    if (this->header_bytes.getLen() > 0) {
        return this->header_bytes.getLen();  // header bytes are set, so we get this length
    }
    if (ChainHashModes::isModeValid(this->dh.chainhash1.mode) && ChainHashModes::isModeValid(this->dh.chainhash2.mode)) {  // all data set to calculate the header length
        return 22 + 2 * this->hash_size + this->dh.chainhash1_datablock_len + this->dh.chainhash2_datablock_len;           // dataheader.md
    } else {
        return 0;  // not enough infos to get the header length
    }
}

unsigned char DataHeader::getHashSize() const noexcept {
    // gets the hash size
    return this->hash_size;
}

void DataHeader::setChainHash1(const ChainHash chainhash, const unsigned char len) {
    // sets the information about the first chainhash
    if (len != chainhash.datablock.getLen()) {  // validates the datablock length
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    ErrorStruct err = ChainHashModes::isChainHashValid(chainhash);  // validates the chainhash
    if (!err.success == SUCCESS) {
        // validates the chainhash
        throw std::invalid_argument(getErrorMessage(err));
    }
    // set the information to the object
    this->dh.chainhash1.mode = chainhash.mode;
    this->dh.chainhash1.datablock = chainhash.datablock;
    this->dh.chainhash1_datablock_len = len;
    this->dh.chainhash1.iters = chainhash.iters;
}

void DataHeader::setChainHash2(const ChainHash chainhash, const unsigned char len) {
    // sets the information about the second chainhash
    if (len != chainhash.datablock.getLen()) {  // validates the datablock length
        throw std::invalid_argument("length of the datablock does not match with the given length");
    }
    ErrorStruct err = ChainHashModes::isChainHashValid(chainhash);  // validates the chainhash
    if (!err.success == SUCCESS) {
        // validates the chainhash
        throw std::invalid_argument(getErrorMessage(err));
    }
    // set the information to the object
    this->dh.chainhash2.mode = chainhash.mode;
    this->dh.chainhash2.datablock = chainhash.datablock;
    this->dh.chainhash2_datablock_len = len;
    this->dh.chainhash2.iters = chainhash.iters;
}

void DataHeader::setFileDataMode(const FModes file_mode) {
    // sets the file data mode
    if (!FileModes::isModeValid(file_mode)) {
        throw std::invalid_argument("invalid file mode");
    }
    this->dh.file_mode = file_mode;
}

void DataHeader::setValidPasswordHashBytes(const Bytes validBytes) {
    // set the passwordhash validator hash
    if (validBytes.getLen() != this->hash_size) {  // check if the hash size is right
        throw std::length_error("Length of the given validBytes does not match with the hash size");
    }
    this->dh.valid_passwordhash = validBytes;
}

void DataHeader::setEncSalt(const Bytes salt) {
    // sets the salt
    if (salt.getLen() != this->hash_size) {  // check if the hash size is right
        throw std::length_error("Length of the given salt does not match with the hash size");
    }
    this->dh.enc_salt = salt;
}

Bytes DataHeader::getHeaderBytes() const {
    // gets the header bytes
    if (this->getHeaderLength() == 0) {
        // no header is set and there are missing information
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    if (this->getHeaderLength() != this->header_bytes.getLen()) {
        // current header has not the length that it should have (current header is out of date)
        throw std::logic_error("the calculated header length is not equal to the set header length. Call calcHeaderBytes() first");
    }
    return this->header_bytes;
}

void DataHeader::calcHeaderBytes(const Bytes passwordhash, const bool verify_pwhash) {
    // calculates the header
    if (!this->isComplete()) {
        // header bytes cannot be calculated (data is missing)
        throw std::logic_error("not all data is set to calculate the length of the header");
    }
    if (verify_pwhash) {
        // verifies the given pwhash with the currently set validator
        Hash* hash = HashModes::getHash(this->dh.hash_mode);  // gets the right hash function
        // is the chainhash from the given hash equal to the validator
        const bool isOkay = (this->dh.valid_passwordhash == ChainHashModes::performChainHash(this->dh.chainhash2, hash, passwordhash).returnValue);
        delete hash;
        if (!isOkay) {
            // given pwhash is not valid
            throw std::invalid_argument("provided passwordhash is not valid (against validator)");
        }
    }
    // saves the expected length to validate the result
    // need to clear header bytes to calculate the length. If it is not clear getHeaderLength() may return the current length
    this->header_bytes = Bytes();
    unsigned int len = this->getHeaderLength();

    Bytes dataheader = Bytes();
    Bytes tmp = Bytes();
    dataheader.addByte(this->dh.file_mode);        // add file mode byte
    dataheader.addByte(this->dh.hash_mode);        // add hash mode byte
    dataheader.addByte(this->dh.chainhash1.mode);  // add first chainhash mode byte
    tmp.setBytes(LongToCharVec(this->dh.chainhash1.iters));
    dataheader.addBytes(tmp);                                           // add iterations for the first chainhash
    dataheader.addByte(this->dh.chainhash1_datablock_len);              // add datablock length byte
    dataheader.addBytes(this->dh.chainhash1.datablock.getDataBlock());  // add first datablock
    dataheader.addByte(this->dh.chainhash2.mode);                       // add second chainhash mode
    tmp.setBytes(LongToCharVec(this->dh.chainhash2.iters));
    dataheader.addBytes(tmp);                                           // add iterations for the second chainhash
    dataheader.addByte(this->dh.chainhash2_datablock_len);              // add datablock length byte
    dataheader.addBytes(this->dh.chainhash2.datablock.getDataBlock());  // add second datablock
    dataheader.addBytes(this->dh.valid_passwordhash);                   // add password validator
    tmp.setBytes(RNG::get_random_bytes(this->hash_size));               // generate the salt with random bytes
    tmp = tmp + passwordhash;                                           // encrypt the salt with the password hash
    this->dh.enc_salt = tmp;                                            // set the encrypted salt
    dataheader.addBytes(this->dh.enc_salt);                             // add encrypted salt
    if (dataheader.getLen() != len) {                                   // checks if the length is equal to the expected length
        throw std::logic_error("calculated header has not the expected length");
    }
    this->header_bytes = dataheader;
}

DataHeaderParts DataHeader::getDataHeaderParts() const {
    // getter for the dataheader parts
    if (!this->isComplete()) {
        // dataheader parts are not ready
        throw std::logic_error("Cannot get DataHeaderParts because its not complete");
    }
    return this->dh;
}

ErrorStruct<DataHeader> DataHeader::setHeaderBytes(Bytes& fileBytes) noexcept {
    // sets the header bytes by taking the first bytes of the file
    // init error struct
    ErrorStruct<DataHeader> err{FAIL, ERR, "An error occured while reading the header", "setHeaderBytes", DataHeader(HModes(STANDARD_HASHMODE))};
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "File mode";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the file mode";
        err.what = ex.what();
        return err;
    }

    // ********************* HASH FUNCTION *********************
    unsigned char hmode;
    try {
        // loading and setting hash mode
        tmp = fileBytes.popFirstBytes(1).value();
        hmode = tmp.getBytes()[0];
        err.returnValue = DataHeader(HModes(hmode));
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Hash mode";
        err.what = ex.what();
        return err;
    } catch (const std::invalid_argument& ex) {
        // the hash mode is invalid
        err.errorCode = ERR_HASHMODE_INVALID;
        err.errorInfo = hmode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the hash mode";
        err.what = ex.what();
        return err;
    }

    // setting the file data mode
    try {
        err.returnValue.setFileDataMode(FModes(fmode));
    } catch (const std::invalid_argument& ex) {
        // the file mode is invalid
        err.errorCode = ERR_FILEMODE_INVALID;
        err.errorInfo = fmode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while setting the file mode";
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash mode 1";
        err.what = ex.what();
        return err;
    } catch (const std::invalid_argument& ex) {
        // the chainhash mode is invalid
        err.errorCode = ERR_CHAINHASHMODE_FORMAT_INVALID;
        err.errorInfo = ch1mode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the chainhash mode 1";
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash iters 1";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the chainhash iters 1";
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash data block len 1";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the chainhash data block len 1";
        err.what = ex.what();
        return err;
    }
    // chainhash 1 data block
    for (NameLen nl : format1.getNameLenList()) {
        try {
            tmp = fileBytes.popFirstBytes(nl.len).value();
            chd1.addBytes(tmp);
        } catch (const std::bad_optional_access& ex) {
            // popFirstBytes returned an empty optional
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Chainhash data block 1";
            err.what = ex.what();
            return err;
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::length_error& ex) {
            // adding to much bytes
            err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occured
            err.errorCode = ERR;
            err.errorInfo = "An error occured while reading the chainhash data block 1";
            err.what = ex.what();
            return err;
        }
    }
    // chainhash 1
    try {
        err.returnValue.setChainHash1(ChainHash{CHModes(ch1mode), ch1iters, chd1}, ch1datablocklen);
    } catch (const std::invalid_argument& ex) {
        // the chainhash is invalid
        err.errorCode = ERR_CHAINHASH1_INVALID;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while setting the chainhash data block 1";
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash mode 2";
        err.what = ex.what();
        return err;
    } catch (const std::invalid_argument& ex) {
        // the chainhash mode is invalid
        err.errorCode = ERR_CHAINHASHMODE_FORMAT_INVALID;
        err.errorInfo = ch2mode;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the chainhash mode 2";
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash iters 2";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the chainhash iters 2";
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
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash data block len 2";
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while reading the chainhash data block len 2";
        err.what = ex.what();
        return err;
    }
    // chainhash 2 data block
    for (NameLen nl : format2.getNameLenList()) {
        try {
            tmp = fileBytes.popFirstBytes(nl.len).value();
            chd2.addBytes(tmp);
        } catch (const std::bad_optional_access& ex) {
            // popFirstBytes returned an empty optional
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Chainhash data block 2";
            err.what = ex.what();
            return err;
        } catch (const std::invalid_argument& ex) {
            // the bytes have the wrong len
            err.errorCode = ERR_CHAINHASH_DATAPART_INVALID;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::length_error& ex) {
            // adding to much bytes
            err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::logic_error& ex) {
            // adding but its completed
            err.errorCode = ERR_CHAINHASH_DATABLOCK_ALREADY_COMPLETED;
            err.errorInfo = toHex(tmp);
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occured
            err.errorCode = ERR;
            err.errorInfo = "An error occured while reading the chainhash data block 2";
            err.what = ex.what();
            return err;
        }
    }
    // chainhash 2
    try {
        err.returnValue.setChainHash2(ChainHash{CHModes(ch2mode), ch2iters, chd2}, ch2datablocklen);
    } catch (const std::invalid_argument& ex) {
        // the chainhash is invalid
        err.errorCode = ERR_CHAINHASH2_INVALID;
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while setting the chainhash data block 2";
        err.what = ex.what();
        return err;
    }

    // password validator hash
    try {
        tmp = fileBytes.popFirstBytes(err.returnValue.hash_size).value();
        err.returnValue.setValidPasswordHashBytes(tmp);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password validator hash";
        err.what = ex.what();
        return err;
    } catch (const std::length_error& ex) {
        // password validator hash has the wrong len
        err.errorCode = ERR_LEN_INVALID;
        err.errorInfo = toHex(tmp);
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while setting and reading the password validator hash";
        err.what = ex.what();
        return err;
    }

    // encrypted salt
    try {
        tmp = fileBytes.popFirstBytes(err.returnValue.hash_size).value();
        err.returnValue.setEncSalt(tmp);
    } catch (const std::bad_optional_access& ex) {
        // popFirstBytes returned an empty optional
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Password salt hash";
        err.what = ex.what();
        return err;
    } catch (const std::length_error& ex) {
        // salt hash has the wrong len
        err.errorCode = ERR_LEN_INVALID;
        err.errorInfo = toHex(tmp);
        err.what = ex.what();
        return err;
    } catch (const std::exception& ex) {
        // some other error occured
        err.errorCode = ERR;
        err.errorInfo = "An error occured while setting and reading the salt hash";
        err.what = ex.what();
        return err;
    }
    err.success = SUCCESS;
    return err;
}

ErrorStruct<DataHeader> DataHeader::setHeaderParts(const DataHeaderParts dhp) noexcept {
    // creating a new header by setting the header parts
    ErrorStruct<DataHeader> err{FAIL, ERR, "", "", DataHeader{HModes(STANDARD_HASHMODE)}};
    try {
        // these methods throw exceptions if the data is invalid
        DataHeader dh{dhp.hash_mode};                                    // setting the hash mode
        dh.setFileDataMode(dhp.file_mode);                               // setting the file data mode
        dh.setChainHash1(dhp.chainhash1, dhp.chainhash1_datablock_len);  // setting the chainhash 1
        dh.setChainHash2(dhp.chainhash2, dhp.chainhash2_datablock_len);  // setting the chainhash 2
        dh.setValidPasswordHashBytes(dhp.valid_passwordhash);            // setting the password validator hash
        dh.setEncSalt(dhp.enc_salt);                                     // setting the encrypted salt

        // success
        err.returnValue = dh;
        err.success = SUCCESS;
        return err;

    } catch (const std::exception& ex) {
        // some error occured
        err.errorInfo = "An error occured while setting the header parts";
        err.what = ex.what();
        return err;
    }
}
