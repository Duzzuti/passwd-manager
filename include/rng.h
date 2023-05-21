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
};

#endif //RNG_H