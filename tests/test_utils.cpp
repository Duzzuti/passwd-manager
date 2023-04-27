/*
this code was inspired from
https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c

its generating some "random" strings that are used for testing the hash function
*/
#include "test_utils.h"
#include "rng.h"

std::string gen_random_string(const int len) {
    std::string tmp_s{};
    for (int i = 0; i < len; ++i) {
        tmp_s += RNG().get_random_bytes(1)[0];
    }
    
    return tmp_s;
}