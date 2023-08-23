#include "rng.h"

#include "bytes.h"
#include "logger.h"

std::vector<unsigned char> RNG::get_random_bytes(const unsigned int num) {
    unsigned char rand_bytes[num];  // creates a new buffer with the given length
    std::vector<unsigned char> ret{};
    if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) == 1) {  // generates the random bytes
        for (int i = 0; i < sizeof(rand_bytes); i++) {
            ret.push_back(rand_bytes[i]);  // turns the buffer into a vector
        }
        return ret;
    } else {
        // some error in openssl occurred (maybe the given entropy was too low)
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
}

std::vector<unsigned char> RNG::get_random_bytes_large(const u_int64_t num) {
    unsigned char* rand_bytes = new unsigned char[num];  // creates a new buffer with the given length
    std::vector<unsigned char> ret{};
    if (RAND_bytes(rand_bytes, sizeof(unsigned char) * num) == 1) {  // generates the random bytes
        for (int i = 0; i < sizeof(unsigned char) * num; i++) {
            ret.push_back(rand_bytes[i]);  // turns the buffer into a vector
        }
        delete[] rand_bytes;
        return ret;
    } else {
        // some error in openssl occurred (maybe the given entropy was too low)
        if (rand_bytes != nullptr) {
            delete[] rand_bytes;
        }
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
}

void RNG::fill_random_bytes(unsigned char* bytes, const unsigned int num) {
    if (RAND_bytes(bytes, num) != 1) {
        // some error in openssl occurred (maybe the given entropy was too low)
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
}

unsigned char RNG::get_random_byte(const unsigned char lower, const unsigned char upper, const unsigned int buffer_size) {
    // gets a random byte in a given range
    // it will get some random bytes, turn them into a long
    // and calculate a random number in that range by shorting it down to the range (long mod range + lower)
    // the buffer size should be long if really random numbers are needed
    // it can be short (1) if the range is 255.WORK
    if (buffer_size < 1 || buffer_size > 8) {
        PLOG_FATAL << "buffer size cannot be zero or greater than 8";
        throw std::logic_error("buffer size cannot be zero or greater than 8");
    }
    if (upper <= lower) {
        if (upper == lower) {
            return upper;  // if upper and lower are equal, return it (there is no need to randomize)
        } else {
            PLOG_FATAL << "upper bound cannot be <= lower bound";
            throw std::logic_error("upper bound cannot be <= lower bound");
        }
    }

    unsigned char rand_bytes[buffer_size];  // creates a new buffer with a given length
    Bytes tmp_bytes{};
    if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) == 1) {
        for (int i = 0; i < sizeof(rand_bytes); i++) {
            tmp_bytes.addByte(rand_bytes[i]);
        }
        return (toLong(tmp_bytes) % (upper - lower)) + lower;
    } else {
        // some error in openssl occurred (maybe the given entropy was too low)
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
}
