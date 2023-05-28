#pragma once
#ifndef RNG_H
#define RNG_H

#include "openssl/err.h"
#include "openssl/rand.h"
#include <vector>
#include <iostream>

class RNG{
    /*
    this class will provide functionality about random generator 
    it uses openssl libraries and is cryptograhically secure
    */
public:
    static std::vector<unsigned char> get_random_bytes(const unsigned int num);     //get random bytes with the given length
    //get a random byte between a lower and upper bound
    static unsigned char get_random_byte(const unsigned char lower=0, const unsigned char upper=-1, const unsigned int buffer_size=4);
};

#endif //RNG_H