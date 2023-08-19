#include <gtest/gtest.h>

#include <atomic>
#include <fstream>
#include <thread>

#include "bench_utils.h"
#include "chainhash_modes.h"
#include "hash_modes.h"
#include "timer.h"

const constexpr u_int8_t iters_indices = 3;
const constexpr u_int64_t iterations[] = {10000, 100000, 1000000};
// const constexpr u_int8_t RUN_ITERS[] = {3,3,3};
const constexpr u_int8_t RUN_ITERS[] = {5, 5, 5};
// const constexpr u_int8_t RUN_ITERS[] = {10,5,3};

// flag that indicates whether the measurement thread should terminate
std::atomic<bool> _terminateMeasurementThread(false);
std::atomic<u_int64_t> _memory_max(0);
std::atomic<u_int64_t> _memory_min(0);
std::atomic<u_int64_t> _memory_avg(0);
std::atomic<u_int64_t> _memory_base(0);

void MemoryThread() {
    // this thread will measure the memory usage
    _memory_max = 0;
    _memory_min = 0;
    _terminateMeasurementThread = false;
    u_int64_t memory = 0;
    u_int64_t memory_max = 0;
    u_int64_t memory_min = 0;
    u_int64_t memory_avg = 0;
    u_int64_t memory_sum = 0;
    u_int64_t memory_count = 0;
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

void filing(std::string chainhash, std::string hash, u_int64_t iters, u_int64_t avg, u_int64_t slowest, u_int64_t memory_max, u_int64_t memory_min, u_int64_t memory_avg, u_int64_t memory_base) {
    std::ofstream file;
    file.open("chainhash_bench.csv", std::ios::app);
    file << chainhash << "," << hash << "," << iters << "," << avg << "," << slowest << "," << memory_max << "," << memory_min << "," << memory_avg << "," << memory_base << "\n";
    file.close();
}

TEST(ChainHash, normal) {
    // setup the chainhashes
    _memory_base = 0;
    std::string data_str = "test";
    u_int8_t ichash = 1;
    std::string chainhash_info = ChainHashModes::getShortInfo(CHModes(ichash));
    ChainHashData chd1{Format{CHModes(ichash)}};
    chd1.generateRandomData();
    std::vector<ChainHash> chainhashes = {ChainHash{CHModes(ichash), iterations[0], chd1}, ChainHash{CHModes(ichash), iterations[1], chd1}, ChainHash{CHModes(ichash), iterations[2], chd1}};
    // the benchmark starts here
    // run the chainhashes
    for (u_int8_t ihash = 1; ihash <= MAX_HASHMODE_NUMBER; ihash++) {
        std::shared_ptr<Hash> hash = HashModes::getHash(HModes(ihash));
        std::string hash_info = HashModes::getInfo(HModes(ihash), true);
        for (u_int64_t iters_ind = 0; iters_ind < iters_indices; iters_ind++) {
            ChainHash ch = chainhashes[iters_ind];
            u_int8_t run_iters = RUN_ITERS[iters_ind];
            Timer timer;
            if (_memory_base == 0) {
                _memory_base = getPhysicalMem();
                for (int i = 0; i < run_iters; i++) ChainHashModes::performChainHash(ch, hash, data_str);
            }
            std::thread memoryThread(MemoryThread);
            timer.start();
            for (int i = 0; i < run_iters; i++) {
                ChainHashModes::performChainHash(ch, hash, data_str);
                timer.recordTime();
            }
            timer.stop();
            _terminateMeasurementThread = true;
            memoryThread.join();
            filing(chainhash_info, hash_info, ch.getIters(), timer.getAverageTime(), timer.getSlowest(), _memory_max, _memory_min, _memory_avg, _memory_base);
        }
    }
}

TEST(ChainHash, constant) {
    // setup the chainhashes
    _memory_base = 0;
    std::string data_str = "test";
    u_int8_t ichash = 2;
    std::string chainhash_info = ChainHashModes::getShortInfo(CHModes(ichash));
    ChainHashData chd1{Format{CHModes(ichash)}};
    chd1.generateRandomData();
    std::vector<ChainHash> chainhashes = {ChainHash{CHModes(ichash), iterations[0], chd1}, ChainHash{CHModes(ichash), iterations[1], chd1}, ChainHash{CHModes(ichash), iterations[2], chd1}};
    // the benchmark starts here
    // run the chainhashes
    for (u_int8_t ihash = 1; ihash <= MAX_HASHMODE_NUMBER; ihash++) {
        std::shared_ptr<Hash> hash = HashModes::getHash(HModes(ihash));
        std::string hash_info = HashModes::getInfo(HModes(ihash), true);
        for (u_int64_t iters_ind = 0; iters_ind < iters_indices; iters_ind++) {
            ChainHash ch = chainhashes[iters_ind];
            u_int8_t run_iters = RUN_ITERS[iters_ind];
            Timer timer;
            if (_memory_base == 0) {
                _memory_base = getPhysicalMem();
                for (int i = 0; i < run_iters; i++) ChainHashModes::performChainHash(ch, hash, data_str);
            }
            std::thread memoryThread(MemoryThread);
            timer.start();
            for (int i = 0; i < run_iters; i++) {
                ChainHashModes::performChainHash(ch, hash, data_str);
                timer.recordTime();
            }
            timer.stop();
            _terminateMeasurementThread = true;
            memoryThread.join();
            filing(chainhash_info, hash_info, ch.getIters(), timer.getAverageTime(), timer.getSlowest(), _memory_max, _memory_min, _memory_avg, _memory_base);
        }
    }
}

TEST(ChainHash, count) {
    // setup the chainhashes
    _memory_base = 0;
    std::string data_str = "test";
    u_int8_t ichash = 3;
    std::string chainhash_info = ChainHashModes::getShortInfo(CHModes(ichash));
    ChainHashData chd1{Format{CHModes(ichash)}};
    chd1.generateRandomData();
    std::vector<ChainHash> chainhashes = {ChainHash{CHModes(ichash), iterations[0], chd1}, ChainHash{CHModes(ichash), iterations[1], chd1}, ChainHash{CHModes(ichash), iterations[2], chd1}};
    // the benchmark starts here
    // run the chainhashes
    for (u_int8_t ihash = 1; ihash <= MAX_HASHMODE_NUMBER; ihash++) {
        std::shared_ptr<Hash> hash = HashModes::getHash(HModes(ihash));
        std::string hash_info = HashModes::getInfo(HModes(ihash), true);
        for (u_int64_t iters_ind = 0; iters_ind < iters_indices; iters_ind++) {
            ChainHash ch = chainhashes[iters_ind];
            u_int8_t run_iters = RUN_ITERS[iters_ind];
            Timer timer;
            if (_memory_base == 0) {
                _memory_base = getPhysicalMem();
                for (int i = 0; i < run_iters; i++) ChainHashModes::performChainHash(ch, hash, data_str);
            }
            std::thread memoryThread(MemoryThread);
            timer.start();
            for (int i = 0; i < run_iters; i++) {
                ChainHashModes::performChainHash(ch, hash, data_str);
                timer.recordTime();
            }
            timer.stop();
            _terminateMeasurementThread = true;
            memoryThread.join();
            filing(chainhash_info, hash_info, ch.getIters(), timer.getAverageTime(), timer.getSlowest(), _memory_max, _memory_min, _memory_avg, _memory_base);
        }
    }
}

TEST(ChainHash, constant_count) {
    // setup the chainhashes
    _memory_base = 0;
    std::string data_str = "test";
    u_int8_t ichash = 4;
    std::string chainhash_info = ChainHashModes::getShortInfo(CHModes(ichash));
    ChainHashData chd1{Format{CHModes(ichash)}};
    chd1.generateRandomData();
    std::vector<ChainHash> chainhashes = {ChainHash{CHModes(ichash), iterations[0], chd1}, ChainHash{CHModes(ichash), iterations[1], chd1}, ChainHash{CHModes(ichash), iterations[2], chd1}};
    // the benchmark starts here
    // run the chainhashes
    for (u_int8_t ihash = 1; ihash <= MAX_HASHMODE_NUMBER; ihash++) {
        std::shared_ptr<Hash> hash = HashModes::getHash(HModes(ihash));
        std::string hash_info = HashModes::getInfo(HModes(ihash), true);
        for (u_int64_t iters_ind = 0; iters_ind < iters_indices; iters_ind++) {
            ChainHash ch = chainhashes[iters_ind];
            u_int8_t run_iters = RUN_ITERS[iters_ind];
            Timer timer;
            if (_memory_base == 0) {
                _memory_base = getPhysicalMem();
                for (int i = 0; i < run_iters; i++) ChainHashModes::performChainHash(ch, hash, data_str);
            }
            std::thread memoryThread(MemoryThread);
            timer.start();
            for (int i = 0; i < run_iters; i++) {
                ChainHashModes::performChainHash(ch, hash, data_str);
                timer.recordTime();
            }
            timer.stop();
            _terminateMeasurementThread = true;
            memoryThread.join();
            filing(chainhash_info, hash_info, ch.getIters(), timer.getAverageTime(), timer.getSlowest(), _memory_max, _memory_min, _memory_avg, _memory_base);
        }
    }
}

TEST(ChainHash, quadratic) {
    // setup the chainhashes
    _memory_base = 0;
    std::string data_str = "test";
    u_int8_t ichash = 5;
    std::string chainhash_info = ChainHashModes::getShortInfo(CHModes(ichash));
    ChainHashData chd1{Format{CHModes(ichash)}};
    chd1.generateRandomData();
    std::vector<ChainHash> chainhashes = {ChainHash{CHModes(ichash), iterations[0], chd1}, ChainHash{CHModes(ichash), iterations[1], chd1}, ChainHash{CHModes(ichash), iterations[2], chd1}};
    // the benchmark starts here
    // run the chainhashes
    for (u_int8_t ihash = 1; ihash <= MAX_HASHMODE_NUMBER; ihash++) {
        std::shared_ptr<Hash> hash = HashModes::getHash(HModes(ihash));
        std::string hash_info = HashModes::getInfo(HModes(ihash), true);
        for (u_int64_t iters_ind = 0; iters_ind < iters_indices; iters_ind++) {
            ChainHash ch = chainhashes[iters_ind];
            u_int8_t run_iters = RUN_ITERS[iters_ind];
            Timer timer;
            if (_memory_base == 0) {
                _memory_base = getPhysicalMem();
                for (int i = 0; i < run_iters; i++) ChainHashModes::performChainHash(ch, hash, data_str);
            }
            std::thread memoryThread(MemoryThread);
            timer.start();
            for (int i = 0; i < run_iters; i++) {
                ChainHashModes::performChainHash(ch, hash, data_str);
                timer.recordTime();
            }
            timer.stop();
            _terminateMeasurementThread = true;
            memoryThread.join();
            filing(chainhash_info, hash_info, ch.getIters(), timer.getAverageTime(), timer.getSlowest(), _memory_max, _memory_min, _memory_avg, _memory_base);
        }
    }
}