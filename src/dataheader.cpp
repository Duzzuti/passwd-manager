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
    // generate the salt with random bytes
    Bytes rand_salt(this->hash_size);
    rand_salt.fillrandom();
    this->dh.setEncSalt(rand_salt);  // set the random generated encrypted salt
}

bool DataHeader::isComplete() const noexcept {
    // checks if the dataheader has everything set
    if (this->dh.isComplete(this->hash_size)) return true;
    return false;
}

unsigned int DataHeader::getHeaderLength() const noexcept {
    // gets the header len, if there is no header set try to calculate the len, else return 0
    if (this->dh.chainhash1.valid() && this->dh.chainhash2.valid())  // all data set to calculate the header length
        return 40 + 2 * this->hash_size + this->datablocks_len + this->dh.chainhash1.getChainHashData()->getLen() + this->dh.chainhash2.getChainHashData()->getLen();  // dataheader.md
    if (this->header_bytes.getLen() > 0) return this->header_bytes.getLen();  // header bytes are set, so we get this length
    return 0;                                                                 // not enough infos to get the header length
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
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
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
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
}

void DataHeader::setFileDataMode(const FModes file_mode) {
    // sets the file data mode
    PLOG_VERBOSE << "setting file mode: " << +file_mode;
    this->dh.setFileDataMode(file_mode);
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
}

void DataHeader::setValidPasswordHashBytes(const Bytes& validBytes) {
    // set the passwordhash validator hash
    PLOG_VERBOSE << "setting password validator hash: " << validBytes.toHex();
    this->dh.setValidPasswordHash(validBytes);
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
}

void DataHeader::clearDataBlocks() noexcept {
    // clears the data blocks
    this->datablocks_len = 0;
    this->dh.dec_data_blocks.clear();
    this->dh.enc_data_blocks.clear();
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
}

void DataHeader::addDataBlock(const DataBlock datablock) {
    // adds a data block
    if (this->dh.dec_data_blocks.size() >= 255) {
        // there are already 255 datablocks
        PLOG_ERROR << "there are already 255 datablocks";
        throw std::length_error("there are already 255 datablocks");
    }
    this->datablocks_len += 2 + datablock.getData().getLen();  // 1 for type, 1 for len, len for data
    this->dh.dec_data_blocks.push_back(datablock);
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
}

void DataHeader::addEncDataBlock(const EncDataBlock encdatablock) {
    // adds an encrypted data block
    if (this->dh.enc_data_blocks.size() >= 255) {
        // there are already 255 encrypted datablocks
        PLOG_ERROR << "there are already 255 encrypted datablocks";
        throw std::length_error("there are already 255 encrypted datablocks");
    }
    this->datablocks_len += 2 + encdatablock.getEnc().getLen();  // 1 for type, 1 for len, len for data
    this->dh.enc_data_blocks.push_back(encdatablock);
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
}

void DataHeader::setFileSize(const u_int64_t file_size) {
    // sets the file size
    PLOG_VERBOSE << "setting file size: " << file_size;
    if (!this->isComplete()) {
        // header bytes cannot be calculated (data is missing)
        PLOG_ERROR << "all dataheader parts have to be set to set the file size";
        throw std::logic_error("all dataheader parts have to be set to set the file size");
    }
    if (file_size < this->getHeaderLength()) {
        // file size is to small
        PLOG_ERROR << "file size is to small (file_size: " << file_size << ", header_length: " << this->getHeaderLength() << ")";
        throw std::invalid_argument("file size is to small");
    }
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
    this->file_size = file_size;
}

std::optional<u_int64_t> DataHeader::getFileSize() const noexcept { return this->file_size; }

void DataHeader::setEncSalt(const Bytes& salt) {
    // sets the salt
    PLOG_VERBOSE << "setting salt: " << salt.toHex();
    this->dh.setEncSalt(salt);
    this->header_bytes.setLen(0);  // clear header bytes because they have to be recalculated
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
    if (!this->file_size.has_value()) {
        // file size is not set
        PLOG_ERROR << "file size is not set";
        throw std::logic_error("file size is not set");
    }
    // file size is set
    if (this->file_size.value() < this->getHeaderLength()) {
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
        Bytes::fromLong(len, true).addcopyToBytes(dataheader);                      // add header size

        dataheader.addByte(this->dh.getFileDataMode());  // add file mode byte
        dataheader.addByte(this->dh.getHashMode());      // add hash mode byte

        dataheader.addByte((unsigned char)this->dh.dec_data_blocks.size());   // add data block count byte
        for (DataBlock& datablock : this->dh.dec_data_blocks) {               // add all data blocks
            dataheader.addByte(datablock.type);                               // add type byte
            dataheader.addByte((unsigned char)datablock.getData().getLen());  // add data length byte
            datablock.getData().addcopyToBytes(dataheader);                   // add data
        }
        dataheader.addByte(this->dh.chainhash1.getMode());                                  // add first chainhash mode byte
        Bytes::fromLong(this->dh.chainhash1.getIters(), true).addcopyToBytes(dataheader);   // add iterations for the first chainhash
        dataheader.addByte(this->dh.chainhash1.getChainHashData()->getLen());               // add datablock length byte
        this->dh.chainhash1.getChainHashData()->getDataBlock().addcopyToBytes(dataheader);  // add first datablock
        dataheader.addByte(this->dh.chainhash2.getMode());                                  // add second chainhash mode
        Bytes::fromLong(this->dh.chainhash2.getIters(), true).addcopyToBytes(dataheader);   // add iterations for the second chainhash
        dataheader.addByte(this->dh.chainhash2.getChainHashData()->getLen());               // add datablock length byte
        this->dh.chainhash2.getChainHashData()->getDataBlock().addcopyToBytes(dataheader);  // add second datablock
        this->dh.getValidPasswordHash().addcopyToBytes(dataheader);                         // add password validator
        this->dh.getEncSalt().addcopyToBytes(dataheader);                                   // add encrypted salt

        dataheader.addByte((unsigned char)this->dh.enc_data_blocks.size());     // add encrypted data block count byte
        for (EncDataBlock& encdatablock : this->dh.enc_data_blocks) {           // add all encrypted data blocks
            dataheader.addByte(encdatablock.getEncType());                      // add type byte
            dataheader.addByte((unsigned char)encdatablock.getEnc().getLen());  // add data length byte
            encdatablock.getEnc().addcopyToBytes(dataheader);                   // add data
        }
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
    u_int64_t file_size;
    u_int64_t header_size;
    unsigned char fmode;
    unsigned char hmode;
    unsigned char data_block_count;
    u_int16_t index = 0;  // index of the current byte

    // ********************* FILESIZE *********************
    try {
        try {
            // loading file size
            file_size = fileBytes.copySubBytes(index, index + 8).toLong();
        } catch (const std::length_error& ex) {
            // copySubBytes failed because the length is reached
            PLOG_ERROR << "not enogh data to read the file size (error msg: " << ex.what() << ")";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "File size";
            err.what = ex.what();
            return err;
        }
        index += 8;

        try {
            // loading file size
            header_size = fileBytes.copySubBytes(index, index + 8).toLong();
        } catch (const std::length_error& ex) {
            // copySubBytes failed because the length is reached
            PLOG_ERROR << "not enogh data to read the header size (error msg: " << ex.what() << ")";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Header size";
            err.what = ex.what();
            return err;
        }
        index += 8;

        if (header_size > fileBytes.getLen()) {
            // header size is bigger than the file size
            PLOG_ERROR << "header size is bigger than the given bytes (header_size: " << header_size << ", given bytes: " << fileBytes.getLen() << ")";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = " because the given header size is bigger than the given size";
            err.what = "header size is bigger than the given bytes";
            return err;
        }
        //********************* FILEMODE *********************
        // loading file mode
        fmode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        index += 1;

        // ********************* HASH FUNCTION *********************
        // loading and setting hash mode
        hmode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        index += 1;
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
            err.errorCode = ERR_FILEMODE_INVALID;
            err.errorInfo = fmode;
            err.what = ex.what();
            return err;
        }

        // ********************* DATABLOCKS *********************
        // loading data blocks
        data_block_count = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        index += 1;
        for (int i = 0; i < data_block_count; i++) {
            // data block type
            unsigned char type = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
            index += 1;
            // data block len
            unsigned char len = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
            index += 1;
            // data block data
            Bytes data = fileBytes.copySubBytes(index, index + len);
            index += len;
            dh->addDataBlock(DataBlock(DatablockType(type), data));
        }

        // ********************* CHAINHASH1 *********************
        std::shared_ptr<Format> format1 = nullptr;
        std::shared_ptr<ChainHashData> chd1 = nullptr;
        unsigned char ch1mode;
        try {
            ch1mode = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
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
            PLOG_ERROR << "An error occurred while reading the chainhash mode 1 (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while reading the chainhash mode 1";
            err.what = ex.what();
            return err;
        }
        index += 1;
        // chainhash 1 iters
        u_int64_t ch1iters = fileBytes.copySubBytes(index, index + 8).toLong();
        index += 8;
        // chainhash 1 data block len
        unsigned char ch1datablocklen = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        index += 1;
        // chainhash 1 data block
        int data_len = 0;
        Bytes tmp{255};
        for (NameLen nl : format1->getNameLenList()) {
            try {
                if (nl.len != 0) {
                    // got a data part with set length
                    tmp = fileBytes.copySubBytes(index, index + nl.len);
                    chd1->addBytes(tmp);
                    index += nl.len;
                    data_len += nl.len;
                } else {
                    // got a data parCHModest with * length (use the remaining bytes)
                    tmp = fileBytes.copySubBytes(index, index + (ch1datablocklen - data_len));
                    chd1->addBytes(tmp);
                    data_len = ch1datablocklen;
                    index += (ch1datablocklen - data_len);
                    break;
                }

            } catch (const std::length_error& ex) {
                // adding to much bytes
                PLOG_ERROR << "adding to much bytes to the chainhash data block 1 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
                err.errorInfo = tmp.toHex();
                err.what = ex.what();
                return err;
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
            err.errorCode = ERR_CHAINHASH1_INVALID;
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while setting the chainhash 1 (error msg: " << ex.what() << ")";
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
        u_int64_t ch2iters = fileBytes.copySubBytes(index, index + 8).toLong();
        index += 8;
        // chainhash 2 data block len
        unsigned char ch2datablocklen = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        index += 1;
        // chainhash 2 data block
        data_len = 0;
        for (NameLen nl : format2->getNameLenList()) {
            try {
                if (nl.len != 0) {
                    // got a data part with set length
                    tmp = fileBytes.copySubBytes(index, index + nl.len);
                    chd2->addBytes(tmp);
                    index += nl.len;
                    data_len += nl.len;
                } else {
                    // got a data parCHModest with * length (use the remaining bytes)
                    tmp = fileBytes.copySubBytes(index, index + (ch2datablocklen - data_len));
                    chd2->addBytes(tmp);
                    data_len = ch2datablocklen;
                    index += (ch2datablocklen - data_len);
                    break;
                }

            } catch (const std::length_error& ex) {
                // adding to much bytes
                PLOG_ERROR << "adding to much bytes to the chainhash data block 2 (error msg: " << ex.what() << ")";
                err.errorCode = ERR_CHAINHASH_DATABLOCK_OUTOFRANGE;
                err.errorInfo = tmp.toHex();
                err.what = ex.what();
                return err;
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
            err.errorCode = ERR_CHAINHASH2_INVALID;
            err.what = ex.what();
            return err;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while setting the chainhash 2 (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while setting the chainhash data block 2";
            err.what = ex.what();
            return err;
        }

        // password validator hash
        try {
            dh->setValidPasswordHashBytes(fileBytes.copySubBytes(index, index + dh->hash_size));
            index += dh->hash_size;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while setting and reading the password validator hash (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while setting and reading the password validator hash";
            err.what = ex.what();
            return err;
        }

        // encrypted salt
        try {
            dh->setEncSalt(fileBytes.copySubBytes(index, index + dh->hash_size));
            index += dh->hash_size;
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while setting and reading the salt hash (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while setting and reading the salt hash";
            err.what = ex.what();
            return err;
        }

        // ********************* ENCRYPTED DATABLOCKS *********************
        // loading encrypted data blocks
        unsigned char enc_data_block_count = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
        index += 1;
        for (int i = 0; i < enc_data_block_count; i++) {
            // data block type
            unsigned char enc_type = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
            index += 1;
            // data block len
            unsigned char len = fileBytes.copySubBytes(index, index + 1).getBytes()[0];
            index += 1;
            // data block data
            Bytes enc_data = fileBytes.copySubBytes(index, index + len);
            index += len;
            dh->addEncDataBlock(EncDataBlock::createEncBlock(enc_type, enc_data));
        }
        try {
            dh->setFileSize(file_size);  // setting the file size
        } catch (const std::invalid_argument& ex) {
            PLOG_ERROR << "invalid file size (error msg: " << ex.what() << ")";
            err.errorCode = ERR_FILESIZE_INVALID;
            err.errorInfo = file_size;
            err.what = ex.what();
            return err;
        }
        try{
            dh->calcHeaderBytes();
        } catch (const std::exception& ex) {
            // some other error occurred
            PLOG_ERROR << "An error occurred while calculating the header bytes (error msg: " << ex.what() << ")";
            err.errorCode = ERR;
            err.errorInfo = "An error occurred while calculating the header bytes";
            err.what = ex.what();
            return err;
        }
        assert(index == header_size);
        assert(dh->getHeaderLength() == header_size);
        return ErrorStruct<std::unique_ptr<DataHeader>>::createMove(std::move(dh));
    } catch (const std::exception& ex) {
        PLOG_ERROR << "An error occurred while reading the header (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while reading the header";
        err.what = ex.what();
        return err;
    }
}

ErrorStruct<std::unique_ptr<DataHeader>> DataHeader::setHeaderBytes(std::ifstream& file) noexcept {
    // sets the header bytes by taking the first bytes of the file
    // init error struct
    ErrorStruct<std::unique_ptr<DataHeader>> err{FAIL, ERR, "An error occurred while reading the header", "setHeaderBytes"};
    std::unique_ptr<DataHeader> dh = nullptr;
    // setting the header parts
    u_int64_t file_size;
    u_int64_t header_size;
    Bytes tmp(8);
    unsigned char fmode;
    unsigned char hmode;
    // get stream length
    std::streampos start = file.tellg();
    file.seekg(0, std::ios::end);
    std::streampos end = file.tellg();
    file.seekg(start);
    //********************* FILESIZE *********************
    if (!readData(file, tmp, 8)) {
        // readsome could not read the file size
        PLOG_ERROR << "not enogh data to read the file size";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "File size";
        return err;
    }
    file_size = tmp.toLong();
    tmp.setLen(0);  // clear tmp
    //********************* HEADERSIZE *********************
    if (!readData(file, tmp, 8)) {
        // readsome could not read the header size
        PLOG_ERROR << "not enogh data to read the header size";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Header size";
        return err;
    }
    header_size = tmp.toLong();
    tmp.setLen(0);  // clear tmp
    if (end - start != file_size) {
        // file size is not equal to the stream size
        PLOG_ERROR << "file size is not equal to the given stream size (file_size: " << file_size << ", stream size: " << (end - start) << ")";
        err.errorCode = ERR_FILESIZE_INVALID;
        err.errorInfo = "file size is not equal to the given stream size (file_size: " + std::to_string(file_size) + ", stream size: " + std::to_string(end - start) + ")";
        return err;
    }
    if (header_size > file_size) {
        // header size is bigger than the file size
        PLOG_ERROR << "header size is bigger than the file size (header_size: " << header_size << ", file size: " << file_size << ")";
        err.errorCode = ERR_HEADERSIZE_FILESIZE_MISMATCH;
        err.errorInfo = "header size is bigger than the file size (header_size: " + std::to_string(header_size) + ", file size: " + std::to_string(file_size) + ")";
        return err;
    }
    //********************* FILEMODE *********************
    // loading file mode
    if (!readData(file, &fmode, 1)) {
        // readsome could not read the file mode
        PLOG_ERROR << "not enogh data to read the file mode";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "File mode";
        return err;
    }

    // ********************* HASH FUNCTION *********************
    // loading and setting hash mode
    if (!readData(file, &hmode, 1)) {
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

    // ********************* DATABLOCKS *********************
    // loading data blocks
    unsigned char data_block_count;
    if (!readData(file, &data_block_count, 1)) {
        // readsome could not read the data block count
        PLOG_ERROR << "not enogh data to read the data block count";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Data block count";
        return err;
    }
    for (int i = 0; i < data_block_count; i++) {
        // data block type
        unsigned char type;
        if (!readData(file, &type, 1)) {
            // readsome could not read the data block type
            PLOG_ERROR << "not enogh data to read the data block type";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Data block type";
            return err;
        }
        // data block len
        unsigned char len;
        if (!readData(file, &len, 1)) {
            // readsome could not read the data block len
            PLOG_ERROR << "not enogh data to read the data block len";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Data block len";
            return err;
        }
        // data block data
        Bytes data(len);
        if (!readData(file, data, len)) {
            // readsome could not read the data block data
            PLOG_ERROR << "not enogh data to read the data block data";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Data block data";
            return err;
        }
        dh->addDataBlock(DataBlock(DatablockType(type), data));
    }

    // ********************* CHAINHASH1 *********************
    std::shared_ptr<Format> format1 = nullptr;
    std::shared_ptr<ChainHashData> chd1 = nullptr;
    unsigned char ch1mode;
    // loading and setting chainhash1 mode
    if (!readData(file, &ch1mode, 1)) {
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
    // loading and chainhash1 iters
    if (!readData(file, tmp, 8)) {
        // readsome could not read the chainhash1 iters
        PLOG_ERROR << "not enogh data to read the chainhash1 iters";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash1 iters";
        return err;
    }
    ch1iters = tmp.toLong();
    // chainhash 1 data block len
    unsigned char ch1datablocklen;
    // loading and chainhash1 blocklen
    if (!readData(file, &ch1datablocklen, 1)) {
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
                tmp.setLen(0); // clear tmp
                if (!readData(file, tmp, nl.len)) {
                    throw std::length_error("not enough data to read the chainhash data block 1");
                }
                copied = true;
                chd1->addBytes(tmp);
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                tmp.setLen(0); // clear tmp
                if (!readData(file, tmp, ch1datablocklen - data_len)) {
                    throw std::length_error("not enough data to read the chainhash data block 1");
                }
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
    if (!readData(file, &ch2mode, 1)) {
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
    if (!readData(file, tmp, 8)) {
        // readsome could not read the chainhash2 iters
        PLOG_ERROR << "not enogh data to read the chainhash2 iters";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash2 iters";
        return err;
    }
    ch2iters = tmp.toLong();
    // chainhash 2 data block len
    unsigned char ch2datablocklen;
    // loading and chainhash1 blocklen
    if (!readData(file, &ch2datablocklen, 1)) {
        // readsome could not read the chainhash2 blocklen
        PLOG_ERROR << "not enogh data to read the chainhash2 blocklen";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Chainhash2 blocklen";
        return err;
    }
    // chainhash 2 data block
    data_len = 0;
    copied = false;
    tmp = Bytes(255);
    for (NameLen nl : format2->getNameLenList()) {
        try {
            bool copied = false;
            if (nl.len != 0) {
                // got a data part with set length
                tmp.setLen(0); // clear tmp
                if (!readData(file, tmp, nl.len)) {
                    throw std::length_error("not enough data to read the chainhash data block 2");
                }
                copied = true;
                chd2->addBytes(tmp);
                data_len += nl.len;
            } else {
                // got a data parCHModest with * length (use the remaining bytes)
                tmp.setLen(0); // clear tmp
                if (!readData(file, tmp, ch2datablocklen - data_len)) {
                    throw std::length_error("not enough data to read the chainhash data block 2");
                }
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

    //********************* PASSWORD VALIDATOR HASH *********************
    try {
        Bytes tmp(dh->hash_size);
        if (!readData(file, tmp, dh->hash_size)) {
            // readsome could not read the password validator hash
            PLOG_ERROR << "not enogh data to read the password validator hash";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Password validator hash";
            return err;
        }
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
        Bytes tmp(dh->hash_size);
        if (!readData(file, tmp, dh->hash_size)) {
            // readsome could not read the encrypted salt
            PLOG_ERROR << "not enogh data to read the encrypted salt";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Encrypted salt";
            return err;
        }
        dh->setEncSalt(tmp);
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while setting and reading the salt hash (error msg: " << ex.what() << ")";
        err.success = FAIL;
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while setting and reading the salt hash";
        err.what = ex.what();
        return err;
    }
    try {
        dh->setFileSize(file_size);  // setting the file size
    } catch (const std::invalid_argument& ex) {
        PLOG_ERROR << "invalid file size (error msg: " << ex.what() << ")";
        err.errorCode = ERR_FILESIZE_INVALID;
        err.errorInfo = file_size;
        err.what = ex.what();
        return err;
    }

    // ********************* ENCRYPTED DATABLOCKS *********************
    // loading encrypted data blocks
    unsigned char enc_data_block_count;
    if (!readData(file, &enc_data_block_count, 1)) {
        // readsome could not read the encrypted block count
        PLOG_ERROR << "not enogh data to read the encrypted block count";
        err.errorCode = ERR_NOT_ENOUGH_DATA;
        err.errorInfo = "Encrypted block count";
        return err;
    }

    for (int i = 0; i < enc_data_block_count; i++) {
        // enc data block type
        unsigned char type;
        if (!readData(file, &type, 1)) {
            // readsome could not read the enc data block type
            PLOG_ERROR << "not enogh data to read the enc data block type";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Enc datablock type";
            return err;
        }
        // data block len
        unsigned char len;
        if (!readData(file, &len, 1)) {
            // readsome could not read the enc data block len
            PLOG_ERROR << "not enogh data to read the enc data block len";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Enc datablock len";
            return err;
        }
        // data block data
        Bytes data(len);
        if (!readData(file, data, len)) {
            // readsome could not read the enc data block data
            PLOG_ERROR << "not enogh data to read the enc data block data";
            err.errorCode = ERR_NOT_ENOUGH_DATA;
            err.errorInfo = "Enc datablock data";
            return err;
        }
        dh->addEncDataBlock(EncDataBlock::createEncBlock(type, data));
    }
    try{
        dh->calcHeaderBytes();
    } catch (const std::exception& ex) {
        // some other error occurred
        PLOG_ERROR << "An error occurred while calculating the header bytes (error msg: " << ex.what() << ")";
        err.errorCode = ERR;
        err.errorInfo = "An error occurred while calculating the header bytes";
        err.what = ex.what();
        return err;
    }
    assert(file.tellg() - start == dh->getHeaderLength());
    assert(dh->getHeaderLength() == header_size);
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
        dh->setEncSalt(dhp.getEncSalt());                                                  // setting the encrypted salt
        for (const DataBlock& db : dhp.dec_data_blocks) {                                  // setting the data blocks
            dh->addDataBlock(db);
        }
        for (const EncDataBlock& edb : dhp.enc_data_blocks) {  // setting the encrypted data blocks
            dh->addEncDataBlock(edb);
        }

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
