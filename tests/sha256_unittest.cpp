#include "sha256.h"

#include <gtest/gtest.h>

#include <openssl/sha.h>

#include "rng.h"
#include "test_settings.cpp"
#include "utility.h"

TEST(SHA256Class, returnTypes) {
    // testing the return types of the sha256 class
    sha256 shaObj = sha256();
    EXPECT_EQ(SHA256_DIGEST_LENGTH, shaObj.getHashSize());
    EXPECT_EQ(typeid(int), typeid(shaObj.getHashSize()));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash(Bytes(10))));
    Bytes b(10);
    b.fillrandom();
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash(b)));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash("")));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash("sadfasd .-fsa")));
}

TEST(SHA256Class, att_strings) {
    // testing the sha256 function on strings
    std::vector<Bytes> hashv = std::vector<Bytes>();
    sha256 shaObj = sha256();
    for (int len = 4; len < TEST_HASH_MAX_LEN; len++) {
        for (u_int64_t num = 0; num < TEST_HASH_ITERS; num++) {
            std::string tmpstr = RNG::get_random_string(len);
            Bytes tmp = shaObj.hash(tmpstr);
            EXPECT_EQ(tmp, shaObj.hash(tmpstr));
            hashv.push_back(tmp);
        }
        //(hashv.end()-1)->print();
    }
    /*
    expectations:
    1. no collisions
    2. 32 Bytes len
    */
    for (Bytes b : hashv) {
        EXPECT_EQ(shaObj.getHashSize(), b.getLen());
        // may be the very unlikely case where two same strings get randomly generated (test fails in this case)
        EXPECT_EQ(1, std::count(hashv.begin(), hashv.end(), b));
    }
}

TEST(SHA256Class, exact_strings) {
    // testing the sha256 function on exact strings that are gotten from a approved sha256 function
    std::vector<std::string> strings = {"jad", "", "z", "opjwdofiasasdf", "iou32894e934zh83", "öül34.ö23-42,.34,-23.4m23oi4z239o4hz239847z2", "asoizdfh8790qazf9up984u89auwrfsaf fjas pflsa .,4nmrt4"};
    std::vector<std::string> hashs = {"65ebae8e077a037d81231c1bde91b721bf23bc3dd92baebdcdbe295ade66b79f", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
                                      "594e519ae499312b29433b7dd8a97ff068defcba9755b6d5d00e84c524d67b06", "794f3b75cb2279f380c2db0f749187b2d7df799a2d17c7060381cc1a24c5acf7",
                                      "1ae020b3a284e8f8e4348692ec56d39b5776cc726366ad24d8e40f44ebce3253", "a5cb38e244f7a4e2ff2f0290be1a30ef587c59fb1b08ec01cf9a84f11dd6eb28",
                                      "29ed1592959b390a8eb22cc95af855e016c4afe9583c7788365274cc09de6bd0"};
    for (int i = 0; i < strings.size(); i++) {
        EXPECT_EQ(sha256().hash(strings[i]), sha256().hash(strings[i]));
        for (auto& c : hashs[i]) c = toupper(c);
        EXPECT_EQ(hashs[i], sha256().hash(strings[i]).toHex());
    }
}

TEST(SHA256Class, att_bytes) {
    // testing the sha256 function on bytes
    std::vector<Bytes> hashv = std::vector<Bytes>();
    sha256 shaObj = sha256();
    for (int len = 4; len < TEST_HASH_MAX_LEN; len++) {
        for (u_int64_t num = 0; num < TEST_HASH_ITERS; num++) {
            Bytes tmpbytes = Bytes(len);
            tmpbytes.fillrandom();
            Bytes tmp = shaObj.hash(tmpbytes);
            EXPECT_EQ(tmp, shaObj.hash(tmpbytes));
            hashv.push_back(tmp);
        }
        //(hashv.end()-1)->print();
    }
    /*
    expectations:
    1. no collisions
    2. 32 Bytes len
    */
    for (Bytes b : hashv) {
        EXPECT_EQ(shaObj.getHashSize(), b.getLen());
        // may be the very unlikely case where two same bytes get randomly generated (test fails in this case)
        EXPECT_EQ(1, std::count(hashv.begin(), hashv.end(), b));
    }
}

TEST(SHA256Class, exact_bytes) {
    // testing the sha256 function on exact bytes that are gotten from a approved sha256 function
    std::vector<std::string> strings = {"jad", "", "z", "opjwdofiasasdf", "iou32894e934zh83", "öül34.ö23-42,.34,-23.4m23oi4z239o4hz239847z2", "asoizdfh8790qazf9up984u89auwrfsaf fjas pflsa .,4nmrt4"};
    std::vector<Bytes> bytes;
    for (std::string str : strings) {
        bytes.push_back(stringToBytes(str));
    }
    std::vector<std::string> hashs = {"65ebae8e077a037d81231c1bde91b721bf23bc3dd92baebdcdbe295ade66b79f", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
                                      "594e519ae499312b29433b7dd8a97ff068defcba9755b6d5d00e84c524d67b06", "794f3b75cb2279f380c2db0f749187b2d7df799a2d17c7060381cc1a24c5acf7",
                                      "1ae020b3a284e8f8e4348692ec56d39b5776cc726366ad24d8e40f44ebce3253", "a5cb38e244f7a4e2ff2f0290be1a30ef587c59fb1b08ec01cf9a84f11dd6eb28",
                                      "29ed1592959b390a8eb22cc95af855e016c4afe9583c7788365274cc09de6bd0"};
    for (int i = 0; i < bytes.size(); i++) {
        EXPECT_EQ(sha256().hash(bytes[i]), sha256().hash(bytes[i]));
        for (auto& c : hashs[i]) c = toupper(c);
        EXPECT_EQ(hashs[i], sha256().hash(bytes[i]).toHex());
    }
}
