#include <gtest/gtest.h>

#include <fstream>
#include <thread>

#include "bench_utils.h"
#include "blockchain_decrypt.h"
#include "blockchain_encrypt.h"
#include "hash_modes.h"
#include "timer.h"

const constexpr int ITERS = 10;
const constexpr int NUM_BYTES = 1000000;

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

void filing(std::string op, u_int64_t iters, u_int64_t avg, u_int64_t slowest) {
    std::ofstream file;
    file.open("blockchain_bench.csv", std::ios::app);
    file << op << "," << iters << "," << avg << "," << slowest << "," << _memory_avg << "," << _memory_max << "," << _memory_base << "\n";
    file.close();
}

TEST(BlockChain, encrypt_sha256) {
    HModes hmode = HModes::HASHMODE_SHA256;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        EncryptBlockChain ebc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        ebc.addData(data);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("encrypt_sha256", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, encrypt_sha384) {
    HModes hmode = HModes::HASHMODE_SHA384;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        EncryptBlockChain ebc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        ebc.addData(data);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("encrypt_sha384", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, encrypt_sha512) {
    HModes hmode = HModes::HASHMODE_SHA512;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        EncryptBlockChain ebc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        ebc.addData(data);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("encrypt_sha512", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, decrypt_sha256) {
    HModes hmode = HModes::HASHMODE_SHA256;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        DecryptBlockChain dbc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        dbc.addData(data);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("decrypt_sha256", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, decrypt_sha384) {
    HModes hmode = HModes::HASHMODE_SHA384;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        DecryptBlockChain dbc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        dbc.addData(data);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("decrypt_sha384", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, decrypt_sha512) {
    HModes hmode = HModes::HASHMODE_SHA512;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        DecryptBlockChain dbc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        dbc.addData(data);
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("decrypt_sha512", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, codec_sha256) {
    HModes hmode = HModes::HASHMODE_SHA256;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        EncryptBlockChain ebc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        DecryptBlockChain dbc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        ebc.addData(data);
        dbc.addData(ebc.getResult());
        std::unique_ptr<Bytes> res = dbc.getResult();
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("codec_sha256", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, codec_sha384) {
    HModes hmode = HModes::HASHMODE_SHA384;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        EncryptBlockChain ebc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        DecryptBlockChain dbc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        ebc.addData(data);
        dbc.addData(ebc.getResult());
        std::unique_ptr<Bytes> res = dbc.getResult();
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("codec_sha384", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}

TEST(BlockChain, codec_sha512) {
    HModes hmode = HModes::HASHMODE_SHA512;
    std::shared_ptr<Hash> hash = std::move(HashModes::getHash(hmode));
    Bytes pwhash{hash->getHashSize()};
    Bytes enc_salt{hash->getHashSize()};
    Bytes data{NUM_BYTES};
    pwhash.fillrandom();
    enc_salt.fillrandom();
    data.fillrandom();
    Timer timer;

    std::thread memory_thread(MemoryThread);
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        EncryptBlockChain ebc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        DecryptBlockChain dbc{std::move(HashModes::getHash(hmode)), pwhash, enc_salt};
        ebc.addData(data);
        dbc.addData(ebc.getResult());
        std::unique_ptr<Bytes> res = dbc.getResult();
        if (i != ITERS - 1) timer.recordTime();
    }
    timer.stop();
    _terminateMeasurementThread = true;
    memory_thread.join();
    filing("codec_sha512", NUM_BYTES, timer.getAverageTime(), timer.getSlowest());
}