#include "rng.h"

#include "openssl/err.h"
#include "openssl/rand.h"

#include "bytes.h"
#include "logger.h"

std::string RNG::get_random_string(const unsigned int num) {
    // gets a random string with the given length
    // it will get some random bytes, turn them into a long
    // and calculate a random number in that range by shorting it down to the range (long mod range + lower)
    // the buffer size should be long if really random numbers are needed
    // it can be short (1) if the range is 255.
    if (num < 1) {
        PLOG_FATAL << "num cannot be zero";
        throw std::logic_error("num cannot be zero");
    }
    std::string res;
    unsigned int len = 0;
    while (true) {
        unsigned char rand_bytes[num];  // creates a new buffer with the given length
        if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) == 1) {
            for (int i = 0; i < sizeof(rand_bytes); i++) {
                if (rand_bytes[i] >= 65 && rand_bytes[i] <= 90 || rand_bytes[i] >= 97 && rand_bytes[i] <= 122) {
                    res += rand_bytes[i];
                    len++;
                }
                if (len == num) {
                    return res;
                }
            }
        } else {
            // some error in openssl occurred (maybe the given entropy was too low)
            PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
            throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
        }
    }
}

void RNG::fill_random_bytes(unsigned char* bytes, const unsigned int num) {
    if (RAND_bytes(bytes, num) != 1) {
        // some error in openssl occurred (maybe the given entropy was too low)
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
}

void RNG::fill_random_bytes(Bytes& bytes, const unsigned int num) {
    if (bytes.getMaxLen() < num) {
        PLOG_FATAL << "bytes length cannot be smaller than num";
        throw std::logic_error("bytes length cannot be smaller than num");
    }
    if (RAND_bytes(bytes.getBytes(), num) != 1) {
        // some error in openssl occurred (maybe the given entropy was too low)
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
    bytes.setLen(num);
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

    Bytes tmp_bytes(buffer_size);  // creates a new buffer with a given length
    if (RAND_bytes(tmp_bytes.getBytes(), buffer_size) == 1) {
        tmp_bytes.setLen(buffer_size);
        return (tmp_bytes.toLong() % (upper - lower)) + lower;
    } else {
        // some error in openssl occurred (maybe the given entropy was too low)
        PLOG_FATAL << "Error occurred while getting random bytes from openssl. OpenSSL errorcode: " << ERR_get_error();
        throw std::runtime_error("Error occurred in get_random_bytes: " + std::to_string(ERR_get_error()));
    }
}
