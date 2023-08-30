/*
contains the implementations of the ChainHashData class
*/
#include "chainhash_data.h"

#include <algorithm>

#include "logger.h"
#include "utility.h"

unsigned char ChainHashData::calculateDatablockSize(const Format& format) noexcept {
    // calculates the size of the datablock
    // the size is the sum of all parts length
    unsigned char size = 0;
    for (NameLen nl : format.getNameLenList()) {
        size += nl.len;
    }
    return size;
}

ChainHashData::ChainHashData(const Format& format)
    : format(format), datablock_size(calculateDatablockSize(format)), is_star(format.getNameLenList().empty() ? false : format.getNameLenList().back().len == 0) {
    // gets the name len data to name the data parts and know which length they are
    // calculates the final length of the datablock
    this->name_len_ind = 0;  // the index of the current part
}

bool ChainHashData::isComplete() const noexcept {
    // checks if all data is added by comparing the lengths of the two vectors
    // the name_len vector contains the expected data and data_pars contains the set data
    // no need to check the lengths of each pair, because if new data is added this should be checked there
    return (this->name_len_ind == this->format.getNameLenList().size());
}

bool ChainHashData::isCompletedFormat(const Format& format) const noexcept {
    // checks if a given format is set
    return (format == this->format && this->isComplete());
}

unsigned char ChainHashData::getPartsNumber() const noexcept {
    // returns the number of currently set data parts
    return this->name_len_ind;
}

Bytes ChainHashData::getDataBlock() const {
    // gets the datablock
    if (!this->isComplete()) {
        // throws if the block is not completed yet
        PLOG_ERROR << "trying to get datablock from a non complete ChainHashData object";
        throw std::logic_error("trying to get datablock from a non complete ChainHashData object.");
    }
    return this->datablock;
}

Bytes ChainHashData::getPart(const std::string& data_name) const {
    // gets the Bytes data that corresponds to a given data name provided in the format
    unsigned char start_ind = 0;  // the index of the first byte of the data part
    for (int i = 0; i < this->format.getNameLenList().size(); i++) {
        // loops through the name_lens list and checks if any name is equal to the given string
        // there has to be more than the index elements in data parts (in order to get the element)
        if (this->format.getNameLenList()[i].name == data_name && this->name_len_ind > i) {
            return this->datablock.copySubBytes(start_ind, this->name_len_ind > i + 1 ? start_ind + this->format.getNameLenList()[i].len : this->datablock.getLen());
        }
        start_ind += this->format.getNameLenList()[i].len;  // adds the length of the current part to the start index
    }
    PLOG_ERROR << "Could not find data name in data parts (name: " << data_name << ")";
    throw std::invalid_argument("Could not find data name in data parts");
}

size_t ChainHashData::getLen() const noexcept {
    // gets the length of the set data parts
    return this->datablock.getLen();
}

void ChainHashData::addBytes(const Bytes& bytes) {
    // adds a new part to the data parts vector
    if (this->isComplete()) {
        // cannot proceed, the datablock is complete. No new data can be added
        PLOG_ERROR << "Datablock is completed. No new data can be added";
        throw std::logic_error("Datablock is completed. No new data can be added");
    }
    // the number of current parts is also the index for the next part (indices start from 0)
    NameLen current_name_len = this->format.getNameLenList()[this->name_len_ind];  // gets the format information
    if (current_name_len.len != bytes.getLen() && current_name_len.len != 0) {
        // if current len is 0, all lengths are allowed
        // cannot add the given bytes. The length does not match with the set format
        PLOG_ERROR << "tried to add a data part with a not matching length (to the format) (len: " << bytes.getLen() << ", expected_len: " << current_name_len.len << ")";
        throw std::invalid_argument("tried to add a data part with a not matching length (to the format).");
    }
    try {
        bytes.addcopyToBytes(this->datablock);  // add the new byte part
    } catch (std::length_error) {
        // datablock is too long
        assert(this->getLen() + bytes.getLen() > 255);
        PLOG_ERROR << "the datablock exceeded the length limit after adding new bytes (len: " << this->getLen() << ")";
        throw std::length_error("you added some bytes to the datablock. Now the datablock exceeded the length limit");
    }
    assert(this->getLen() <= 255);
    this->name_len_ind++;  // increase the index for the next part
}

void ChainHashData::generateRandomData() {
    // fills the datablock with random data
    if (!this->datablock.isEmpty()) {
        // some parts have been set, therefore this function is not working
        PLOG_ERROR << "Cannot generate random data, because some data parts have been set.";
        throw new std::logic_error("Cannot generate random data, because some data parts have been set.");
    }
    if (is_star)
        this->datablock.fillrandom();  // fills the datablock with random data
    else
        this->datablock.addrandom(this->datablock_size);  // fills the datablock with random data

    this->name_len_ind = this->format.getNameLenList().size();  // set the index to the last part
}

bool operator==(const ChainHashData& chd1, const ChainHashData& chd2) {
    if (chd1.isComplete() && chd2.isComplete()) {
        // both objects are complete, compare the data parts
        return (chd1.getDataBlock() == chd2.getDataBlock());
    }
    return false;  // if one of the objects is not complete, they cannot be equal
}
