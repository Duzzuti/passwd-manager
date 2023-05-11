/*
its generating some random strings that are used for testing the hash function
the rng is from openssl and cryptograhically secure
*/
#include "test_utils.h"
#include "rng.h"

std::string gen_random_string(const int len) {
    std::string tmp_s{};
    for (int i = 0; i < len; ++i) {
        //generate each character and adds them together to a string
        tmp_s += RNG::get_random_bytes(1)[0];
    }
    
    return tmp_s;
}