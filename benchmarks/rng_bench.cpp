#include <gtest/gtest.h>

#include <fstream>

#include "bytes.h"
#include "rng.h"
#include "timer.h"

const constexpr int ITERS = 10;
const constexpr int NUM_BYTES = 3000000;

void filing(std::string op, u_int64_t iters, u_int64_t avg, u_int64_t slowest) {
    std::ofstream file;
    file.open("rng_bench.csv", std::ios::app);
    file << op << "," << iters << "," << avg << "," << slowest << "\n";
    file.close();
}

TEST(RNG, get_random_bytes) {
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        RNG::get_random_string(NUM_BYTES);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("get_random_string", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(RNG, fill_random_bytes) {
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        Bytes bytes(NUM_BYTES);
        RNG::fill_random_bytes(bytes, NUM_BYTES);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("get_random_bytes", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(RNG, get_random_byte) {
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        for (int j = 0; j < NUM_BYTES; j++) {
            // most complex call
            RNG::get_random_byte(12, 206, 8);
        }
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("get_random_byte", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}