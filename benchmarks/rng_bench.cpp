#include <gtest/gtest.h>
#include <fstream>

#include "rng.h"
#include "bytes.h"
#include "timer.h"

const constexpr int ITERS = 10;
const constexpr int NUM_BYTES = 3000000;

void filing(std::string op, u_int64_t iters, u_int64_t avg, u_int64_t slowest) {
    std::ofstream file;
    file.open("rng_bench.csv", std::ios::app);
    file << op << "," << iters << "," << avg << "," << slowest << "\n";
    file.close();
}

TEST(RNG, get_random_bytes){
    Timer timer;
    timer.start();
    for(int i = 0; i < ITERS; i++){
        RNG::get_random_bytes(NUM_BYTES);
        if(i != ITERS-1)
            timer.recordTime();
    }
    timer.stop();
    filing("get_random_bytes", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(RNG, get_random_byte){
    Timer timer;
    timer.start();
    for(int i = 0; i < ITERS; i++){
        for(int j = 0; j < NUM_BYTES; j++){
            // most complex call
            RNG::get_random_byte(12, 206, 8);
        }
        if(i != ITERS-1)
            timer.recordTime();
    }
    timer.stop();
    filing("get_random_byte", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}