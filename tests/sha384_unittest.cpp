#include "sha384.h"

#include "gtest/gtest.h"
#include "test_settings.cpp"
#include "test_utils.h"  //provide gen_random for random strings

TEST(SHA384Class, returnTypes){
    // testing the return types of the sha384 class
    sha384 shaObj = sha384();
    EXPECT_EQ(SHA384_DIGEST_LENGTH, shaObj.getHashSize());
    EXPECT_EQ(typeid(int), typeid(shaObj.getHashSize()));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash(Bytes())));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash(Bytes(10))));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash("")));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash("sadfasd .-fsa")));
}

TEST(SHA384Class, att_strings) {
    // testing the sha384 function on strings
    std::vector<Bytes> hashv = std::vector<Bytes>();
    sha384 shaObj = sha384();
    for (int len = 4; len < TEST_HASH_MAX_LEN; len++) {
        for (u_int64_t num = 0; num < TEST_HASH_ITERS; num++) {
            std::string tmpstr = gen_random_string(len);
            Bytes tmp = shaObj.hash(tmpstr);
            EXPECT_EQ(tmp, shaObj.hash(tmpstr));
            hashv.push_back(tmp);
        }
        //(hashv.end()-1)->print();
    }
    /*
    expectations:
    1. no collisions
    2. 48 Bytes len
    */
    for (Bytes b : hashv) {
        EXPECT_EQ(shaObj.getHashSize(), b.getLen());
        // may be the very unlikely case where two same strings get randomly generated (test failes in this case)
        EXPECT_EQ(1, std::count(hashv.begin(), hashv.end(), b));
    }
}

TEST(SHA384Class, exact_strings) {
    // testing the sha384 function on exact strings that are gotten from a approved sha384 function
    std::vector<std::string> strings = {"jad", "", "z", "opjwdofiasasdf", "iou32894e934zh83", "öül34.ö23-42,.34,-23.4m23oi4z239o4hz239847z2", "asoizdfh8790qazf9up984u89auwrfsaf fjas pflsa .,4nmrt4"};
    std::vector<std::string> hashs = {"b882e19d748657b99d03f15106a7de155251fdb5b7217a4c841cf7a2c8c52305f13783a866c83f0fa2c46a0f0de77671",
                                      "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
                                      "c39c06ca383f11c2870c8ea1368e861cee29dde246368c17b6985f7a7d650d86a90aa8bbb176ddbd99f06d490f0495e5",
                                      "a14bd4a8173ef24a875c3bf6492dbab38c329d737a7f1ef15c88b6ec46e3e2fd68036ddc8a03031ba4dc31621f28f56e",
                                      "ba15460f1e8755f4878cccaeafc2c5d3231f8531ff9ebd5b361a958470f31cbdf5e82d86c39bb288ca51700f65390281",
                                      "0067e6987129b5f1e7b871428df596aa3d0201e46bf08267826bf5b77ee6707006b8c01fc787c1a634fa5d5594492494",
                                      "1623e3f3ac382a083a72a481ba2f409e772dffcab530092fbb87d9da6f6a84896c6a101db8d081f04a6fc2c92bdd767b"};
    for (int i = 0; i < strings.size(); i++) {
        EXPECT_EQ(sha384().hash(strings[i]), sha384().hash(strings[i]));
        for (auto& c : hashs[i]) c = toupper(c);
        EXPECT_EQ(hashs[i], toHex(sha384().hash(strings[i])));
    }
}

TEST(SHA384Class, att_bytes) {
    // testing the sha384 function on bytes
    std::vector<Bytes> hashv = std::vector<Bytes>();
    sha384 shaObj = sha384();
    for (int len = 4; len < TEST_HASH_MAX_LEN; len++) {
        for (u_int64_t num = 0; num < TEST_HASH_ITERS; num++) {
            Bytes tmpbytes = Bytes(len);
            Bytes tmp = shaObj.hash(tmpbytes);
            EXPECT_EQ(tmp, shaObj.hash(tmpbytes));
            hashv.push_back(tmp);
        }
        //(hashv.end()-1)->print();
    }
    /*
    expectations:
    1. no collisions
    2. 48 Bytes len
    */
    for (Bytes b : hashv) {
        EXPECT_EQ(shaObj.getHashSize(), b.getLen());
        // may be the very unlikely case where two same bytes get randomly generated (test failes in this case)
        EXPECT_EQ(1, std::count(hashv.begin(), hashv.end(), b));
    }
}

TEST(SHA384Class, exact_bytes) {
    // testing the sha384 function on exact bytes that are gotten from a approved sha384 function
    std::vector<std::string> strings = {"jad", "", "z", "opjwdofiasasdf", "iou32894e934zh83", "öül34.ö23-42,.34,-23.4m23oi4z239o4hz239847z2", "asoizdfh8790qazf9up984u89auwrfsaf fjas pflsa .,4nmrt4"};
    std::vector<Bytes> bytes;
    for (std::string str : strings) {
        bytes.emplace_back();
        std::vector<unsigned char> data(str.begin(), str.end());
        bytes.back().setBytes(data);
    }
    std::vector<std::string> hashs = {"b882e19d748657b99d03f15106a7de155251fdb5b7217a4c841cf7a2c8c52305f13783a866c83f0fa2c46a0f0de77671",
                                      "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
                                      "c39c06ca383f11c2870c8ea1368e861cee29dde246368c17b6985f7a7d650d86a90aa8bbb176ddbd99f06d490f0495e5",
                                      "a14bd4a8173ef24a875c3bf6492dbab38c329d737a7f1ef15c88b6ec46e3e2fd68036ddc8a03031ba4dc31621f28f56e",
                                      "ba15460f1e8755f4878cccaeafc2c5d3231f8531ff9ebd5b361a958470f31cbdf5e82d86c39bb288ca51700f65390281",
                                      "0067e6987129b5f1e7b871428df596aa3d0201e46bf08267826bf5b77ee6707006b8c01fc787c1a634fa5d5594492494",
                                      "1623e3f3ac382a083a72a481ba2f409e772dffcab530092fbb87d9da6f6a84896c6a101db8d081f04a6fc2c92bdd767b"};
    for (int i = 0; i < bytes.size(); i++) {
        EXPECT_EQ(sha384().hash(bytes[i]), sha384().hash(bytes[i]));
        for (auto& c : hashs[i]) c = toupper(c);
        EXPECT_EQ(hashs[i], toHex(sha384().hash(bytes[i])));
    }
}
