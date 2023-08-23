#pragma once

#include <iostream>
#include <vector>

#include "openssl/err.h"
#include "openssl/rand.h"

class RNG {
    /*
    this class will provide functionality about random generator
    it uses openssl libraries and is cryptographically secure
    */
   public:
    static std::vector<unsigned char> get_random_bytes(const unsigned int num);  // get random bytes with the given length
    static std::vector<unsigned char> get_random_bytes_large(const u_int64_t num);  // get random bytes with the given length (large)
    // get a random byte between a lower and upper bound
    static unsigned char get_random_byte(const unsigned char lower = 0, const unsigned char upper = -1, const unsigned int buffer_size = 4);
};
