#pragma once
#ifndef RNG_H
#define RNG_H
#include "openssl/err.h"
#include "openssl/rand.h"
#include <vector>
#include <iostream>
#include <tgmath.h>

class RNG{
public:
    std::vector<unsigned char> get_random_bytes(int num) const;
    //long get_random_number(long min, long max) const;
};

#endif //RNG_H