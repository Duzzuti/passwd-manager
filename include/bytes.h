#pragma once

#include <fstream>
#include <iostream>
#include <memory>

class Bytes {
    /*
    bytes datatype that is the optimized version of Bytes
    it has a fixed size and is deployed on the heap
    it is a lot faster than Bytes but it does not support all functionalities
    */
   private:
    unsigned char* bytes;    // bytes array (deployed on the heap)
    size_t max_len;          // max length of the byte array
    size_t len;              // length of the byte array
    bool deallocate = true;  // if the bytes array should be deallocated when the object is destroyed
    Bytes() {
        this->bytes = nullptr;
        this->max_len = 0;
        this->len = 0;
    }

   public:
    static Bytes fromLong(const u_int64_t l, const bool addzeros = false);  // sets the Bytes to the decimal representation of the given long
    static Bytes withU64(const u_int64_t max_len) noexcept;                 // creates an empty byte array with a given maximum length

    Bytes(const int64_t max_len);                                    // creates an empty byte array with a given maximum length
    Bytes(unsigned char* bytes, const size_t len);                   // creates a Bytes object with the given bytes and length (consumes the array)
    Bytes(const Bytes& other, const size_t extra_len);               // creates a new Bytes object with the bytes of the other Bytes object and adds extra_len bytes at the end
    Bytes(const Bytes& other);                                       // copy constructor
    Bytes(Bytes&& other) noexcept;                                   // move constructor
    Bytes& operator=(const Bytes& other);                            // copy assignment
    void setDeallocate(const bool deallocate) noexcept;              // setter for the deallocate variable
    void fillrandom() noexcept;                                      // fills the byte array with random bytes
    void addrandom(const int64_t num);                               // adds random bytes to the byte array (num is the number of bytes that will be added)
    void setBytes(const unsigned char* bytes, const size_t len);     // set the bytes to a given value
    void addBytes(const unsigned char* bytes, const size_t len);     // adds the bytes to the current value
    void consumeBytes(unsigned char* bytes, const size_t len);       // consumes the bytes from the current value
    void consumeBytes(Bytes&& b);                                    // consumes the bytes from the current value (from another Bytes object)
    unsigned char* getBytes() const noexcept;                        // getter for the byte array (by reference)
    void setLen(const size_t len);                                   // setter for the length in bytes
    void copyToArray(unsigned char* array, const size_t len) const;  // copys the bytes to the given array
    void copyToBytes(Bytes& b) const;                                // copys the bytes to the given Bytes object
    void addcopyToBytes(Bytes& b) const;                             // adds the bytes to the given Bytes object
    void addcopyToBytes(std::unique_ptr<Bytes>& b) const;            // adds the bytes to the given Bytes object
    Bytes copySubBytes(const size_t start, const size_t end) const;  // returns a Bytes object that is a copy of the bytes from start to end
    size_t getLen() const noexcept;                                  // getter for the length in bytes
    size_t getMaxLen() const noexcept;                               // getter for the maximum length in bytes
    void addSize(const size_t size) noexcept;                        // increase the max length of the bytes object by size
    void addByte(const unsigned char byte);                          // adds one byte at the end of the byte array by reference
    bool isEmpty() const noexcept;                                   // returns true if there are no bytes in the array
    u_int64_t toLong() const;                                        // returns a long that is the decimal representation of the Bytes
    std::string toHex() const noexcept;                              // returns a string (with 2*len chars) that is the hexadecimal representation of the Bytes
    bool operator==(const Bytes& b2) const noexcept;                 // returns true if the byte arrays of the two Byte objects are equal
    bool operator!=(const Bytes& b2) const noexcept;                 // returns true if the byte arrays of the two Byte objects are not equal
    Bytes operator+(const Bytes& b2) const;                          // performs an add elementwise (the two byte arrays are added to each other (elementwise) mod 256)
    Bytes operator-(const Bytes& b2) const;                          // performs an subtract elementwise (the second byte array is subtracted from the first (elementwise) mod 256)
    ~Bytes() {
        if (this->bytes != nullptr && this->deallocate) {
            delete[] this->bytes;
            this->bytes = nullptr;
        }
    };  // destructor

    friend std::ofstream& operator<<(std::ofstream& os, const Bytes& bytes);  // writes the bytes to the given ofstream
};
