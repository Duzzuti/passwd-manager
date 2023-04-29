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
    std::vector<unsigned char> get_random_bytes(int num) const;     //get random bytes with the given length
    //long get_random_number(long min, long max) const;
};

#endif //RNG_H