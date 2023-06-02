#include "utility.h"

#include "gtest/gtest.h"
#include "rng.h"

//testing the utility functions

TEST(UtilityClass, endswith) {
    std::string s1 = "isdahfaksjdfas asdf asdf sadffsaasdfkoö-.:fp9pfdas ";
    std::string e1 = "isdahfaksjdfas asdf asdf sadffsaasdfkoö-.:fp9pfdas ";
    std::string e2 = "";
    std::string e3 = " ";
    std::string e4 = "pfdas ";
    std::string e5 = " pfdas ";
    EXPECT_TRUE(endsWith(s1, e1));
    EXPECT_TRUE(endsWith(s1, e2));
    EXPECT_TRUE(endsWith(s1, e3));
    EXPECT_TRUE(endsWith(s1, e4));
    EXPECT_FALSE(endsWith(s1, e5));

    std::string s2 = "";
    std::string e21 = "";
    std::string e22 = " ";
    std::string e23 = "\n";
    EXPECT_TRUE(endsWith(s2, e21));
    EXPECT_FALSE(endsWith(s2, e22));
    EXPECT_FALSE(endsWith(s2, e23));

    std::string s3 = "moi.n";
    std::string e31 = "moi.n";
    std::string e32 = " ";
    std::string e33 = " moi.n";
    EXPECT_TRUE(endsWith(s3, e31));
    EXPECT_FALSE(endsWith(s3, e32));
    EXPECT_FALSE(endsWith(s3, e33));
}

TEST(UtilityClass, Longtocharvec) {
    u_int64_t l1 = 0;
    u_int64_t l2 = 1;
    u_int64_t l3 = -1;
    u_int64_t l4 = -2;
    u_int64_t l5 = l3 / 256;
    u_int64_t l6 = l3 / 256 - 1;
    u_int64_t l7 = l3 / 256 + 1;
    std::vector<unsigned char> v1;
    std::vector<unsigned char> v2;
    std::vector<unsigned char> v3;
    std::vector<unsigned char> v4;
    std::vector<unsigned char> v5;
    std::vector<unsigned char> v6;
    std::vector<unsigned char> v7;
    v5.push_back(0);
    v6.push_back(0);
    v7.push_back(1);
    for (int i = 0; i < 6; i++) {
        v6.push_back(255);
    }
    v6.push_back(254);
    for (int i = 0; i < 7; i++) {
        v1.push_back(0);
        v2.push_back(0);
        v3.push_back(255);
        v4.push_back(255);
        v5.push_back(255);
        v7.push_back(0);
    }
    v1.push_back(0);
    v2.push_back(1);
    v3.push_back(255);
    v4.push_back(254);

    EXPECT_EQ(v1, LongToCharVec(l1));
    EXPECT_EQ(v2, LongToCharVec(l2));
    EXPECT_EQ(v3, LongToCharVec(l3));
    EXPECT_EQ(v4, LongToCharVec(l4));
    EXPECT_EQ(v5, LongToCharVec(l5));
    EXPECT_EQ(v6, LongToCharVec(l6));
    EXPECT_EQ(v7, LongToCharVec(l7));
}

TEST(UtilityClass, getLongLen) {
    u_int64_t l1 = 0;
    u_int64_t l2 = 1;
    u_int64_t l3 = -1;
    u_int64_t l4 = -2;
    u_int64_t l5 = l3 / 256;
    u_int64_t l6 = l3 / 256 - 1;
    u_int64_t l7 = l3 / 256 + 1;
    u_int64_t l8 = 256;
    u_int64_t l9 = 255;
    u_int64_t l10 = (u_int64_t)256 * 256 * 256 * 256;
    u_int64_t l11 = (u_int64_t)256 * 256 * 256 * 256 - 1;
    EXPECT_EQ(0, getLongLen(l1));
    EXPECT_EQ(1, getLongLen(l2));
    EXPECT_EQ(8, getLongLen(l3));
    EXPECT_EQ(8, getLongLen(l4));
    EXPECT_EQ(7, getLongLen(l5));
    EXPECT_EQ(7, getLongLen(l6));
    EXPECT_EQ(8, getLongLen(l7));
    EXPECT_EQ(2, getLongLen(l8));
    EXPECT_EQ(1, getLongLen(l9));
    EXPECT_EQ(5, getLongLen(l10));
    EXPECT_EQ(4, getLongLen(l11));
}

TEST(UtilityClass, stringToBytes){
    Bytes b1;
    for(u_int64_t i = 0; i < 1000; i++){
        b1.setBytes(RNG::get_random_bytes(10));
        EXPECT_EQ(b1, stringToBytes(charVecToString(b1.getBytes())));
    }
    std::string s1 = "moin";
    b1.setBytes(std::vector<unsigned char>(s1.begin(), s1.end()));
    EXPECT_EQ(b1, stringToBytes(s1));
}