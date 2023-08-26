#include <gtest/gtest.h>

#include <cstring>
#include <fstream>

#include "bytes_opt.h"
#include "rng.h"
#include "timer.h"

const constexpr int ITERS = 10;
const constexpr int NUM_BYTES = 10000000;
const constexpr int NUM_BYTES2 = 5000000;
const constexpr int NUM_BYTES3 = 100000;
const constexpr int NUM_BYTES4 = 1000000;

void filing(std::string op, u_int64_t iters, u_int64_t avg, u_int64_t slowest) {
    std::ofstream file;
    file.open("bytes_opt_bench.csv", std::ios::app);
    file << op << "," << iters << "," << avg << "," << slowest << "\n";
    file.close();
}

TEST(BytesOPT, fromLong) {
    u_int64_t* l = new u_int64_t[NUM_BYTES3];
    for (int i = 0; i < NUM_BYTES3; i++) {
        unsigned char bytes[8];
        RNG::fill_random_bytes(bytes, 8);
        std::memcpy(&l[i], bytes, 8);
    }
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        for (int j = 0; j < NUM_BYTES3; j++) BytesOpt b = BytesOpt::fromLong(l[j]);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    delete[] l;
    filing("fromLong", NUM_BYTES3, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, addByte) {
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        BytesOpt b(NUM_BYTES);
        for (int i = 0; i < NUM_BYTES; i++) {
            b.addByte(0xad);
        }
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("addByte", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, setBytes) {
    BytesOpt b(NUM_BYTES);
    BytesOpt c(NUM_BYTES);
    c.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        b.setBytes(c.getBytes(), c.getLen());
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("setBytes", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, addBytes) {
    BytesOpt b(NUM_BYTES * (ITERS + 1));
    BytesOpt c(NUM_BYTES);
    for (int i = 0; i < NUM_BYTES; i++) {
        b.addByte(0xad);
        c.addByte(0xad);
    }
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        b.addBytes(c.getBytes(), c.getLen());
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("addBytes", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, constructor) {
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        BytesOpt b(NUM_BYTES2);
        b.fillrandom();
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("constructor", NUM_BYTES2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, addrandom) {
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        BytesOpt b(NUM_BYTES);
        b.addrandom(NUM_BYTES * 0.99);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("addrandom", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, copyconstructor) {
    BytesOpt b(NUM_BYTES * 10);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        BytesOpt c(b);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("copyconstructor", NUM_BYTES * 10, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, copyassignment) {
    BytesOpt b(NUM_BYTES * 10);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        BytesOpt c = b;
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("copyassignment", NUM_BYTES * 10, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, getByteArray) {
    BytesOpt b(NUM_BYTES2 * 2);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        unsigned char* array = new unsigned char[NUM_BYTES2 * 2];
        b.copyToArray(array, NUM_BYTES2 * 2);
        if (i != ITERS - 1) timer.recordTime();
        delete[] array;
    }
    timer.stop();
    filing("getByteArray", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, copyToBytes) {
    BytesOpt b(NUM_BYTES2 * 2);
    b.fillrandom();
    BytesOpt c(NUM_BYTES2 * 2);
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        b.copyToBytes(c);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("copyToBytes", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, copyaddToBytes) {
    BytesOpt b(NUM_BYTES2 * 2 - 7);
    b.fillrandom();
    BytesOpt c(NUM_BYTES2 * 2 * ITERS);
    c.addrandom(7);
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        b.addcopyToBytes(c);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("copyaddToBytes", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, copySubBytes) {
    BytesOpt b(NUM_BYTES2 * 2);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        BytesOpt c = b.copySubBytes(3, NUM_BYTES2 * 2 - 7);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("copySubBytes", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, equal) {
    BytesOpt b(NUM_BYTES2 * 2 + ITERS);
    b.addrandom(NUM_BYTES2 * 2);
    BytesOpt c(NUM_BYTES2 * 2);
    c = b;
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        if (b == c) b.addByte(0xad);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("equal", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, plus) {
    BytesOpt a(NUM_BYTES3);
    a.fillrandom();
    BytesOpt b(NUM_BYTES3);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        a = a + b;
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("plus", NUM_BYTES3, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, minus) {
    BytesOpt a(NUM_BYTES3);
    a.fillrandom();
    BytesOpt b(NUM_BYTES3);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        a = a - b;
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("minus", NUM_BYTES3, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, toHex) {
    BytesOpt b(NUM_BYTES2 * 2);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        b.toHex();
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("toHex", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
}

TEST(BytesOPT, toLong) {
    BytesOpt b(8);
    b.fillrandom();
    Timer timer;
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        for (int j = 0; j < NUM_BYTES4; j++) b.toLong();
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    filing("toLong", NUM_BYTES4, timer.getAverageTime(), timer.getSlowest());
}