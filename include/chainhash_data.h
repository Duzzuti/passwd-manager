#pragma once

#include "bytes.h"
#include "format.h"

class ChainHashData {
    /*
    this class provides functionality for the datablock of the chainhash data
    it is initialized with a format and gets all data needed for that format
    a format declares parts of the datablock, these data parts have a name and a len (NameLen)
    */
   private:
    Format format;                   // the format of the datablock
    std::vector<Bytes> data_parts;   // a list of all datablock parts as Bytes
    std::vector<NameLen> name_lens;  // a list of all expected datablock parts as NameLen
   private:
    void clear() noexcept;  // clears the datablock
   public:
    ChainHashData(Format format);  // constructs the data object, takes a format for this datablock

    // checks if all expected data is already goten
    bool isComplete() const noexcept;
    // checks if the datablock is complete and it has the given format
    bool isCompletedFormat(Format format) const noexcept;

    // gets the number of gotten parts
    unsigned char getPartsNumber() const noexcept;
    // gets one Bytes object that is a concatination of all byte parts, throws if not complete
    Bytes getDataBlock() const;
    // gets the bytes for a given part name, throws if the part name is not found (or set)
    Bytes getPart(std::string data_name) const;

    size_t getLen() const noexcept;  // gets the length of the datablock (the sum of all parts length)

    // add bytes to the datablock; it is assumed that these bytes corresponds to the next part
    // the bytes have to be provided in the format string order
    // checks the length of the given bytes;
    // throws if the given bytes have a invalid length for the next part or if the datablock is already complete
    void addBytes(Bytes bytes);

    // fills the datablock with random data
    void generateRandomData();

    // if we compare two ChainHashData objects we compare their data_parts and header bytes
    // both have to be complete, otherwise there are not equal
    friend bool operator==(const ChainHashData& chd1, const ChainHashData& chd2);
};
