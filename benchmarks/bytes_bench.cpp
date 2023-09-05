// #include <gtest/gtest.h>

// #include <fstream>

// #include "bytes.h"
// #include "timer.h"

// const constexpr int ITERS = 10;
// const constexpr int NUM_BYTES = 10000000;
// const constexpr int NUM_BYTES2 = 5000000;
// const constexpr int NUM_BYTES3 = 100000;
// const constexpr int NUM_BYTES4 = 1000000;

// void filing(std::string op, u_int64_t iters, u_int64_t avg, u_int64_t slowest) {
//     std::ofstream file;
//     file.open("bytes_bench.csv", std::ios::app);
//     file << op << "," << iters << "," << avg << "," << slowest << "\n";
//     file.close();
// }

// TEST(Bytes, addByte) {
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         Bytes b;
//         for (int i = 0; i < NUM_BYTES; i++) {
//             b.addByte(0xad);
//         }
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("addByte", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, addBytes) {
//     Bytes b;
//     Bytes c;
//     for (int i = 0; i < NUM_BYTES; i++) {
//         b.addByte(0xad);
//         c.addByte(0xad);
//     }
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         b.addBytes(c);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("addBytes", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, constructor) {
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         Bytes b(NUM_BYTES2);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("constructor", NUM_BYTES2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, getByteArray) {
//     Bytes b(NUM_BYTES2);
//     Bytes c(NUM_BYTES2);
//     b.addBytes(c);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         unsigned char* array = new unsigned char[NUM_BYTES2 * 2];
//         b.getBytesArray(array, NUM_BYTES2 * 2);
//         if (i != ITERS - 1) timer.recordTime();
//         delete[] array;
//     }
//     timer.stop();
//     filing("getByteArray", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, getFirstBytes) {
//     Bytes b(NUM_BYTES2);
//     Bytes c(NUM_BYTES2);
//     b.addBytes(c);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         b.getFirstBytes(NUM_BYTES2 * 2 - 1);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("getFirstBytes", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, popFirstBytes) {
//     Bytes b(NUM_BYTES2);
//     Bytes c(NUM_BYTES2);
//     b.addBytes(c);
//     Bytes d = b;
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         d.popFirstBytes(NUM_BYTES2 * 2 - 1);
//         if (i != ITERS - 1) timer.recordTime();
//         d = b;
//     }
//     timer.stop();
//     filing("popFirstBytes", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, getFirstBytesFillup) {
//     Bytes b(NUM_BYTES2);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         b.getFirstBytesFilledUp(NUM_BYTES2 * 2, 0xad);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("getFirstBytesFillup", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, popFirstBytesFillup) {
//     Bytes b(NUM_BYTES2);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         b.popFirstBytesFilledUp(NUM_BYTES2 * 2, 0xad);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("popFirstBytesFillup", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, equal) {
//     Bytes b(NUM_BYTES2);
//     Bytes d(NUM_BYTES2);
//     b.addBytes(d);
//     Bytes c = b;
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         if (b == c) b.addByte(0xad);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("equal", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, plus) {
//     Bytes a(NUM_BYTES3);
//     Bytes b(NUM_BYTES3);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         a = a + b;
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("plus", NUM_BYTES3, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, minus) {
//     Bytes a(NUM_BYTES3);
//     Bytes b(NUM_BYTES3);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         a = a - b;
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("minus", NUM_BYTES3, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, toHex) {
//     Bytes b(NUM_BYTES2);
//     Bytes c(NUM_BYTES2);
//     b.addBytes(c);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         toHex(b);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("toHex", NUM_BYTES2 * 2, timer.getAverageTime(), timer.getSlowest());
// }

// TEST(Bytes, toLong) {
//     Bytes b(8);
//     Timer timer;
//     timer.start();
//     for (int i = 0; i < ITERS; i++) {
//         for (int j = 0; j < NUM_BYTES4; j++) toLong(b);
//         if (i != ITERS - 1) timer.recordTime();
//     }
//     timer.stop();
//     filing("toLong", NUM_BYTES4, timer.getAverageTime(), timer.getSlowest());
// }