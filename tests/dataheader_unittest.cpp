#include "dataheader.h"

#include "chainhash_data.h"
#include "gtest/gtest.h"
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
            Bytes tmp;
            tmp.setBytes(RNG::get_random_bytes(rand_bytes));  // set random iterations
            u_int64_t iters = toLong(tmp);                    // sets random iters
            if (iters > MAX_ITERATIONS) {
                // if the random iterations are bigger than the max iterations, set the max iterations
                iters = MAX_ITERATIONS;
            }
            // init chainhashdata with chainhash mode
            Format format{CHAINHASH_NORMAL};
            if (i == 0 || i == ch_modes.size() - 1) {
                // invalid chainhash mode
                EXPECT_THROW(Format(CHModes(ch_modes[i])), std::invalid_argument);
            } else {
                format = Format{CHModes(ch_modes[i])};
            }
            ChainHashData chd{format};
            std::vector<NameLen> vnl = format.getNameLenList();
            // add random datablock data
            chd.generateRandomData();
            unsigned char len = chd.getLen();  // current used datablock length

            EXPECT_TRUE(chd.isCompletedFormat(format));  // check if the chainhashdata is completed

            // setting up the ChainHash struct
            ChainHash chainhash = ChainHash{CHModes(ch_modes[i]), iters, chd};

            // testing the chainhash modes on the setter
            // init dataheader with random hashmode
            HModes hash_mode = HModes(RNG::get_random_byte(1, MAX_HASHMODE_NUMBER));
            Hash* hash = HashModes::getHash(hash_mode);
            unsigned char hash_size = hash->getHashSize();
            delete hash;
            DataHeader dh{hash_mode};   // testing chainhash1
            DataHeader dh2{hash_mode};  // testing chainhash2
            if (i == 0 || i == ch_modes.size() - 1) {
                // invalid chainhash mode
                EXPECT_THROW(dh.setChainHash1(chainhash, len), std::invalid_argument);
                EXPECT_THROW(dh2.setChainHash2(chainhash, len), std::invalid_argument);
            } else {
                // valid chainhash mode
                EXPECT_NO_THROW(dh.setChainHash1(chainhash, len));
                // checking if the setter returns void
                EXPECT_EQ(typeid(void), typeid(DataHeader(HModes(hash_mode)).setChainHash1(chainhash, len)));
                EXPECT_EQ(typeid(void), typeid(DataHeader(HModes(hash_mode)).setChainHash2(chainhash, len)));

                // tests on the changed object
                EXPECT_THROW(dh.getDataHeaderParts(), std::logic_error);               // dataheader is not completed
                EXPECT_EQ(0, dh.getHeaderLength());                                    // chainhash2 is not set, therefore no length can be computed
                EXPECT_THROW(dh.getHeaderBytes(), std::logic_error);                   // dataheader is not completed
                EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error);  // dataheader is not completed

                // chainhash2
                // valid chainhash mode
                EXPECT_NO_THROW(dh2.setChainHash2(chainhash, len));
                // tests on the changed object
                EXPECT_THROW(dh2.getDataHeaderParts(), std::logic_error);               // dataheader is not completed
                EXPECT_EQ(0, dh2.getHeaderLength());                                    // chainhash1 is not set, therefore no length can be computed
                EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error);                   // dataheader is not completed
                EXPECT_THROW(dh2.calcHeaderBytes(Bytes(hash_size)), std::logic_error);  // dataheader is not completed

                // setting both chainhashes
                EXPECT_NO_THROW(dh.setChainHash2(chainhash, len));
                EXPECT_NO_THROW(dh2.setChainHash1(chainhash, len));
                // tests on the changed object
                EXPECT_THROW(dh2.getDataHeaderParts(), std::logic_error);                 // dataheader is not completed
                EXPECT_EQ(22 + 2 * hash_size + 2 * chd.getLen(), dh2.getHeaderLength());  // both chainhashes are set, therefore a length can be computed
                EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error);                     // dataheader is not completed
                EXPECT_THROW(dh2.calcHeaderBytes(Bytes(hash_size)), std::logic_error);    // dataheader is not completed
            }
            // some edge cases
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(ch_modes[i] - 255), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(ch_modes[i] - 256), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(0), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash1(ChainHash{CHModes(-1), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(ch_modes[i] - 255), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(ch_modes[i] - 256), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(0), iters, chd}, len), std::invalid_argument);
            EXPECT_THROW(DataHeader(HModes(STANDARD_HASHMODE)).setChainHash2(ChainHash{CHModes(-1), iters, chd}, len), std::invalid_argument);
        }
    }
}

TEST(DataHeaderClass, setValidPasswordHashBytes) {
    // testing the setter for the password validator hash
    for (unsigned char hash_mode = 1; hash_mode < MAX_HASHMODE_NUMBER; hash_mode++) {
        DataHeader dh{HModes(hash_mode)};  // init dataheader with every possible hashmode

        // getting the hashsize
        Hash* hash = HashModes::getHash(HModes(hash_mode));
        unsigned char hash_size = hash->getHashSize();
        delete hash;

        // checking if the setter returns void
        EXPECT_EQ(typeid(void), typeid(DataHeader{HModes(hash_mode)}.setValidPasswordHashBytes(Bytes(hash_size))));

        EXPECT_NO_THROW(dh.setValidPasswordHashBytes(Bytes(hash_size)));  // valid hashsize
        // tests on the changed object
        EXPECT_THROW(dh.getDataHeaderParts(), std::logic_error);               // dataheader is not completed
        EXPECT_EQ(0, dh.getHeaderLength());                                    // chainhashes are not set, therefore no length can be computed
        EXPECT_THROW(dh.getHeaderBytes(), std::logic_error);                   // dataheader is not completed
        EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error);  // dataheader is not completed

        // invalid hashsize
        for (unsigned int i = 0; i < 512; i++) {
            if (i != hash_size) {
                EXPECT_THROW(DataHeader{HModes(hash_mode)}.setValidPasswordHashBytes(Bytes(i)), std::length_error);
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
            Hash* hash = HashModes::getHash(HModes(hash_mode));
            unsigned char hash_size = hash->getHashSize();

            Bytes tmp;
            // generating random chainhash modes and iterations
            CHModes ch1_mode = CHModes(RNG::get_random_byte(1, MAX_CHAINHASHMODE_NUMBER));
            Format format1{ch1_mode};
            CHModes ch2_mode = CHModes(RNG::get_random_byte(1, MAX_CHAINHASHMODE_NUMBER));
            Format format2{ch2_mode};
            tmp.setBytes(RNG::get_random_bytes(getLongLen(TEST_DH_MAX_PW_ITERS - 1)));
            u_int64_t iters1 = toLong(tmp);  // random iterations
            tmp.setBytes(RNG::get_random_bytes(getLongLen(TEST_DH_MAX_PW_ITERS - 1)));
            u_int64_t iters2 = toLong(tmp);  // random iterations
            // culling the iterations to the maximum
            if (iters1 > TEST_DH_MAX_PW_ITERS) {
                iters1 = TEST_DH_MAX_PW_ITERS;
            }
            if (iters2 > TEST_DH_MAX_PW_ITERS) {
                iters2 = TEST_DH_MAX_PW_ITERS;
            }

            FModes file_mode = FModes(RNG::get_random_byte(1, MAX_FILEMODE_NUMBER));

            ChainHashData chd1{format1};
            ChainHashData chd2{format2};

            // setting the chainhash data parts into the datablock
            chd1.generateRandomData();
            unsigned int len1 = chd1.getLen();
            chd2.generateRandomData();
            unsigned int len2 = chd2.getLen();

            EXPECT_TRUE(chd1.isCompletedFormat(format1));  // checking if the chainhashdata is completed
            EXPECT_TRUE(chd2.isCompletedFormat(format2));  // checking if the chainhashdata is completed
            // setting up the ChainHash struct
            ChainHash chainhash1 = ChainHash{ch1_mode, iters1, chd1};
            ChainHash chainhash2 = ChainHash{ch2_mode, iters2, chd2};

            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error);

            EXPECT_NO_THROW(dh.setFileDataMode(file_mode));

            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error);

            EXPECT_NO_THROW(dh.setChainHash1(chainhash1, len1));
            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error);

            EXPECT_NO_THROW(dh.setChainHash2(chainhash2, len2));
            // dataheader is not completed
            EXPECT_THROW(dh.calcHeaderBytes(Bytes(hash_size)), std::logic_error);

            DataHeader dh2{dh};
            DataHeader dh3{dh};
            DataHeader dh4{dh};

            // generating random password and passwordhash
            std::string password = charVecToString(RNG::get_random_bytes(16));  // random password with 16 characters
            Bytes phash = ChainHashModes::performChainHash(chainhash1, hash, password).returnValue();
            Bytes pval = ChainHashModes::performChainHash(chainhash2, hash, phash).returnValue();
            delete hash;

            // wrong valid password hash validator and passwordhash
            EXPECT_NO_THROW(dh2.setValidPasswordHashBytes(Bytes(hash_size)));
            EXPECT_THROW(dh2.calcHeaderBytes(Bytes(hash_size)), std::logic_error);

            // valid password hash validator and wrong passwordhash
            EXPECT_NO_THROW(dh3.setValidPasswordHashBytes(pval));
            EXPECT_THROW(dh2.calcHeaderBytes(Bytes(hash_size)), std::logic_error);

            // wrong password hash validator and valid passwordhash
            EXPECT_NO_THROW(dh4.setValidPasswordHashBytes(Bytes(hash_size)));
            EXPECT_THROW(dh4.calcHeaderBytes(phash), std::logic_error);

            // valid password hash validator and valid passwordhash
            EXPECT_NO_THROW(dh.setValidPasswordHashBytes(pval));
            EXPECT_EQ(typeid(void), typeid(dh.calcHeaderBytes(phash)));
            EXPECT_NO_THROW(dh.calcHeaderBytes(phash));

            // stores the valid data
            DataHeaderParts dp;
            Bytes dataheaderbytes;

            EXPECT_THROW(dh2.getHeaderBytes(), std::logic_error);                // dataheader is invalid
            EXPECT_THROW(dh3.getHeaderBytes(), std::logic_error);                // dataheader is invalid
            EXPECT_THROW(dh4.getHeaderBytes(), std::logic_error);                // dataheader is invalid
            EXPECT_NO_THROW(dataheaderbytes = dh.getHeaderBytes());              // dataheader is valid
            EXPECT_NO_THROW(dp = dh.getDataHeaderParts());                       // dataheader is valid
            EXPECT_EQ(22 + 2 * hash_size + len1 + len2, dh.getHeaderLength());   // chainhashes are set, therefore length can be computed
            EXPECT_EQ(22 + 2 * hash_size + len1 + len2, dh2.getHeaderLength());  // chainhashes are set, therefore length can be computed
            EXPECT_EQ(22 + 2 * hash_size + len1 + len2, dh3.getHeaderLength());  // chainhashes are set, therefore length can be computed
            EXPECT_EQ(22 + 2 * hash_size + len1 + len2, dh4.getHeaderLength());  // chainhashes are set, therefore length can be computed

            // testing the returned dataparts
            EXPECT_EQ(HModes(hash_mode), dp.hash_mode);
            EXPECT_EQ(file_mode, dp.file_mode);
            EXPECT_EQ(ch1_mode, dp.chainhash1.mode);
            EXPECT_EQ(ch2_mode, dp.chainhash2.mode);
            EXPECT_EQ(iters1, dp.chainhash1.iters);
            EXPECT_EQ(iters2, dp.chainhash2.iters);
            EXPECT_EQ(len1, dp.chainhash1_datablock_len);
            EXPECT_EQ(len2, dp.chainhash2_datablock_len);
            EXPECT_EQ(chd1, dp.chainhash1.datablock);
            EXPECT_EQ(chd2, dp.chainhash2.datablock);
            EXPECT_EQ(pval, dp.valid_passwordhash);

            // testing the returned dataheaderbytes
            EXPECT_EQ(22 + 2 * hash_size + len1 + len2, dataheaderbytes.getLen());
            EXPECT_EQ(file_mode, FModes(dataheaderbytes.popFirstBytes(1).value().getBytes()[0]));
            EXPECT_EQ(hash_mode, HModes(dataheaderbytes.popFirstBytes(1).value().getBytes()[0]));
            EXPECT_EQ(ch1_mode, CHModes(dataheaderbytes.popFirstBytes(1).value().getBytes()[0]));
            EXPECT_EQ(iters1, toLong(dataheaderbytes.popFirstBytes(8).value()));
            EXPECT_EQ(len1, dataheaderbytes.popFirstBytes(1).value().getBytes()[0]);
            EXPECT_EQ(chd1.getDataBlock(), dataheaderbytes.popFirstBytes(len1).value());
            EXPECT_EQ(ch2_mode, CHModes(dataheaderbytes.popFirstBytes(1).value().getBytes()[0]));
            EXPECT_EQ(iters2, toLong(dataheaderbytes.popFirstBytes(8).value()));
            EXPECT_EQ(len2, dataheaderbytes.popFirstBytes(1).value().getBytes()[0]);
            EXPECT_EQ(chd2.getDataBlock(), dataheaderbytes.popFirstBytes(len2).value());
            EXPECT_EQ(pval, dataheaderbytes.popFirstBytes(hash_size).value());
            EXPECT_EQ(dp.enc_salt, dataheaderbytes.popFirstBytes(hash_size).value());
            EXPECT_EQ(0, dataheaderbytes.getLen());

            // testing some edge cases
            // setting all data except valid hash or file mode
            DataHeader dh5{HModes(hash_mode)};
            EXPECT_NO_THROW(dh5.setChainHash1(chainhash1, len1));
            EXPECT_NO_THROW(dh5.setChainHash2(chainhash2, len2));
            DataHeader dh6{dh5};
            EXPECT_NO_THROW(dh5.setFileDataMode(file_mode));
            EXPECT_THROW(dh5.calcHeaderBytes(Bytes(hash_size)), std::logic_error);
            EXPECT_NO_THROW(dh6.setValidPasswordHashBytes(pval));
            EXPECT_THROW(dh6.calcHeaderBytes(phash), std::logic_error);
        }
    }
}
