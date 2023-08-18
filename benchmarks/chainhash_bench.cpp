#include <gtest/gtest.h>

#include <atomic>
#include <fstream>
#include <thread>

#include "bench_utils.h"
#include "chainhash_modes.h"
#include "hash_modes.h"
#include "timer.h"

const constexpr u_int64_t i1[] = {1000, 100000, 1000000};
const constexpr u_int64_t RUN_ITERS = 3;

// flag that indicates whether the measurement thread should terminate
std::atomic<bool> _terminateMeasurementThread(false);
std::atomic<u_int64_t> _memory_max(0);
std::atomic<u_int64_t> _memory_min(0);
std::atomic<u_int64_t> _memory_avg(0);
std::atomic<u_int64_t> _memory_base(0);

void MemoryThread() {
    // this thread will measure the memory usage
    u_int64_t memory;
    u_int64_t memory_max = 0;
    u_int64_t memory_min = 0;
    u_int64_t memory_avg = 0;
    u_int64_t memory_sum = 0;
    u_int64_t memory_count = 0;
    _memory_base = getPhysicalMem();
    while (!_terminateMeasurementThread) {
        memory = getPhysicalMem();
        memory_sum += memory;
        memory_count++;
        if (memory_max < memory || memory_max == 0) {
            memory_max = memory;
        }
        if (memory_min > memory || memory_min == 0) {
            memory_min = memory;
        }
        memory_avg = memory_sum / memory_count;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    _memory_max = memory_max;
    _memory_min = memory_min;
    _memory_avg = memory_avg;
}

void filing(u_int64_t avg, u_int64_t slowest, u_int64_t memory_max, u_int64_t memory_min, u_int64_t memory_avg, u_int64_t memory_base) {
    std::ofstream file;
    file.open("chainhash_bench.csv", std::ios::app);
    file << avg << "," << slowest << "," << memory_max << "," << memory_min << "," << memory_avg << "," << memory_base << "\n";
    file.close();
}

TEST(ChainHash, chainhash) {
    // setup the chainhashes
    std::shared_ptr<Hash> hash = HashModes::getHash(HASHMODE_SHA256);
    std::string data_str = "test";
    Bytes data;
    data.setBytes(std::vector<unsigned char>(data_str.begin(), data_str.end()));
    ChainHashData chd1{Format{CHAINHASH_NORMAL}};
    chd1.generateRandomData();
    ChainHash c1{CHAINHASH_NORMAL, i1[0], chd1};
    ChainHash c2{CHAINHASH_NORMAL, i1[1], chd1};
    ChainHash c3{CHAINHASH_NORMAL, i1[2], chd1};

    // the benchmark starts here
    // run the chainhashes
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (int i = 0; i < RUN_ITERS; i++) {
        ChainHashModes::performChainHash(c1, std::move(hash), data);
        timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing(timer.getAverageTime(), timer.getSlowest(), _memory_max, _memory_min, _memory_avg, _memory_base);

    ChainHashModes::performChainHash(c1, std::move(hash), data_str);

    ChainHashModes::performChainHash(c2, std::move(hash), data);
    ChainHashModes::performChainHash(c2, std::move(hash), data_str);

    ChainHashModes::performChainHash(c3, std::move(hash), data);
    ChainHashModes::performChainHash(c3, std::move(hash), data_str);
}