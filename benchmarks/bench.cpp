#include <gtest/gtest.h>

#include <fstream>
#include <thread>

#include "api.h"
#include "bench_utils.h"
#include "rng.h"
#include "timer.h"
#include "utility.h"

const constexpr u_int64_t CITERS = 1000000;
const constexpr u_int64_t CITERS_SMALL = 1;
const constexpr u_int64_t DATA_SIZE_SMALL = 1024 * 1024;  // 1MB
const constexpr u_int64_t DATA_SIZE_SMALL_MB = 1;
const constexpr u_int64_t DATA_SIZE_MEDIUM = 1024 * 1024 * 50;  // 50MB
const constexpr u_int64_t DATA_SIZE_MEDIUM_MB = 50;
const constexpr u_int64_t DATA_SIZE_LARGE = 1024 * 1024 * 1024;  // 1GB
const constexpr u_int64_t DATA_SIZE_LARGE_MB = 1024;
const constexpr u_int64_t ITERS = 2;
const constexpr u_int64_t FILES = 10;  // 100
std::string password = "password";

std::atomic<bool> _terminateMeasurementThread(false);
std::atomic<u_int64_t> _memory_max(0);
std::atomic<u_int64_t> _memory_min(0);
std::atomic<u_int64_t> _memory_avg(0);
std::atomic<u_int64_t> _memory_base(0);

void MemoryThread() {
    // this thread will measure the memory usage
    _memory_max = 0;
    _memory_min = 0;
    _memory_base = getPhysicalMem();
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

void filing(std::string op, u_int64_t iters, u_int64_t size, u_int64_t avg, u_int64_t slowest) {
    std::ofstream file;
    file.open("bench.csv", std::ios::app);
    file << op << "," << iters << "," << size << "," << avg << "," << slowest << "," << _memory_avg << "," << _memory_max << "," << _memory_base << "\n";
    file.close();
}

TEST(Benchmark_write, small_sha256) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("small_sha256", CITERS, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, small_sha384) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("small_sha384", CITERS, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, small_sha512) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("small_sha512", CITERS, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, medium_sha256) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("medium_sha256", CITERS, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, medium_sha384) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("medium_sha384", CITERS, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, medium_sha512) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("medium_sha512", CITERS, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, large_sha256) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_LARGE));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("large_sha256", CITERS, DATA_SIZE_LARGE_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, large_sha384) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_LARGE));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("large_sha384", CITERS, DATA_SIZE_LARGE_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, large_sha512) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_LARGE));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < 1; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        std::cout << file << std::endl;
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        std::cout << "created data header" << std::endl;
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        std::cout << "got file data" << std::endl;
        api.getEncryptedData(fds);
        std::cout << "got encrypted data" << std::endl;
        api.writeToFile();
        std::cout << "wrote to file" << std::endl;
        api.logout();
        std::cout << "logged out" << std::endl;
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    std::cout << "stopped timer" << std::endl;
    _terminateMeasurementThread = true;
    memoryThread.join();
    std::cout << "joined thread" << std::endl;
    filing("large_sha512", CITERS, DATA_SIZE_LARGE_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, small_sha256_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("small_sha256_nochain", CITERS_SMALL, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, small_sha384_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("small_sha384_nochain", CITERS_SMALL, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, small_sha512_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("small_sha512_nochain", CITERS_SMALL, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, medium_sha256_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("medium_sha256_nochain", CITERS_SMALL, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, medium_sha384_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("medium_sha384_nochain", CITERS_SMALL, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, medium_sha512_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("medium_sha512_nochain", CITERS_SMALL, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, large_sha256_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_LARGE));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("large_sha256_nochain", CITERS_SMALL, DATA_SIZE_LARGE_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, large_sha384_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_LARGE));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < ITERS; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("large_sha384_nochain", CITERS_SMALL, DATA_SIZE_LARGE_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_write, large_sha512_nochain) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS_SMALL);
    ds.setChainHash2Iters(CITERS_SMALL);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_LARGE));
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < 1; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = charVecToString(RNG::get_random_bytes(5)) + ".enc";
        std::cout << file << std::endl;
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        std::cout << "created data header" << std::endl;
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        std::cout << "got file data" << std::endl;
        api.getEncryptedData(fds);
        std::cout << "got encrypted data" << std::endl;
        api.writeToFile();
        std::cout << "wrote to file" << std::endl;
        api.logout();
        std::cout << "logged out" << std::endl;
        if (i != ITERS - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    std::cout << "stopped timer" << std::endl;
    _terminateMeasurementThread = true;
    memoryThread.join();
    std::cout << "joined thread" << std::endl;
    filing("large_sha512_nochain", CITERS_SMALL, DATA_SIZE_LARGE_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_read1, sha256) {
    std::filesystem::path files[FILES];
    for (int i = 0; i < FILES; i++) {
        files[i] = RNG::get_random_string(10) + ".enc";
    }
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(1);
    ds.setChainHash2Iters(1);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    for (u_int64_t i = 0; i < FILES; i++) {
        std::cout << i << std::endl;
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = files[i];
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        ErrorStruct<FileDataStruct> fds = api.getFileData();
        assert(fds.isSuccess());
        fds.returnRef().dec_data = data;
        api.getEncryptedData(fds.returnRef());
        api.writeToFile();
        api.logout();
    }
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < FILES; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = files[i];
        api.selectFile(file);
        api.verifyPassword(password);
        FileDataStruct fds = api.getDecryptedData().returnValue();
        assert(fds.dec_data == data);
        api.logout();
        if (i != FILES - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("read_sha256", 1, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_read1, sha384) {
    std::filesystem::path files[FILES];
    for (int i = 0; i < FILES; i++) {
        files[i] = RNG::get_random_string(10) + ".enc";
    }
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA384);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(1);
    ds.setChainHash2Iters(1);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    for (u_int64_t i = 0; i < FILES; i++) {
        std::cout << i << std::endl;
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = files[i];
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        ErrorStruct<FileDataStruct> fds = api.getFileData();
        assert(fds.isSuccess());
        fds.returnRef().dec_data = data;
        api.getEncryptedData(fds.returnRef());
        api.writeToFile();
        api.logout();
    }
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < FILES; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = files[i];
        api.selectFile(file);
        api.verifyPassword(password);
        FileDataStruct fds = api.getDecryptedData().returnValue();
        assert(fds.dec_data == data);
        api.logout();
        if (i != FILES - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("read_sha384", 1, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_read1, sha512) {
    std::filesystem::path files[FILES];
    for (int i = 0; i < FILES; i++) {
        files[i] = RNG::get_random_string(10) + ".enc";
    }
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA512);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(1);
    ds.setChainHash2Iters(1);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_SMALL));
    for (u_int64_t i = 0; i < FILES; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = files[i];
        api.createFile(file);
        api.selectFile(file);
        api.createDataHeader(password, ds);
        FileDataStruct fds = api.getFileData().returnValue();
        fds.dec_data = data;
        api.getEncryptedData(fds);
        api.writeToFile();
        api.logout();
    }
    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();
    for (u_int64_t i = 0; i < FILES; i++) {
        API api{FILEMODE_PASSWORD};
        std::filesystem::path file = files[i];
        api.selectFile(file);
        api.verifyPassword(password);
        FileDataStruct fds = api.getDecryptedData().returnValue();
        assert(fds.dec_data == data);
        api.logout();
        if (i != FILES - 1) {
            timer.recordTime();
        }
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("read_sha512", 1, DATA_SIZE_SMALL_MB, timer.getAverageTime(), timer.getSlowest());
}

TEST(Benchmark_read2, sha256) {
    DataHeaderSettingsIters ds;
    ds.setFileDataMode(FILEMODE_PASSWORD);
    ds.setHashMode(HASHMODE_SHA256);
    ds.setChainHash1Mode(CHAINHASH_CONSTANT_COUNT_SALT);
    ds.setChainHash2Mode(CHAINHASH_QUADRATIC);
    ds.setChainHash1Iters(CITERS);
    ds.setChainHash2Iters(CITERS);
    Bytes data;
    data.setBytes(RNG::get_random_bytes_large(DATA_SIZE_MEDIUM));

    API api{FILEMODE_PASSWORD};
    std::filesystem::path file = RNG::get_random_string(10) + ".enc";
    api.createFile(file);
    api.selectFile(file);
    api.createDataHeader(password, ds);
    FileDataStruct fds = api.getFileData().returnValue();
    fds.dec_data = data;
    api.getEncryptedData(fds);
    api.writeToFile();
    api.logout();

    std::thread memoryThread(MemoryThread);
    Timer timer;
    timer.start();

    API api2{FILEMODE_PASSWORD};
    api2.selectFile(file);
    api2.verifyPassword(password);
    fds = api2.getDecryptedData().returnValue();
    assert(fds.dec_data == data);
    api2.logout();

    timer.stop();
    _terminateMeasurementThread = true;
    memoryThread.join();
    filing("read_medium_sha256", CITERS, DATA_SIZE_MEDIUM_MB, timer.getAverageTime(), timer.getSlowest());
}