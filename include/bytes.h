#pragma once
#ifndef BYTES_H
#define BYTES_H

#include <iostream>
#include <optional>
#include <vector>

class Bytes {
    /*
    bytes datatype holds a byte vector
    defines useful functionalities on this vector
    */
   private:
    std::vector<unsigned char> bytes;  // bytes vector
   public:
    Bytes();                                                                           // creates a empty byte list
    Bytes(const int len);                                                              // creates a byte vector with a given length (it is filled with cryptographically random bytes)
    void print() const noexcept;                                                       // prints the hex string of this byte vector
    void setBytes(const std::vector<unsigned char> bytes) noexcept;                    // set the bytes to a given value
    std::vector<unsigned char> getBytes() const noexcept;                              // getter for the byte vector
    size_t getLen() const noexcept;                                                       // getter for the length in bytes
    void addByte(const unsigned char byte) noexcept;                                   // adds one byte at the end of the byte vector
    void addBytes(const Bytes b1) noexcept;                                            // adds a other byte object at the end of the byte vector
    std::optional<Bytes> popFirstBytes(const int num);                                 // calls getFirstBytes and removes them from the vector (if it returns a valid value)
    std::optional<Bytes> getFirstBytes(const int num) const;                           // gets the first num bytes of the vector (if there are not enough bytes we will get nothing)
    Bytes popFirstBytesFilledUp(const int num, const unsigned char fillup = 0);        // same as PopFirstBytes but if there are not enough bytes to get we will fill them up with the given value
    Bytes getFirstBytesFilledUp(const int num, const unsigned char fillup = 0) const;  // same as getFirstBytes but if there are not enough bytes to get we will fill them up with the given value
    bool isEmpty() const noexcept;                                                     // returns true if there are no bytes in the vector
    void clear() noexcept;                                                             // deletes all bytes from the vector
    friend bool operator==(const Bytes b1, const Bytes b2);                            // returns true if the byte vectors of the two Byte objects are equal
    friend Bytes operator+(const Bytes b1, const Bytes b2);                            // performs an add elementwise (the two byte vectors are added to each other (elementwise) mod 256)
    friend Bytes operator-(const Bytes b1, const Bytes b2);                            // performs an subtract elementwise (the second byte vector is subtracted from the first (elementwise) mod 256)
};

std::string toHex(const unsigned char byte) noexcept;  // returns a string (with two chars) that is the hexadecimal representation of the byte
std::string toHex(const Bytes b) noexcept;             // returns a string (with 2*len chars) that is the hexadecimal representation of the Bytes
u_int64_t toLong(const unsigned char byte) noexcept;   // returns a long that is the decimal representation of the byte
u_int64_t toLong(const Bytes b) noexcept;              // returns a long that is the decimal representation of the Bytes

#endif  // BYTES_H