#include <map>
#include "tgmath.h"
#include "gtest/gtest.h"
#include "test_settings.cpp"
#include "rng.h"


TEST(RNGClass, entropy){
    //calculates the entropy of the RNG
    std::unordered_map<unsigned char, long> bytemap{};
    for(long i=0; i<TEST_NUMBER_ENTROPY; i++){
        bytemap[RNG::get_random_bytes(1)[0]] += 1;
    }
    double entropy{};
    for(std::pair<unsigned char, long> key_value : bytemap){
        double num = key_value.second;
        double charprob = num/TEST_NUMBER_ENTROPY;
        if(isnanl(charprob) || charprob == 0){
            continue;
        }
        entropy += charprob*(log2(charprob));
    }
    entropy = -entropy;
    std::cout << "Entropy per byte (in bit): " << entropy << std::endl;
    EXPECT_GT(entropy, 8 - TEST_ENTROPY_ERROR);
}