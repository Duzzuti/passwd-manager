#include "filehandler.h"

#include <gtest/gtest.h>

#include <sstream>

#include "dataheader_generator.h"
#include "rng.h"

TEST(FileHandlerClass, statics) {
    EXPECT_EQ(FileHandler::extension, ".enc");
    for (int i = 0; i < 10; i++) {
        std::filesystem::path path = RNG::get_random_string(10) + ".enc";
        EXPECT_TRUE(FileHandler::isValidPath(path, false).returnValue());
        EXPECT_TRUE(FileHandler::createFile(path).returnValue());
        EXPECT_TRUE(FileHandler::isValidPath(path, true).returnValue());
    }
}

TEST(FileHandlerClass, basic_empty) {
    // handling basic operations with an empty file
    for (int i = 0; i < 10; i++) {
        Bytes tmp(10);
        tmp.fillrandom();
        std::filesystem::path path = RNG::get_random_string(10) + ".enc";
        FileHandler::createFile(path);
        FileHandler file_handler(path);
        EXPECT_EQ(file_handler.getPath(), path);
        EXPECT_TRUE(file_handler.isEmtpy());
        EXPECT_TRUE(file_handler.getFileStream().is_open());
        EXPECT_TRUE(file_handler.getDataStream().is_open());
        EXPECT_EQ(file_handler.getHeaderSize(), 0);
        EXPECT_EQ(file_handler.getFileSize(), 0);
        EXPECT_THROW(file_handler.getFirstBytes(1), std::length_error);

        // write to file, check if it has been written correctly
        std::ofstream file = file_handler.getWriteStreamIfEmpty().returnMove();
        file << tmp;
        file.close();
        // not updated yet
        EXPECT_TRUE(file_handler.isEmtpy());
        EXPECT_EQ(file_handler.getHeaderSize(), 0);
        EXPECT_EQ(file_handler.getFileSize(), 0);
        EXPECT_EQ(file_handler.getFirstBytes(1), tmp.copySubBytes(0, 1));  // that should work nethertheless
        std::stringstream ss;
        ss << file_handler.getFileStream().rdbuf();
        EXPECT_TRUE(std::memcmp(reinterpret_cast<const unsigned char*>(ss.str().c_str()), tmp.getBytes(), tmp.getLen()) == 0);
        std::stringstream ss2;
        ss2 << file_handler.getDataStream().rdbuf();
        EXPECT_TRUE(std::memcmp(reinterpret_cast<const unsigned char*>(ss2.str().c_str()), tmp.getBytes(), tmp.getLen()) == 0);
        EXPECT_EQ(file_handler.getAllBytes().getLen(), 10);

        // update will lead to invalid dataheader (file size mismatch)
        EXPECT_THROW(file_handler.update(), std::logic_error);
        EXPECT_EQ(file_handler.getHeaderSize(), 0);
        EXPECT_EQ(file_handler.getFileSize(), 10);  // file size should be set anyway
        EXPECT_FALSE(file_handler.isEmtpy());
        EXPECT_FALSE(file_handler.getWriteStreamIfEmpty().isSuccess());
        EXPECT_EQ(file_handler.getAllBytes().getLen(), 10);

        file = file_handler.getWriteStream().returnMove();
        EXPECT_TRUE(file.is_open());
        EXPECT_NO_THROW(file_handler.update());
        EXPECT_EQ(file_handler.getHeaderSize(), 0);
        EXPECT_EQ(file_handler.getFileSize(), 0);  // file size should be reset
        EXPECT_THROW(file_handler.getFirstBytes(1), std::length_error);
        EXPECT_TRUE(file_handler.isEmtpy());
    }
}

TEST(FileHandlerClass, basic_filled) {
    for (int i = 0; i < 10; i++) {
        Bytes tmp = DataHeaderGen::generateDH(DataHeaderGenSet{
            .hashmode = HASHMODE_SHA256,
            .datablocknum = 0,
            .decdatablocknum = 0,
            .chainhashlen1 = 0,
            .chainhashlen2 = 0,
        });
        std::filesystem::path path = RNG::get_random_string(10) + ".enc";
        FileHandler::createFile(path);
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(tmp.getBytes()), tmp.getLen());
        file.close();
        FileHandler file_handler(path);
        EXPECT_EQ(file_handler.getPath(), path);
        EXPECT_FALSE(file_handler.isEmtpy());
        EXPECT_TRUE(file_handler.getFileStream().is_open());
        EXPECT_TRUE(file_handler.getDataStream().is_open());
        EXPECT_EQ(file_handler.getHeaderSize(), MIN_DATAHEADER_LEN);
        EXPECT_EQ(file_handler.getFileSize(), MIN_DATAHEADER_LEN);
        EXPECT_EQ(file_handler.getFirstBytes(1), tmp.copySubBytes(0, 1));
        std::stringstream ss;
        ss << file_handler.getFileStream().rdbuf();
        EXPECT_TRUE(std::memcmp(reinterpret_cast<const unsigned char*>(ss.str().c_str()), tmp.getBytes(), tmp.getLen()) == 0);
        std::stringstream ss2;
        ss2 << file_handler.getDataStream().rdbuf();
        EXPECT_EQ(ss2.str(), "");
        EXPECT_EQ(file_handler.getAllBytes().getLen(), MIN_DATAHEADER_LEN);
        std::unique_ptr<DataHeader> dh = file_handler.getDataHeader().returnMove();
        EXPECT_TRUE(dh->getHeaderBytes() == tmp);
        EXPECT_EQ(dh->getHeaderLength(), MIN_DATAHEADER_LEN);
        EXPECT_EQ(dh->getFileSize(), MIN_DATAHEADER_LEN);
    }
}

TEST(FileHandlerClass, dataheader_simple) {
    for (int i = 0; i < 10; i++) {
        Bytes tmp(64);
        tmp.fillrandom();
        std::filesystem::path path = RNG::get_random_string(10) + ".enc";
        FileHandler::createFile(path);
        DataHeaderParts dhp;
        dhp.setHashMode(HASHMODE_SHA512);
        dhp.setFileDataMode(FILEMODE_PASSWORD);
        dhp.setEncSalt(tmp);
        dhp.setValidPasswordHash(tmp);
        CHModes chm1 = CHAINHASH_CONSTANT_COUNT_SALT;
        CHModes chm2 = CHAINHASH_QUADRATIC;
        std::unique_ptr<ChainHashData> chd1 = std::make_unique<ChainHashData>(Format(chm1));
        std::unique_ptr<ChainHashData> chd2 = std::make_unique<ChainHashData>(Format(chm2));
        chd1->generateRandomData();
        chd2->generateRandomData();
        dhp.chainhash1 = ChainHash(chm1, 1000, std::move(chd1));
        dhp.chainhash2 = ChainHash(chm2, 1000, std::move(chd2));
        EXPECT_TRUE(dhp.isComplete(64));
        std::unique_ptr<DataHeader> dh = DataHeader::setHeaderParts(dhp).returnMove();
        dh->setFileSize(dh->getHeaderLength());
        EXPECT_NO_THROW(dh->calcHeaderBytes());
        FileHandler file_handler(path);
        std::ofstream file = file_handler.getWriteStreamIfEmpty().returnMove();
        file.write(reinterpret_cast<const char*>(dh->getHeaderBytes().getBytes()), dh->getHeaderBytes().getLen());
        file.close();
        EXPECT_NO_THROW(file_handler.update());
        EXPECT_EQ(file_handler.getHeaderSize(), dh->getHeaderLength());
        EXPECT_EQ(file_handler.getFileSize(), dh->getHeaderLength());
        EXPECT_EQ(file_handler.getFirstBytes(1), dh->getHeaderBytes().copySubBytes(0, 1));
        std::stringstream ss;
        ss << file_handler.getFileStream().rdbuf();
        EXPECT_TRUE(std::memcmp(reinterpret_cast<const unsigned char*>(ss.str().c_str()), dh->getHeaderBytes().getBytes(), dh->getHeaderBytes().getLen()) == 0);
        std::stringstream ss2;
        ss2 << file_handler.getDataStream().rdbuf();
        EXPECT_TRUE(ss2.str() == "");
        EXPECT_EQ(file_handler.getAllBytes().getLen(), dh->getHeaderLength());
        EXPECT_TRUE(file_handler.isDataHeader(FILEMODE_PASSWORD).returnValue());
        EXPECT_TRUE(file_handler.getDataHeader().returnMove()->getHeaderBytes() == dh->getHeaderBytes());
    }
}

TEST(FileHandlerClass, complex_filled) {
    for (int i = 0; i < 10; i++) {
        Bytes tmp = DataHeaderGen::generateDH(DataHeaderGenSet{
            .hashmode = HASHMODE_SHA512,
            .datablocknum = 189,
            .decdatablocknum = 54,
            .chainhashlen1 = 154,
            .chainhashlen2 = 1,
        });

        std::filesystem::path path = RNG::get_random_string(10) + ".enc";
        FileHandler::createFile(path);
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(tmp.getBytes()), tmp.getLen());
        file.close();
        FileHandler file_handler(path);
        EXPECT_EQ(file_handler.getPath(), path);
        EXPECT_FALSE(file_handler.isEmtpy());
        EXPECT_TRUE(file_handler.getFileStream().is_open());
        EXPECT_TRUE(file_handler.getDataStream().is_open());
        EXPECT_EQ(file_handler.getHeaderSize(), file_handler.getFileSize());
        EXPECT_GT(file_handler.getFileSize(), MIN_DATAHEADER_LEN);
        EXPECT_EQ(file_handler.getFirstBytes(1), tmp.copySubBytes(0, 1));
        std::stringstream ss;
        ss << file_handler.getFileStream().rdbuf();
        EXPECT_TRUE(std::memcmp(reinterpret_cast<const unsigned char*>(ss.str().c_str()), tmp.getBytes(), tmp.getLen()) == 0);
        std::stringstream ss2;
        ss2 << file_handler.getDataStream().rdbuf();
        EXPECT_EQ(ss2.str(), "");
        EXPECT_EQ(file_handler.getAllBytes().getLen(), file_handler.getHeaderSize());
        std::unique_ptr<DataHeader> dh = file_handler.getDataHeader().returnMove();
        EXPECT_TRUE(dh->getHeaderBytes() == tmp);
        EXPECT_EQ(dh->getHeaderLength(), file_handler.getHeaderSize());
        EXPECT_EQ(dh->getFileSize(), file_handler.getHeaderSize());
    }
}
