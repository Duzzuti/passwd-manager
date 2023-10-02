#include "dataheader.h"

#include <gtest/gtest.h>

#include "chainhash_data.h"
#include "dataheader_generator.h"
#include "rng.h"
#include "settings.h"
#include "test_settings.cpp"
#include "utility.h"

TEST(DataHeaderClass, Constructor) {
    // testing the dataheader constructor
    // creating a list of hash modes, where the first (0) and the last (MAX_HASHMODE +1) are invalid
    std::vector<unsigned char> hash_modes;
    std::vector<unsigned char> hash_sizes = {32, 48, 64};
    for (int i = 0; i <= MAX_HASHMODE_NUMBER + 1; i++) {
        hash_modes.push_back(i);
    }
    // testing the hash modes on the constructor
    for (int i = 0; i < hash_modes.size(); i++) {
        // throw testing
        if (i == 0 || i == hash_modes.size() - 1) {
            EXPECT_THROW(DataHeader(HModes(hash_modes[i])), std::invalid_argument);
        } else {
            EXPECT_NO_THROW(DataHeader(HModes(hash_modes[i])));
            EXPECT_EQ(hash_sizes[i - 1], DataHeader(HModes(hash_modes[i])).getHashSize());
        }
    }
    // some edge cases
    EXPECT_THROW(DataHeader(HModes(255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(256)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(257)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(-1)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(-255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(-256)), std::invalid_argument);
}

TEST(DataHeaderClass, setFileDataMode) {
    // testing the set file data mode from the dataheader
    // creating a list of file modes, where the first (0) and the last (MAX_FILEMODE +1) are invalid
    std::vector<unsigned char> file_modes;
    for (int i = 0; i <= MAX_FILEMODE_NUMBER + 1; i++) {
        file_modes.push_back(i);
    }

    // testing the file modes on the setter
    for (int i = 0; i < file_modes.size(); i++) {
        // throw testing
        if (i == 0 || i == file_modes.size() - 1) {
            // invalid file mode
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(file_modes[i])), std::invalid_argument);
        } else {
            // valid file mode
            EXPECT_NO_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(file_modes[i])));
            // checking if the setter returns void
            EXPECT_EQ(typeid(void), typeid(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(file_modes[i]))));
        }
    }
    // some edge cases
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(256)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(257)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(-1)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(-255)), std::invalid_argument);
    EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setFileDataMode(FModes(-256)), std::invalid_argument);
}

TEST(DataHeaderClass, setChainHash) {
    // testing the chainhash setters from the dataheader
    // creating a list of chainhash modes, where the first (0) and the last (MAX_CHAINHASHMODE +1) are invalid
    std::vector<unsigned char> ch_modes;
    for (int i = 0; i <= MAX_CHAINHASHMODE_NUMBER + 1; i++) {
        ch_modes.push_back(i);
    }
    // calculate the bytes needed for the max iteration (to get all valid iterations possible)
    unsigned char rand_bytes = std::min<unsigned char>(8, getLongLen(MAX_ITERATIONS - 1));

    // testing the chainhash modes with different iterations and datablocks
    for (int i = 0; i < ch_modes.size(); i++) {
        for (int j = 0; j < TEST_DH_ITERS; j++) {
            Bytes tmp(rand_bytes);
            tmp.fillrandom();                // set random iterations
            u_int64_t iters = tmp.toLong();  // sets random iters
            if (iters > MAX_ITERATIONS) {
                // if the random iterations are bigger than the max iterations, set the max iterations
                iters = MAX_ITERATIONS;
            }
            // init chainhashdata with chainhash mode
            std::shared_ptr<Format> format = nullptr;
            if (i == 0 || i == ch_modes.size() - 1) {
                // invalid chainhash mode
                EXPECT_THROW(Format(CHModes(ch_modes[i])), std::invalid_argument);
                continue;
            } else {
                format = std::make_shared<Format>(CHModes(ch_modes[i]));
            }
            std::shared_ptr<ChainHashData> chd = std::make_shared<ChainHashData>(*format);
            std::vector<NameLen> vnl = format->getNameLenList();
            // add random datablock data
            chd->generateRandomData();

            EXPECT_TRUE(chd->isCompletedFormat(*format));  // check if the chainhashdata is completed

            // setting up the ChainHash struct
            ChainHash chainhash{CHModes(ch_modes[i]), iters, chd};

            // testing the chainhash modes on the setter
            // init dataheader with random hashmode
            HModes hash_mode = HModes(RNG::get_random_byte(1, MAX_HASHMODE_NUMBER));
            std::unique_ptr<Hash> hash = std::move(HashModes::getHash(hash_mode));
            unsigned char hash_size = hash->getHashSize();
            DataHeader dh{hash_mode};   // testing chainhash1
            DataHeader dh2{hash_mode};  // testing chainhash2
            // valid chainhash mode
            EXPECT_NO_THROW(dh.setChainHash1(chainhash));
            // checking if the setter returns void
            EXPECT_EQ(typeid(void), typeid(DataHeader(HModes(hash_mode)).setChainHash1(chainhash)));
            EXPECT_EQ(typeid(void), typeid(DataHeader(HModes(hash_mode)).setChainHash2(chainhash)));

            Bytes hsb(hash_size);
            hsb.fillrandom();
            // tests on the changed object
            EXPECT_THROW(dh.getDataHeaderParts(), std::logic_error);  // dataheader is not completed
            EXPECT_EQ(0, dh.getHeaderLength());                       // chainhash2 is not set, therefore no length can be computed
            EXPECT_THROW(dh.getHeaderBytes(), std::logic_error);      // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(hsb), std::logic_error);  // dataheader is not completed

            // chainhash2
            // valid chainhash mode
            EXPECT_NO_THROW(dh2.setChainHash2(chainhash));
            // tests on the changed object
            EXPECT_THROW(dh2.getDataHeaderParts(), std::logic_error);  // dataheader is not completed
            EXPECT_EQ(0, dh2.getHeaderLength());                       // chainhash1 is not set, therefore no length can be computed
            EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error);      // dataheader is not completed
            EXPECT_THROW(dh2.calcHeaderBytes(hsb), std::logic_error);  // dataheader is not completed

            // setting both chainhashes
            EXPECT_NO_THROW(dh.setChainHash2(chainhash));
            EXPECT_NO_THROW(dh2.setChainHash1(chainhash));
            // tests on the changed object
            EXPECT_THROW(dh2.getDataHeaderParts(), std::logic_error);                  // dataheader is not completed
            EXPECT_EQ(40 + 2 * hash_size + 2 * chd->getLen(), dh2.getHeaderLength());  // both chainhashes are set, therefore a length can be computed
            EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error);                      // dataheader is not completed
            EXPECT_THROW(dh2.calcHeaderBytes(hsb), std::logic_error);                  // dataheader is not completed
            // some edge cases
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(ch_modes[i] - 255), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(ch_modes[i] - 256), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(0), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(-1), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(ch_modes[i] - 255), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(ch_modes[i] - 256), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(0), iters, chd}), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(-1), iters, chd}), std::invalid_argument);
        }
    }
}

TEST(DataHeaderClass, setValidPasswordHashBytes) {
    // testing the setter for the password validator hash
    for (unsigned char hash_mode = 1; hash_mode < MAX_HASHMODE_NUMBER; hash_mode++) {
        DataHeader dh{HModes(hash_mode)};  // init dataheader with every possible hashmode

        // getting the hashsize
        std::unique_ptr<Hash> hash = std::move(HashModes::getHash(HModes(hash_mode)));
        unsigned char hash_size = hash->getHashSize();

        // checking if the setter returns void
        Bytes tmp(hash_size);
        tmp.fillrandom();  // random password hash validator
        EXPECT_EQ(typeid(void), typeid(DataHeader{HModes(hash_mode)}.setValidPasswordHashBytes(tmp)));

        EXPECT_NO_THROW(dh.setValidPasswordHashBytes(tmp));  // valid hashsize
        // tests on the changed object
        EXPECT_THROW(dh.getDataHeaderParts(), std::logic_error);  // dataheader is not completed
        EXPECT_EQ(0, dh.getHeaderLength());                       // chainhashes are not set, therefore no length can be computed
        EXPECT_THROW(dh.getHeaderBytes(), std::logic_error);      // dataheader is not completed
        EXPECT_THROW(dh.calcHeaderBytes(tmp), std::logic_error);  // dataheader is not completed

        // invalid hashsize
        for (u_int16_t i = 0; i < 512; i++) {
            if (i != hash_size) {
                Bytes tmp2(i);
                tmp2.fillrandom();
                EXPECT_THROW(DataHeader{HModes(hash_mode)}.setValidPasswordHashBytes(tmp2), std::invalid_argument);
            }
        }
    }
}

TEST(DataHeaderClass, calcHeaderBytes) {
    // testing the calcHeaderBytes function
    EXPECT_GE(MAX_ITERATIONS, TEST_DH_MAX_PW_ITERS);
    for (unsigned char hash_mode = 1; hash_mode < MAX_HASHMODE_NUMBER; hash_mode++) {
        for (u_int64_t k = 0; k < TEST_DH_CALC_ITERS; k++) {
            DataHeader dh{HModes(hash_mode)};  // init dataheader with every possible hashmode

            // getting the hashsize
            std::shared_ptr<Hash> hash = std::move(HashModes::getHash(HModes(hash_mode)));
            unsigned char hash_size = hash->getHashSize();

            Bytes tmp(getLongLen(TEST_DH_MAX_PW_ITERS - 1));
            Bytes tmp2(getLongLen(TEST_DH_MAX_PW_ITERS - 1));
            tmp.fillrandom();
            tmp2.fillrandom();
            // generating random chainhash modes and iterations
            CHModes ch1_mode = CHModes(RNG::get_random_byte(1, MAX_CHAINHASHMODE_NUMBER));
            Format format1{ch1_mode};
            CHModes ch2_mode = CHModes(RNG::get_random_byte(1, MAX_CHAINHASHMODE_NUMBER));
            Format format2{ch2_mode};
            u_int64_t iters1 = tmp.toLong();   // random iterations
            u_int64_t iters2 = tmp2.toLong();  // random iterations
            // culling the iterations to the maximum
            if (iters1 > TEST_DH_MAX_PW_ITERS) {
                iters1 = TEST_DH_MAX_PW_ITERS;
            }
            if (iters2 > TEST_DH_MAX_PW_ITERS) {
                iters2 = TEST_DH_MAX_PW_ITERS;
            }

            FModes file_mode = FModes(RNG::get_random_byte(1, MAX_FILEMODE_NUMBER));

            std::shared_ptr<ChainHashData> chd1 = std::make_shared<ChainHashData>(format1);
            std::shared_ptr<ChainHashData> chd2 = std::make_shared<ChainHashData>(format2);

            // setting the chainhash data parts into the datablock
            chd1->generateRandomData();
            chd2->generateRandomData();

            EXPECT_TRUE(chd1->isCompletedFormat(format1));  // checking if the chainhashdata is completed
            EXPECT_TRUE(chd2->isCompletedFormat(format2));  // checking if the chainhashdata is completed
            // setting up the ChainHash struct
            ChainHash chainhash1 = ChainHash{ch1_mode, iters1, chd1};
            ChainHash chainhash2 = ChainHash{ch2_mode, iters2, chd2};

            // dataheader is not completed
            Bytes hsb(hash_size);
            hsb.fillrandom();
            EXPECT_THROW(dh.calcHeaderBytes(hsb), std::logic_error);

            EXPECT_NO_THROW(dh.setFileDataMode(file_mode));

            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(hsb), std::logic_error);

            EXPECT_NO_THROW(dh.setChainHash1(chainhash1));
            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(hsb), std::logic_error);

            EXPECT_NO_THROW(dh.setChainHash2(chainhash2));
            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(hsb), std::logic_error);

            DataHeader dh2{dh};
            DataHeader dh3{dh};
            DataHeader dh4{dh};

            // generating random password and passwordhash
            std::string password = RNG::get_random_string(16);  // random password with 16 characters
            Bytes phash = ChainHashModes::performChainHash(chainhash1, hash, password).returnValue();
            Bytes pval = ChainHashModes::performChainHash(chainhash2, hash, phash).returnValue();

            // wrong valid password hash validator and passwordhash
            RNG::fill_random_bytes(hsb, hash_size);
            EXPECT_NO_THROW(dh2.setValidPasswordHashBytes(hsb));
            RNG::fill_random_bytes(hsb, hash_size);
            EXPECT_THROW(dh2.calcHeaderBytes(hsb), std::logic_error);

            // valid password hash validator and wrong passwordhash
            EXPECT_NO_THROW(dh3.setValidPasswordHashBytes(pval));
            RNG::fill_random_bytes(hsb, hash_size);
            EXPECT_THROW(dh2.calcHeaderBytes(hsb), std::logic_error);

            // wrong password hash validator and valid passwordhash
            RNG::fill_random_bytes(hsb, hash_size);
            EXPECT_NO_THROW(dh4.setValidPasswordHashBytes(hsb));
            EXPECT_THROW(dh4.calcHeaderBytes(phash), std::logic_error);

            // valid password hash validator and valid passwordhash
            EXPECT_NO_THROW(dh.setValidPasswordHashBytes(pval));
            EXPECT_THROW(dh.calcHeaderBytes(phash), std::logic_error);

            EXPECT_NO_THROW(dh.setFileSize(100000));
            EXPECT_EQ(typeid(void), typeid(dh.calcHeaderBytes(phash)));
            EXPECT_NO_THROW(dh.calcHeaderBytes(phash));

            // stores the valid data
            DataHeaderParts dp;
            Bytes dataheaderbytes(0);

            EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error);                                    // dataheader is invalid
            EXPECT_THROW(dh3.getHeaderBytes(), std::logic_error);                                    // dataheader is invalid
            EXPECT_THROW(dh4.getHeaderBytes(), std::logic_error);                                    // dataheader is invalid
            EXPECT_NO_THROW(dataheaderbytes = dh.getHeaderBytes());                                  // dataheader is valid
            EXPECT_NO_THROW(dp = dh.getDataHeaderParts());                                           // dataheader is valid
            EXPECT_EQ(40 + 2 * hash_size + chd1->getLen() + chd2->getLen(), dh.getHeaderLength());   // chainhashes are set, therefore length can be computed
            EXPECT_EQ(40 + 2 * hash_size + chd1->getLen() + chd2->getLen(), dh2.getHeaderLength());  // chainhashes are set, therefore length can be computed
            EXPECT_EQ(40 + 2 * hash_size + chd1->getLen() + chd2->getLen(), dh3.getHeaderLength());  // chainhashes are set, therefore length can be computed
            EXPECT_EQ(40 + 2 * hash_size + chd1->getLen() + chd2->getLen(), dh4.getHeaderLength());  // chainhashes are set, therefore length can be computed

            // testing the returned dataparts
            EXPECT_EQ(HModes(hash_mode), dp.getHashMode());
            EXPECT_EQ(file_mode, dp.getFileDataMode());
            EXPECT_EQ(ch1_mode, dp.chainhash1.getMode());
            EXPECT_EQ(ch2_mode, dp.chainhash2.getMode());
            EXPECT_EQ(iters1, dp.chainhash1.getIters());
            EXPECT_EQ(iters2, dp.chainhash2.getIters());
            EXPECT_EQ(chd1->getLen(), dp.chainhash1.getChainHashData()->getLen());
            EXPECT_EQ(chd2->getLen(), dp.chainhash2.getChainHashData()->getLen());
            EXPECT_EQ(chd1, dp.chainhash1.getChainHashData());
            EXPECT_EQ(chd2, dp.chainhash2.getChainHashData());
            EXPECT_EQ(pval, dp.getValidPasswordHash());

            // testing the returned dataheaderbytes
            EXPECT_EQ(40 + 2 * hash_size + chd1->getLen() + chd2->getLen(), dataheaderbytes.getLen());
            EXPECT_EQ(100000, dataheaderbytes.copySubBytes(0, 8).toLong());
            EXPECT_EQ(dataheaderbytes.getLen(), dataheaderbytes.copySubBytes(8, 16).toLong());
            EXPECT_EQ(file_mode, FModes(dataheaderbytes.copySubBytes(16, 17).getBytes()[0]));
            EXPECT_EQ(hash_mode, HModes(dataheaderbytes.copySubBytes(17, 18).getBytes()[0]));
            EXPECT_EQ(0, dataheaderbytes.copySubBytes(18, 19).getBytes()[0]);
            EXPECT_EQ(ch1_mode, CHModes(dataheaderbytes.copySubBytes(19, 20).getBytes()[0]));
            EXPECT_EQ(iters1, dataheaderbytes.copySubBytes(20, 28).toLong());
            EXPECT_EQ(chd1->getLen(), dataheaderbytes.copySubBytes(28, 29).getBytes()[0]);
            EXPECT_EQ(chd1->getDataBlock(), dataheaderbytes.copySubBytes(29, 29 + chd1->getLen()));
            EXPECT_EQ(ch2_mode, CHModes(dataheaderbytes.copySubBytes(29 + chd1->getLen(), 30 + chd1->getLen()).getBytes()[0]));
            EXPECT_EQ(iters2, dataheaderbytes.copySubBytes(30 + chd1->getLen(), 38 + chd1->getLen()).toLong());
            EXPECT_EQ(chd2->getLen(), dataheaderbytes.copySubBytes(38 + chd1->getLen(), 39 + chd1->getLen()).getBytes()[0]);
            EXPECT_EQ(chd2->getDataBlock(), dataheaderbytes.copySubBytes(39 + chd1->getLen(), 39 + chd1->getLen() + chd2->getLen()));
            EXPECT_EQ(pval, dataheaderbytes.copySubBytes(39 + chd1->getLen() + chd2->getLen(), 39 + chd1->getLen() + chd2->getLen() + hash_size));
            EXPECT_EQ(dp.getEncSalt(), dataheaderbytes.copySubBytes(39 + chd1->getLen() + chd2->getLen() + hash_size, 39 + chd1->getLen() + chd2->getLen() + 2 * hash_size));
            EXPECT_EQ(0, dataheaderbytes.copySubBytes(39 + chd1->getLen() + chd2->getLen() + 2 * hash_size, 40 + chd1->getLen() + chd2->getLen() + 2 * hash_size).getBytes()[0]);

            // testing some edge cases
            // setting all data except valid hash or file mode
            DataHeader dh5{HModes(hash_mode)};
            EXPECT_NO_THROW(dh5.setChainHash1(chainhash1));
            EXPECT_NO_THROW(dh5.setChainHash2(chainhash2));
            DataHeader dh6{dh5};
            EXPECT_NO_THROW(dh5.setFileDataMode(file_mode));
            RNG::fill_random_bytes(hsb, hash_size);
            EXPECT_THROW(dh5.calcHeaderBytes(hsb), std::logic_error);
            EXPECT_NO_THROW(dh6.setValidPasswordHashBytes(pval));
            EXPECT_THROW(dh6.calcHeaderBytes(phash), std::logic_error);
        }
    }
}

TEST(DataHeaderClass, datablocks){
    for(int k = 0; k < 100; k++){
        std::vector<DataBlock> db;
        std::vector<EncDataBlock> ddb;
        HModes hashmode = HModes(RNG::get_random_byte(1, MAX_HASHMODE_NUMBER));
        for(int i = 0; i < RNG::get_random_byte(); i++){
            Bytes tmp(RNG::get_random_byte(1));
            tmp.fillrandom();
            db.push_back(DataBlock(DatablockType(RNG::get_random_byte()), tmp));
        }
        for(int j = 0; j < RNG::get_random_byte(); j++){
            Bytes tmp(255);
            tmp.fillrandom();
            ddb.push_back(EncDataBlock::createEncBlock(DatablockType(RNG::get_random_byte()), tmp, RNG::get_random_byte()));
        }
        Bytes dhb = DataHeaderGen::generateDH(DataHeaderGenSet{
            .hashmode = hashmode,
            .datablocknum = 0,
            .decdatablocknum = 0,
            .chainhashlen1 = 0,
            .chainhashlen2 = 0,
        });
        std::unique_ptr<DataHeader> dh = DataHeader::setHeaderBytes(dhb).returnMove();
        for(int i = 0; i < db.size(); i++){
            dh->addDataBlock(db[i]);
        }
        for(int i = 0; i < ddb.size(); i++){
            dh->addEncDataBlock(ddb[i]);
        }
        dh->setFileSize(100000);
        dh->calcHeaderBytes();
        Bytes header = dh->getHeaderBytes();
        std::unique_ptr<DataHeader> dh2 = DataHeader::setHeaderBytes(header).returnMove();
        dh2->calcHeaderBytes();
        EXPECT_EQ(dh->getHeaderBytes(), dh2->getHeaderBytes());
    }
}
