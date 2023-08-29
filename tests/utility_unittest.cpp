#include "utility.h"

#include <gtest/gtest.h>

#include "rng.h"

// testing the utility functions

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

TEST(UtilityClass, stringToBytes) {
    Bytes b1(10);
    for (u_int64_t i = 0; i < 1000; i++) {
        std::string str = RNG::get_random_string(10);
        b1.setBytes(reinterpret_cast<const unsigned char*>(str.c_str()), str.length());
        EXPECT_EQ(b1, stringToBytes(str));
    }
    std::string s1 = "moin";
    Bytes b2(4);
    b2.addByte('m');
    b2.addByte('o');
    b2.addByte('i');
    b2.addByte('n');
    EXPECT_EQ(b2, stringToBytes(s1));
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

TEST(UtilityClass, isValidNumber){
    std::string s1 = "0";
    std::string s2 = "";
    std::string s3 = "102131231421";
    std::string s4 = "-131";
    std::string s5 = "0.0";
    std::string s6 = "0,0";
    std::string s7 = ".5";
    std::string s8 = "0.";
    std::string s9 = "-1-23";
    std::string s10 = "0.5";
    std::string s11 = "00000";
    std::string s12 = "-0000";
    std::string s13 = "0000.0";
    std::string s14 = "1 5";
    std::string s15 = " 15";
    std::string s16 = "15 ";

    EXPECT_TRUE(isValidNumber(s1));
    EXPECT_TRUE(isValidNumber(s1, true));
    EXPECT_TRUE(isValidNumber(s1, true, 0, 1));
    EXPECT_TRUE(isValidNumber(s1, true, 0, 0));
    EXPECT_FALSE(isValidNumber(s1, true, 1));

    EXPECT_FALSE(isValidNumber(s2));
    EXPECT_TRUE(isValidNumber(s2, true));
    EXPECT_TRUE(isValidNumber(s2, true, 0, 1));
    EXPECT_FALSE(isValidNumber(s2, false, 0, 256));

    EXPECT_TRUE(isValidNumber(s3));
    EXPECT_TRUE(isValidNumber(s3, true));
    EXPECT_TRUE(isValidNumber(s3, true, 0, 102131231421));
    EXPECT_FALSE(isValidNumber(s3, true, 0, 102131231420));

    EXPECT_FALSE(isValidNumber(s4));
    EXPECT_FALSE(isValidNumber(s4, true));

    EXPECT_FALSE(isValidNumber(s5));
    EXPECT_FALSE(isValidNumber(s5, true));

    EXPECT_FALSE(isValidNumber(s6));
    EXPECT_FALSE(isValidNumber(s6, true));

    EXPECT_FALSE(isValidNumber(s7));
    EXPECT_FALSE(isValidNumber(s7, true));

    EXPECT_FALSE(isValidNumber(s8));
    EXPECT_FALSE(isValidNumber(s8, true));

    EXPECT_FALSE(isValidNumber(s9));
    EXPECT_FALSE(isValidNumber(s9, true));

    EXPECT_FALSE(isValidNumber(s10));
    EXPECT_FALSE(isValidNumber(s10, true));

    EXPECT_TRUE(isValidNumber(s11));
    EXPECT_TRUE(isValidNumber(s11, true));
    EXPECT_TRUE(isValidNumber(s11, true, 0, 1));
    EXPECT_TRUE(isValidNumber(s11, true, 0, 0));
    EXPECT_FALSE(isValidNumber(s11, true, 1));

    EXPECT_FALSE(isValidNumber(s12));
    EXPECT_FALSE(isValidNumber(s12, true));

    EXPECT_FALSE(isValidNumber(s13));
    EXPECT_FALSE(isValidNumber(s13, true));

    EXPECT_FALSE(isValidNumber(s14));
    EXPECT_FALSE(isValidNumber(s14, true));

    EXPECT_FALSE(isValidNumber(s15));
    EXPECT_FALSE(isValidNumber(s15, true));

    EXPECT_FALSE(isValidNumber(s16));
    EXPECT_FALSE(isValidNumber(s16, true));
}
