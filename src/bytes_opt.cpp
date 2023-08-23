#include <sstream>

#include "bytes_opt.h"
#include "logger.h"
#include "rng.h"

BytesOpt::BytesOpt(const int max_len) {
    // creates an empty byte array with a given maximum length
    this->max_len = max_len;
    this->len = 0;
    this->bytes = new unsigned char[max_len];
}

void BytesOpt::fillrandom() noexcept {
    // fills the byte array with random bytes
    RNG::fill_random_bytes(this->bytes + this->len, this->max_len - this->len);
    this->len = this->max_len;
}

void BytesOpt::addrandom(const int num) {
    // adds random bytes to the byte array (num is the number of bytes that will be added)
    if (this->len + num > this->max_len) {
        PLOG_FATAL << "given bytes are too long (given: " << num << ", max: " << this->max_len << ")";
        throw std::invalid_argument("given bytes are too long");
    }
    RNG::fill_random_bytes(this->bytes + this->len, num);
    this->len += num;
}

void BytesOpt::consumeBytes(const unsigned char* bytes, const size_t len) {
    // set the bytes to a given value
    if (len > this->max_len) {
        PLOG_FATAL << "given bytes are too long (given: " << len << ", max: " << this->max_len << ")";
        throw std::invalid_argument("given bytes are too long");
    }
    this->len = len;
    std::memcpy(this->bytes, bytes, len);   // copy the bytes to the byte array
}

void BytesOpt::addconsumeBytes(const unsigned char* bytes, const size_t len) {
    // adds the bytes to the current value
    if (this->len + len > this->max_len) {
        PLOG_FATAL << "given bytes are too long (given: " << len << ", max: " << this->max_len << ")";
        throw std::invalid_argument("given bytes are too long");
    }
    std::memcpy(this->bytes + this->len, bytes, len);   // copy the bytes to the byte array
    this->len += len;
}

unsigned char* BytesOpt::getBytes() const noexcept { 
    // getter for the byte array (by reference)
    return this->bytes; 
}

void BytesOpt::copyToArray(unsigned char* array, const size_t len) const {
    // copys the bytes to the given array
    if (len < this->len) {
        PLOG_FATAL << "given array is too short (given: " << len << ", needed: " << this->len << ")";
        throw std::invalid_argument("given array is too short");
    }
    std::memcpy(array, this->bytes, this->len);
}

size_t BytesOpt::getLen() const noexcept { 
    // getter for the length in bytes
    return this->len; 
}

size_t BytesOpt::getMaxLen() const noexcept { 
    // getter for the maximum length in bytes
    return this->max_len; 
}

void BytesOpt::addByte(const unsigned char& byte) {
    // adds one byte at the end of the byte array by reference
    if (this->len + 1 > this->max_len) {
        PLOG_FATAL << "given bytes are too long (given: " << 1 << ", max: " << this->max_len << ")";
        throw std::invalid_argument("given bytes are too long");
    }
    this->bytes[this->len] = byte;
    this->len++;
}

bool BytesOpt::isEmpty() const noexcept { 
    // returns true if there are no bytes in the array
    return this->len == 0; 
}

bool BytesOpt::operator==(const BytesOpt& b2) { 
    // returns true if the byte arrays of the two Byte objects are equal
    if (this->len != b2.len) return false;
    return std::memcmp(this->bytes, b2.bytes, this->len) == 0;
}

BytesOpt BytesOpt::operator+(const BytesOpt& b2) { 
    // performs an add elementwise (the two byte arrays are added to each other (elementwise) mod 256)
    if (this->len != b2.len) {
        PLOG_FATAL << "cannot add bytes with different lengths (len1: " << this->len << ", len2: " << b2.len << ")";
        throw std::invalid_argument("cannot add bytes with different lengths");
    }
    BytesOpt res(this->max_len);
    for (size_t i = 0; i < this->len; i++) {
        res.bytes[i] = this->bytes[i] + b2.bytes[i];
    }
    res.len = this->len;
    return res;
}

BytesOpt BytesOpt::operator-(const BytesOpt& b2) { 
    // performs an subtract elementwise (the second byte array is subtracted from the first (elementwise) mod 256)
    if (this->len != b2.len) {
        PLOG_FATAL << "cannot subtract bytes with different lengths (len1: " << this->len << ", len2: " << b2.len << ")";
        throw std::invalid_argument("cannot subtract bytes with different lengths");
    }
    BytesOpt res(this->max_len);
    for (size_t i = 0; i < this->len; i++) {
        res.bytes[i] = this->bytes[i] - b2.bytes[i];
    }
    res.len = this->len;
    return res;
}

std::string BytesOpt::toHex() const noexcept { 
    // returns a string (with 2*len chars) that is the hexadecimal representation of the Bytes
    constexpr char hexChars[] = "0123456789ABCDEF"; // Array to map values to hex characters
    std::string hexString;
    hexString.reserve(this->len * 2); // Reserve space for the final hex string

    for (size_t i = 0; i < this->len; ++i) {
        hexString.push_back(hexChars[this->bytes[i] >> 4]); // High nibble
        hexString.push_back(hexChars[this->bytes[i] & 0xF]); // Low nibble
    }

    return hexString;
}

u_int64_t BytesOpt::toLong() const noexcept { 
    // returns a long that is the decimal representation of the Bytes
    u_int64_t res = 0;
    for (size_t i = 0; i < this->len; i++) {
        res += this->bytes[i] << (8 * i);
    }
    return res;
}
