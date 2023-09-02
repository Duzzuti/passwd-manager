#include <gtest/gtest.h>

#include <fstream>

#include "dataheader.h"
#include "timer.h"

const constexpr int ITERS = 10000;
const constexpr int HASHITERS = 1000000;

void filing(std::string op, u_int64_t iters, u_int64_t avg, u_int64_t slowest) {
    std::ofstream file;
    file.open("dataheader_bench.csv", std::ios::app);
    file << op << "," << iters << "," << avg << "," << slowest << "\n";
    file.close();
}

TEST(DataHeader, calcHeaderBytes_sha256) {
    Timer timer;
    std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    chd1->generateRandomData();
    chd2->generateRandomData();
    DataHeader header{HASHMODE_SHA256};
    header.setFileDataMode(FILEMODE_PASSWORD);
    Bytes bytes(32);
    bytes.fillrandom();
    header.setValidPasswordHashBytes(bytes);
    header.setChainHash1(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd1});
    header.setChainHash2(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd2});
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        header.calcHeaderBytes();
    }
    timer.stop();
    filing("calcHeaderBytes_sha256", ITERS, timer.getAverageTime(), timer.getSlowest());
}

TEST(DataHeader, setHeaderBytes_sha256) {
    Timer timer;
    std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    chd1->generateRandomData();
    chd2->generateRandomData();
    DataHeader header{HASHMODE_SHA256};
    header.setFileDataMode(FILEMODE_PASSWORD);
    Bytes bytes(32);
    bytes.fillrandom();
    header.setValidPasswordHashBytes(bytes);
    header.setChainHash1(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd1});
    header.setChainHash2(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd2});
    header.calcHeaderBytes();
    Bytes headerBytes = header.getHeaderBytes();
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        DataHeader header{HASHMODE_SHA256};
        header.setHeaderBytes(headerBytes);
    }
    timer.stop();
    filing("setHeaderBytes_sha256", ITERS, timer.getAverageTime(), timer.getSlowest());
}

TEST(DataHeader, calcHeaderBytes_sha384) {
    Timer timer;
    std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    chd1->generateRandomData();
    chd2->generateRandomData();
    DataHeader header{HASHMODE_SHA384};
    header.setFileDataMode(FILEMODE_PASSWORD);
    Bytes bytes(48);
    bytes.fillrandom();
    header.setValidPasswordHashBytes(bytes);
    header.setChainHash1(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd1});
    header.setChainHash2(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd2});
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        header.calcHeaderBytes();
    }
    timer.stop();
    filing("calcHeaderBytes_sha384", ITERS, timer.getAverageTime(), timer.getSlowest());
}

TEST(DataHeader, setHeaderBytes_sha384) {
    Timer timer;
    std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    chd1->generateRandomData();
    chd2->generateRandomData();
    DataHeader header{HASHMODE_SHA384};
    header.setFileDataMode(FILEMODE_PASSWORD);
    Bytes bytes(48);
    bytes.fillrandom();
    header.setValidPasswordHashBytes(bytes);
    header.setChainHash1(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd1});
    header.setChainHash2(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd2});
    header.calcHeaderBytes();
    Bytes headerBytes = header.getHeaderBytes();
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        DataHeader header{HASHMODE_SHA384};
        header.setHeaderBytes(headerBytes);
    }
    timer.stop();
    filing("setHeaderBytes_sha384", ITERS, timer.getAverageTime(), timer.getSlowest());
}

TEST(DataHeader, calcHeaderBytes_sha512) {
    Timer timer;
    std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    chd1->generateRandomData();
    chd2->generateRandomData();
    DataHeader header{HASHMODE_SHA512};
    header.setFileDataMode(FILEMODE_PASSWORD);
    Bytes bytes(64);
    bytes.fillrandom();
    header.setValidPasswordHashBytes(bytes);
    header.setChainHash1(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd1});
    header.setChainHash2(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd2});
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        header.calcHeaderBytes();
    }
    timer.stop();
    filing("calcHeaderBytes_sha512", ITERS, timer.getAverageTime(), timer.getSlowest());
}

TEST(DataHeader, setHeaderBytes_sha512) {
    Timer timer;
    std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(Format{CHAINHASH_QUADRATIC});
    chd1->generateRandomData();
    chd2->generateRandomData();
    DataHeader header{HASHMODE_SHA512};
    header.setFileDataMode(FILEMODE_PASSWORD);
    Bytes bytes(64);
    bytes.fillrandom();
    header.setValidPasswordHashBytes(bytes);
    header.setChainHash1(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd1});
    header.setChainHash2(ChainHash{CHAINHASH_QUADRATIC, HASHITERS, chd2});
    header.calcHeaderBytes();
    Bytes headerBytes = header.getHeaderBytes();
    timer.start();
    for (int i = 0; i < ITERS; i++) {
        DataHeader header{HASHMODE_SHA512};
        header.setHeaderBytes(headerBytes);
    }
    timer.stop();
    filing("setHeaderBytes_sha512", ITERS, timer.getAverageTime(), timer.getSlowest());
}