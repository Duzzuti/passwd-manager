#include "gtest/gtest.h"
#include "utility.h"

TEST(UtilityClass, endswith){
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

TEST(UtilityClass, Longtocharvec){
    unsigned long l1 = 0;
    unsigned long l2 = 1;
    unsigned long l3 = -1;
    unsigned long l4 = -2;
    unsigned long l5 = l3/256;
    unsigned long l6 = l3/256 -1;
    unsigned long l7 = l3/256 +1;
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
    for(int i=0; i<6; i++){
        v6.push_back(255);
    }
    v6.push_back(254);
    for(int i=0; i<7; i++){
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

