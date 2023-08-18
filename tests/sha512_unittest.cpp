#include "sha512.h"

#include <gtest/gtest.h>

#include "test_settings.cpp"
#include "utility.h"

TEST(SHA512Class, returnTypes) {
    // testing the return types of the sha512 class
    sha512 shaObj = sha512();
    EXPECT_EQ(SHA512_DIGEST_LENGTH, shaObj.getHashSize());
    EXPECT_EQ(typeid(int), typeid(shaObj.getHashSize()));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash(Bytes())));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash(Bytes(10))));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash("")));
    EXPECT_EQ(typeid(Bytes), typeid(shaObj.hash("sadfasd .-fsa")));
}

TEST(SHA512Class, att_strings) {
    // testing the sha512 function on strings
    std::vector<Bytes> hashv = std::vector<Bytes>();
    sha512 shaObj = sha512();
    for (int len = 4; len < TEST_HASH_MAX_LEN; len++) {
        for (u_int64_t num = 0; num < TEST_HASH_ITERS; num++) {
            std::string tmpstr = charVecToString(Bytes(len).getBytes());
            Bytes tmp = shaObj.hash(tmpstr);
            EXPECT_EQ(tmp, shaObj.hash(tmpstr));
            hashv.push_back(tmp);
        }
        //(hashv.end()-1)->print();
    }
    /*
    expectations:
    1. no collisions
    2. 64 Bytes len
    */
    for (Bytes b : hashv) {
        EXPECT_EQ(shaObj.getHashSize(), b.getLen());
        // may be the very unlikely case where two same strings get randomly generated (test fails in this case)
        EXPECT_EQ(1, std::count(hashv.begin(), hashv.end(), b));
    }
}

TEST(SHA512Class, exact_strings) {
    // testing the sha512 function on exact strings that are gotten from a approved sha512 function
    std::vector<std::string> strings = {"jad", "", "z", "opjwdofiasasdf", "iou32894e934zh83", "öül34.ö23-42,.34,-23.4m23oi4z239o4hz239847z2", "asoizdfh8790qazf9up984u89auwrfsaf fjas pflsa .,4nmrt4"};
    std::vector<std::string> hashs = {"d490d8705c47cc9e57e2388362ce1495474e0047709bf4b1f169a9ff9c0e594bb4ec8245e3374deafb59581af1d0618c86e66b68b758a5a91750f3411dfb742d",
                                      "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
                                      "5ae625665f3e0bd0a065ed07a41989e4025b79d13930a2a8c57d6b4325226707d956a082d1e91b4d96a793562df98fd03c9dcf743c9c7b4e3055d4f9f09ba015",
                                      "0f684aec0c7aaea79aa6b9473e8e667fe53fda8969281db1c69d1a018bf82665850bc5e5f8103a09f4f4fd71e419e2c4e696af02f9c2205fd8c87ce4c0c5e066",
                                      "15b6ff3dbd04491ecf0fab91e4fde72204575dbc451142222dd66673b998d14f9f6cf5d34ea8f2575677593c2342ccb98f350e6581d4a92ff2903e27afc92a79",
                                      "b499a6ca760f77f7d9c54524e9aeaa1b0dc7527f924171d482dc657d504a3d6061f1706bd14f51cdb804b69433c377e74cfe168ff0792416f5845a5e5cd0617a",
                                      "26397ade76d807563fb434cec79150070199e6ff57c7f45ea765e11daad5c93f688faa94a42da8995b45e5b3f9c491ad3e10a435f56f4d2072ffd57211706531"};
    for (int i = 0; i < strings.size(); i++) {
        EXPECT_EQ(sha512().hash(strings[i]), sha512().hash(strings[i]));
        for (auto& c : hashs[i]) c = toupper(c);
        EXPECT_EQ(hashs[i], toHex(sha512().hash(strings[i])));
    }
}

TEST(SHA512Class, att_bytes) {
    // testing the sha512 function on bytes
    std::vector<Bytes> hashv = std::vector<Bytes>();
    sha512 shaObj = sha512();
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
    2. 64 Bytes len
    */
    for (Bytes b : hashv) {
        EXPECT_EQ(shaObj.getHashSize(), b.getLen());
        // may be the very unlikely case where two same bytes get randomly generated (test fails in this case)
        EXPECT_EQ(1, std::count(hashv.begin(), hashv.end(), b));
    }
}

TEST(SHA512Class, exact_bytes) {
    // testing the sha512 function on exact bytes that are gotten from a approved sha512 function
    std::vector<std::string> strings = {"jad", "", "z", "opjwdofiasasdf", "iou32894e934zh83", "öül34.ö23-42,.34,-23.4m23oi4z239o4hz239847z2", "asoizdfh8790qazf9up984u89auwrfsaf fjas pflsa .,4nmrt4"};
    std::vector<Bytes> bytes;
    for (std::string str : strings) {
        bytes.emplace_back();
        std::vector<unsigned char> data(str.begin(), str.end());
        bytes.back().setBytes(data);
    }
    std::vector<std::string> hashs = {"d490d8705c47cc9e57e2388362ce1495474e0047709bf4b1f169a9ff9c0e594bb4ec8245e3374deafb59581af1d0618c86e66b68b758a5a91750f3411dfb742d",
                                      "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
                                      "5ae625665f3e0bd0a065ed07a41989e4025b79d13930a2a8c57d6b4325226707d956a082d1e91b4d96a793562df98fd03c9dcf743c9c7b4e3055d4f9f09ba015",
                                      "0f684aec0c7aaea79aa6b9473e8e667fe53fda8969281db1c69d1a018bf82665850bc5e5f8103a09f4f4fd71e419e2c4e696af02f9c2205fd8c87ce4c0c5e066",
                                      "15b6ff3dbd04491ecf0fab91e4fde72204575dbc451142222dd66673b998d14f9f6cf5d34ea8f2575677593c2342ccb98f350e6581d4a92ff2903e27afc92a79",
                                      "b499a6ca760f77f7d9c54524e9aeaa1b0dc7527f924171d482dc657d504a3d6061f1706bd14f51cdb804b69433c377e74cfe168ff0792416f5845a5e5cd0617a",
                                      "26397ade76d807563fb434cec79150070199e6ff57c7f45ea765e11daad5c93f688faa94a42da8995b45e5b3f9c491ad3e10a435f56f4d2072ffd57211706531"};
    for (int i = 0; i < bytes.size(); i++) {
        EXPECT_EQ(sha512().hash(bytes[i]), sha512().hash(bytes[i]));
        for (auto& c : hashs[i]) c = toupper(c);
        EXPECT_EQ(hashs[i], toHex(sha512().hash(bytes[i])));
    }
}
