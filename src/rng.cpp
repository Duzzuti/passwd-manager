#include "rng.h"

std::vector<unsigned char> RNG::get_random_bytes(int num) const{
    unsigned char rand_bytes[num];      //creates a new buffer with the given length
    std::vector<unsigned char> ret{};
    if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) == 1) {  //generates the random bytes
        for (int i = 0; i < sizeof(rand_bytes); i++) {
            ret.push_back(rand_bytes[i]);   //turns the buffer into a vector
        }
        return ret;
    } else {
        //some error in openssl occured (maybe the given entropy was too low)
        throw std::runtime_error("Error occured in get_random_bytes: "+ERR_get_error());
    }
}
