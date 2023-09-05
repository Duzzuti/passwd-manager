#include "bytes.h"

#include <sstream>

#include "logger.h"
#include "rng.h"

Bytes Bytes::fromLong(const u_int64_t l, const bool addzeros) {
    // sets the Bytes to the decimal representation of the given long
    Bytes res(8);
    unsigned char reversedBytes[8];
    std::memcpy(reversedBytes, &l, 8);
    for (size_t i = 0; i < 8; ++i) {
        if (!res.isEmpty() || reversedBytes[7 - i] != 0) {
            res.addByte(reversedBytes[7 - i]);
        } else if (addzeros) {
            res.addByte(0);
        }
    }
    return res;
}

Bytes::Bytes(const int64_t max_len) {
    // creates an empty byte array with a given maximum length
    if (max_len < 0) {
        // invalid length given
        PLOG_FATAL << "The provided max_len is negative (max_len: " << max_len << ")";
        throw std::invalid_argument("The provided max_len is negative");
    }
    this->max_len = max_len;
    this->len = 0;
    this->bytes = new unsigned char[max_len];
}

Bytes::Bytes(unsigned char* bytes, const size_t len) {
    // creates a Bytes object with the given bytes and length (consumes the array)
    if (bytes == nullptr) {
        PLOG_FATAL << "cannot consume nullptr";
        throw std::invalid_argument("cannot consume nullptr");
    }
    this->max_len = len;
    this->len = len;
    this->bytes = bytes;  // set the byte array to the given bytes
}

Bytes::Bytes(const Bytes& other, const size_t extra_len) {
    if (this == &other) {
        // self assignment
        return;
    }
    this->max_len = other.max_len + extra_len;
    this->bytes = new unsigned char[this->max_len];
    if (other.bytes == nullptr) {
        PLOG_FATAL << "cannot copy nullptr";
        throw std::invalid_argument("cannot copy nullptr");
    }
    std::memcpy(this->bytes, other.bytes, other.len);
    this->len = other.len;
}

// Copy constructor
Bytes::Bytes(const Bytes& other) {
    if (this == &other) {
        // self assignment
        return;
    }
    this->max_len = other.max_len;
    this->bytes = new unsigned char[max_len];
    if (other.bytes == nullptr) {
        PLOG_FATAL << "cannot copy nullptr";
        throw std::invalid_argument("cannot copy nullptr");
    }
    std::memcpy(this->bytes, other.bytes, other.len);
    this->len = other.len;
}

Bytes& Bytes::operator=(const Bytes& other) {
    if (this == &other) {
        // self assignment
        return *this;
    }
    if (other.bytes == nullptr) {
        PLOG_FATAL << "cannot copy nullptr";
        throw std::invalid_argument("cannot copy nullptr");
    }
    // if (max_len < other.len) {
    //     PLOG_FATAL << "cannot copy to a Bytes object with max_len " << max_len << " from a Bytes object with len " << other.len;
    //     throw std::length_error("cannot copy to a Bytes object with max_len " + std::to_string(max_len) + " from a Bytes object with len " + std::to_string(other.len));
    // }
    this->max_len = other.max_len;
    delete[] this->bytes;
    this->bytes = new unsigned char[max_len];
    std::memcpy(this->bytes, other.bytes, other.len);
    this->len = other.len;
    return *this;
}

void Bytes::setDeallocate(const bool deallocate) noexcept {
    // setter for the deallocate variable
    this->deallocate = deallocate;
}

void Bytes::fillrandom() noexcept {
    // fills the byte array with random bytes
    RNG::fill_random_bytes(this->bytes + this->len, this->max_len - this->len);
    this->len = this->max_len;
}

void Bytes::addrandom(const int64_t num) {
    // adds random bytes to the byte array (num is the number of bytes that will be added)
    if (this->len + num > this->max_len) {
        PLOG_FATAL << "cannot add " << num << " random bytes, because there is only space for " << this->max_len - this->len;
        throw std::length_error("cannot add " + std::to_string(num) + " random bytes, because there is only space for " + std::to_string(this->max_len - this->len));
    }
    RNG::fill_random_bytes(this->bytes + this->len, num);
    this->len += num;
}

void Bytes::setBytes(const unsigned char* bytes, const size_t len) {
    // set the bytes to a given value
    if (len > this->max_len) {
        PLOG_FATAL << "cannot set bytes with len " << len << " in a Bytes object with max len " << this->max_len;
        throw std::length_error("cannot consume bytes with len " + std::to_string(len) + " in a Bytes object with max len " + std::to_string(this->max_len));
    }
    if (bytes == nullptr) {
        PLOG_FATAL << "cannot set nullptr";
        throw std::invalid_argument("cannot set nullptr");
    }
    this->len = len;
    std::memcpy(this->bytes, bytes, len);  // copy the bytes to the byte array
}

void Bytes::addBytes(const unsigned char* bytes, const size_t len) {
    // adds the bytes to the current value
    if (this->len + len > this->max_len) {
        PLOG_FATAL << "cannot add bytes with len " << len << " in a Bytes object with free space " << this->max_len - this->len;
        throw std::length_error("cannot add bytes with len " + std::to_string(len) + " in a Bytes object with free space " + std::to_string(this->max_len - this->len));
    }
    if (bytes == nullptr) {
        PLOG_FATAL << "cannot add nullptr";
        throw std::invalid_argument("cannot add nullptr");
    }
    std::memcpy(this->bytes + this->len, bytes, len);  // copy the bytes to the byte array
    this->len += len;
}

void Bytes::consumeBytes(unsigned char* bytes, const size_t len) {
    // consumes the bytes from the current value
    if (len > this->max_len) {
        PLOG_FATAL << "cannot consume bytes with len " << len << " in a Bytes object with max len " << this->max_len;
        throw std::length_error("cannot consume bytes with len " + std::to_string(len) + " in a Bytes object with max len " + std::to_string(this->max_len));
    }
    if (bytes == nullptr) {
        PLOG_FATAL << "cannot consume nullptr";
        throw std::invalid_argument("cannot consume nullptr");
    }
    this->len = len;
    delete[] this->bytes;
    this->bytes = bytes;  // set the byte array to the given bytes
}

void Bytes::consumeBytes(Bytes&& b) {
    // consumes the bytes from the bytes object
    if (b.getLen() > this->max_len) {
        PLOG_FATAL << "cannot consume bytes object with len " << b.getLen() << " in a Bytes object with max len " << this->max_len;
        throw std::length_error("cannot consume bytes object with len " + std::to_string(b.getLen()) + " in a Bytes object with max len " + std::to_string(this->max_len));
    }
    this->len = b.getLen();
    delete[] this->bytes;
    this->bytes = b.getBytes();  // set the byte array to the given bytes
    b.setDeallocate(false);      // the bytes object should not deallocate the bytes array (because it is now owned by this object)
}

unsigned char* Bytes::getBytes() const noexcept {
    // getter for the byte array (by reference)
    return this->bytes;
}

void Bytes::setLen(const size_t len) {
    // setter for the length in bytes
    if (len > this->max_len) {
        PLOG_FATAL << "cannot set len to " << len << " in a Bytes object with max len " << this->max_len;
        throw std::length_error("cannot set len to " + std::to_string(len) + " in a Bytes object with max len " + std::to_string(this->max_len));
    }
    this->len = len;
}

void Bytes::copyToArray(unsigned char* array, const size_t len) const {
    // copys the bytes to the given array
    if (len < this->len) {
        PLOG_FATAL << "given array is too short to copy all bytes in (given: " << len << ", needed: " << this->len << ")";
        throw std::length_error("given array is too short to copy all bytes in (given: " + std::to_string(len) + ", needed: " + std::to_string(this->len) + ")");
    }
    if (array == nullptr) {
        PLOG_FATAL << "cannot copy to nullptr array";
        throw std::invalid_argument("cannot copy to nullptr array");
    }
    std::memcpy(array, this->bytes, this->len);
}

void Bytes::copyToBytes(Bytes& b) const {
    // copys the bytes to the given Bytes object
    if (b.max_len < this->len) {
        PLOG_FATAL << "given Bytes object is too short to copy to (given: " << b.max_len << ", needed: " << this->len << ")";
        throw std::length_error("given Bytes object is too short to copy to (given: " + std::to_string(b.max_len) + ", needed: " + std::to_string(this->len) + ")");
    }
    std::memcpy(b.bytes, this->bytes, this->len);
    b.len = this->len;
}

void Bytes::addcopyToBytes(Bytes& b) const {
    // adds the bytes to the given Bytes object
    if (b.max_len < this->len + b.len) {
        PLOG_FATAL << "given Bytes object is too short to add copy to (given: " << b.max_len << ", needed: " << this->len + b.len << ")";
        throw std::length_error("given Bytes object is too short to add copy to (given: " + std::to_string(b.max_len) + ", needed: " + std::to_string(this->len + b.len) + ")");
    }
    std::memcpy(b.bytes + b.len, this->bytes, this->len);
    b.len += this->len;
}

Bytes Bytes::copySubBytes(const size_t start, const size_t end) const {
    // returns a Bytes object that is a copy of the bytes from start to end
    if (start > end) {
        PLOG_FATAL << "start is bigger than end (start: " << start << ", end: " << end << ")";
        throw std::invalid_argument("start is bigger than end (start: " + std::to_string(start) + ", end: " + std::to_string(end) + ")");
    }
    if (end > this->len) {
        PLOG_FATAL << "end is bigger than len (end: " << end << ", len: " << this->len << ")";
        throw std::length_error("end is bigger than len (end: " + std::to_string(end) + ", len: " + std::to_string(this->len) + ")");
    }
    Bytes res(end - start);
    std::memcpy(res.bytes, this->bytes + start, end - start);
    res.len = end - start;
    return res;
}

size_t Bytes::getLen() const noexcept {
    // getter for the length in bytes
    return this->len;
}

size_t Bytes::getMaxLen() const noexcept {
    // getter for the maximum length in bytes
    return this->max_len;
}

void Bytes::addByte(const unsigned char byte) {
    // adds one byte at the end of the byte array by reference
    if (this->len >= this->max_len) {
        PLOG_FATAL << "Bytes are already full. Cannot add an other byte (max_len = len = " << this->max_len << ")";
        throw std::length_error("Bytes are already full. Cannot add an other byte (max_len = len = " + std::to_string(this->max_len) + ")");
    }
    this->bytes[this->len] = byte;
    this->len++;
}

bool Bytes::isEmpty() const noexcept {
    // returns true if there are no bytes in the array
    return this->len == 0;
}

bool Bytes::operator==(const Bytes& b2) const noexcept {
    // returns true if the byte arrays of the two Byte objects are equal
    if (this->len != b2.len) return false;
    return std::memcmp(this->bytes, b2.bytes, this->len) == 0;
}

bool Bytes::operator!=(const Bytes& b2) const noexcept {
    // returns true if the byte arrays of the two Byte objects are not equal
    if (this->len != b2.len) return true;
    return std::memcmp(this->bytes, b2.bytes, this->len) != 0;
}

Bytes Bytes::operator+(const Bytes& b2) const {
    // performs an add elementwise (the two byte arrays are added to each other (elementwise) mod 256)
    if (this->len != b2.len) {
        PLOG_FATAL << "cannot add bytes with different lengths (len1: " << this->len << ", len2: " << b2.len << ")";
        throw std::length_error("cannot add bytes with different lengths");
    }
    Bytes res(this->max_len);
    for (size_t i = 0; i < this->len; i++) {
        res.bytes[i] = this->bytes[i] + b2.bytes[i];
    }
    res.len = this->len;
    return res;
}

Bytes Bytes::operator-(const Bytes& b2) const {
    // performs an subtract elementwise (the second byte array is subtracted from the first (elementwise) mod 256)
    if (this->len != b2.len) {
        PLOG_FATAL << "cannot subtract bytes with different lengths (len1: " << this->len << ", len2: " << b2.len << ")";
        throw std::length_error("cannot subtract bytes with different lengths");
    }
    Bytes res(this->max_len);
    for (size_t i = 0; i < this->len; i++) {
        res.bytes[i] = this->bytes[i] - b2.bytes[i];
    }
    res.len = this->len;
    return res;
}

std::string Bytes::toHex() const noexcept {
    // returns a string (with 2*len chars) that is the hexadecimal representation of the Bytes
    constexpr char hexChars[] = "0123456789ABCDEF";  // Array to map values to hex characters
    std::string hexString;
    hexString.reserve(this->len * 2);  // Reserve space for the final hex string

    for (size_t i = 0; i < this->len; ++i) {
        hexString.push_back(hexChars[this->bytes[i] >> 4]);   // High nibble
        hexString.push_back(hexChars[this->bytes[i] & 0xF]);  // Low nibble
    }

    return hexString;
}

u_int64_t Bytes::toLong() const {
    // returns a long that is the decimal representation of the Bytes
    if (this->len > sizeof(u_int64_t)) {
        PLOG_ERROR << "Bytes::toLong() is not defined for Bytes with more than 8 bytes";
        throw std::length_error("Bytes::toLong() is not defined for Bytes with more than 8 bytes");
    }
    u_int64_t res = 0;
    unsigned char reversedBytes[this->len];
    for (size_t i = 0; i < this->len; ++i) {
        reversedBytes[this->len - 1 - i] = this->bytes[i];
    }
    std::memcpy(&res, reversedBytes, this->len);
    return res;
}
