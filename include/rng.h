#pragma once

#include <iostream>
#include <vector>

class Bytes;

class RNG {
    /*
    this class will provide functionality about random generator
    it uses openssl libraries and is cryptographically secure
    */
   public:
    static std::string get_random_string(const unsigned int num);                 // get random string with the given length
    static void fill_random_bytes(unsigned char* bytes, const unsigned int num);  // fill the given bytes with random bytes
    static void fill_random_bytes(Bytes& bytes, const unsigned int num);          // fill the given bytes with num random bytes
    // get a random byte between a lower and upper bound
    static unsigned char get_random_byte(const unsigned char lower = 0, const unsigned char upper = -1, const unsigned int buffer_size = 4);
};
