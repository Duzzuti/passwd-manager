#include "rng.h"

std::vector<unsigned char> RNG::get_random_bytes(int num) const{
    unsigned char rand_bytes[num];
    std::vector<unsigned char> ret{};
    if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) == 1) {
        for (int i = 0; i < sizeof(rand_bytes); i++) {
            ret.push_back(rand_bytes[i]);
        }
        return ret;
    } else {
        throw std::runtime_error("Error occured in get_random_bytes: "+ERR_get_error());
    }
}
