#pragma once

#include <iostream>

class BytesOpt {
    /*
    bytes datatype that is the optimized version of Bytes
    it has a fixed size and is deployed on the heap
    it is a lot faster than Bytes but it does not support all functionalities
    */
   private:
    unsigned char* bytes;  // bytes array (deployed on the heap)
    size_t max_len;        // max length of the byte array
    size_t len;            // length of the byte array

   public:
    BytesOpt(const int max_len);                                         // creates an empty byte array with a given maximum length
    void fillrandom() noexcept;                                          // fills the byte array with random bytes
    void addrandom(const int num);                                       // adds random bytes to the byte array (num is the number of bytes that will be added)
    void consumeBytes(const unsigned char* bytes, const size_t len);     // set the bytes to a given value
    void addconsumeBytes(const unsigned char* bytes, const size_t len);  // adds the bytes to the current value
    unsigned char* getBytes() const noexcept;                            // getter for the byte array (by reference)
    void copyToArray(unsigned char* array, const size_t len) const;      // copys the bytes to the given array
    size_t getLen() const noexcept;                                      // getter for the length in bytes
    size_t getMaxLen() const noexcept;                                   // getter for the maximum length in bytes
    void addByte(const unsigned char& byte);                             // adds one byte at the end of the byte array by reference
    bool isEmpty() const noexcept;                                       // returns true if there are no bytes in the array
    u_int64_t toLong() const noexcept;                                   // returns a long that is the decimal representation of the Bytes
    std::string toHex() const noexcept;                                  // returns a string (with 2*len chars) that is the hexadecimal representation of the Bytes
    bool operator==(const BytesOpt& b2);                                 // returns true if the byte arrays of the two Byte objects are equal
    BytesOpt operator+(const BytesOpt& b2);                              // performs an add elementwise (the two byte arrays are added to each other (elementwise) mod 256)
    BytesOpt operator-(const BytesOpt& b2);                              // performs an subtract elementwise (the second byte array is subtracted from the first (elementwise) mod 256)
    ~BytesOpt() {
        if (this->bytes == nullptr) delete[] this->bytes;
    };  // destructor
};
