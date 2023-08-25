#include "bytes.h"

#include <gtest/gtest.h>

#include "rng.h"

TEST(BytesClass, generatingBytes) {
    // testing constructors
    EXPECT_FALSE(Bytes(20) == Bytes(20));
    EXPECT_EQ(20, Bytes(20).getLen());
    EXPECT_EQ(200, Bytes(200).getLen());
    EXPECT_EQ(0, Bytes(0).getLen());
    EXPECT_EQ(0, Bytes().getLen());
    EXPECT_THROW(Bytes(-20), std::range_error);
}

TEST(BytesClass, returnTypes) {
    // testing return types
    EXPECT_EQ(typeid(void), typeid(Bytes().print()));
    EXPECT_EQ(typeid(void), typeid(Bytes().setBytes(RNG::get_random_bytes(10))));
    EXPECT_EQ(typeid(std::vector<unsigned char>), typeid(Bytes(10).getBytes()));
    unsigned char* l = new unsigned char[10];
    EXPECT_EQ(typeid(void), typeid(Bytes(10).getBytesArray(l, 10)));
    delete[] l;
    EXPECT_EQ(typeid(size_t), typeid(Bytes(10).getLen()));
    EXPECT_EQ(typeid(void), typeid(Bytes(10).addByte(1)));
    EXPECT_EQ(typeid(void), typeid(Bytes(10).addBytes(Bytes(10))));
    EXPECT_EQ(typeid(std::optional<Bytes>), typeid(Bytes(10).popFirstBytes(5)));
    EXPECT_EQ(typeid(std::optional<Bytes>), typeid(Bytes(10).getFirstBytes(5)));
    EXPECT_EQ(typeid(Bytes), typeid(Bytes(10).getFirstBytesFilledUp(5)));
    EXPECT_EQ(typeid(Bytes), typeid(Bytes(10).getFirstBytesFilledUp(5, 9)));
    EXPECT_EQ(typeid(Bytes), typeid(Bytes(10).popFirstBytesFilledUp(5)));
    EXPECT_EQ(typeid(Bytes), typeid(Bytes(10).popFirstBytesFilledUp(5, 9)));
    EXPECT_EQ(typeid(bool), typeid(Bytes(10).isEmpty()));
    EXPECT_EQ(typeid(bool), typeid(Bytes().isEmpty()));
    EXPECT_EQ(typeid(void), typeid(Bytes(10).clear()));
    EXPECT_EQ(typeid(bool), typeid(Bytes(10) == Bytes(10)));
    EXPECT_EQ(typeid(Bytes), typeid(Bytes(10) + Bytes(10)));
    EXPECT_EQ(typeid(Bytes), typeid(Bytes(10) - Bytes(10)));
    EXPECT_EQ(typeid(std::string), typeid(toHex(Bytes(10))));
    EXPECT_EQ(typeid(std::string), typeid(toHex(78)));
    EXPECT_EQ(typeid(u_int64_t), typeid(toLong(Bytes(10))));
    EXPECT_EQ(typeid(u_int64_t), typeid(toLong(78)));
}

TEST(BytesClass, getter) {
    // testing getters
    // right size of getter
    EXPECT_EQ(0, Bytes().getBytes().size());
    EXPECT_EQ(0, Bytes(0).getBytes().size());
    EXPECT_EQ(123, Bytes(123).getBytes().size());
    EXPECT_THROW(Bytes(-1).getBytes().size(), std::range_error);

    // get first bytes optional test
    EXPECT_TRUE(Bytes().getFirstBytes(0).has_value());
    EXPECT_TRUE(Bytes(0).getFirstBytes(0).has_value());
    EXPECT_TRUE(Bytes(12).getFirstBytes(0).has_value());
    EXPECT_FALSE(Bytes(12).getFirstBytes(13).has_value());
    EXPECT_FALSE(Bytes().getFirstBytes(1).has_value());

    // get first bytes len test
    EXPECT_EQ(10, Bytes(12).getFirstBytes(10).value().getLen());
    EXPECT_EQ(0, Bytes(12).getFirstBytes(0).value().getLen());
    EXPECT_THROW(Bytes(12).getFirstBytes(-1), std::range_error);

    // get first bytes value test
    Bytes testBytes1(30);
    Bytes testBytes2(20);
    std::vector<unsigned char> testv1 = {3, 123, 84, 0, 255, 4, 12, 98, 237, 223};
    std::vector<unsigned char> testv15 = {3, 123, 84, 0, 255};
    Bytes testBytes3 = Bytes();
    testBytes3.setBytes(testv1);
    EXPECT_EQ(testBytes1.getFirstBytes(10).value(), testBytes1.getFirstBytes(10).value());
    EXPECT_EQ(testBytes2.getFirstBytes(12).value(), testBytes2.getFirstBytes(12).value());
    EXPECT_EQ(testv15, testBytes3.getFirstBytes(5).value().getBytes());
}

TEST(BytesClass, setter) {
    // testing setters
    // right size of setter
    std::vector<unsigned char> testv1 = {123, 43, 23, 123, 213, 32, 0};
    std::vector<unsigned char> testv2 = {};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv31 = {0, 0, 0, 0, 0, 0, 255};
    std::vector<unsigned char> testv4 = {255};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);
    EXPECT_EQ(testv1, testBytes1.getBytes());
    EXPECT_EQ(testv2, testBytes2.getBytes());
    EXPECT_EQ(testv3, testBytes3.getBytes());
    EXPECT_EQ(testv4, testBytes4.getBytes());
    testBytes1.addBytes(testBytes2);
    EXPECT_EQ(testv1, testBytes1.getBytes());
    testBytes2.addBytes(testBytes3);
    EXPECT_EQ(testv3, testBytes2.getBytes());
    testBytes2.addBytes(testBytes4);
    EXPECT_EQ(testv31, testBytes2.getBytes());
}

TEST(BytesClass, pop) {
    // testing popFirstBytes
    // test optionals
    EXPECT_TRUE(Bytes(13).popFirstBytes(2).has_value());
    EXPECT_TRUE(Bytes().popFirstBytes(0).has_value());
    EXPECT_TRUE(Bytes(0).popFirstBytes(0).has_value());
    EXPECT_TRUE(Bytes(23).popFirstBytes(0).has_value());
    EXPECT_FALSE(Bytes(13).popFirstBytes(15).has_value());
    EXPECT_FALSE(Bytes().popFirstBytes(1).has_value());
    EXPECT_THROW(Bytes(12).getFirstBytes(-1), std::range_error);

    // pop first bytes len test
    EXPECT_EQ(10, Bytes(12).popFirstBytes(10).value().getLen());
    EXPECT_EQ(0, Bytes(12).popFirstBytes(0).value().getLen());

    // pop first bytes value test
    std::vector<unsigned char> testv1 = {123, 43, 23, 113, 213, 32, 0};
    std::vector<unsigned char> testv12 = {123, 43};
    std::vector<unsigned char> testv1from2 = {23, 113, 213, 32, 0};
    std::vector<unsigned char> testv2 = {};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv4 = {255};
    std::vector<unsigned char> testv40 = {};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);

    EXPECT_EQ(testv12, testBytes1.popFirstBytes(2).value().getBytes());
    EXPECT_FALSE(testBytes2.popFirstBytes(1).has_value());
    EXPECT_EQ(0, testBytes3.popFirstBytes(0).value().getLen());
    EXPECT_EQ(testv4, testBytes4.popFirstBytes(1).value().getBytes());
    EXPECT_EQ(testv1from2, testBytes1.getBytes());
    EXPECT_EQ(testv2, testBytes2.getBytes());
    EXPECT_EQ(testv3, testBytes3.getBytes());
    EXPECT_EQ(testv40, testBytes4.getBytes());
}

TEST(BytesClass, getfillup) {
    // get and pop first bytes with fill up value test

    // get first bytes len test
    EXPECT_EQ(10, Bytes(12).getFirstBytesFilledUp(10, 12).getLen());
    EXPECT_EQ(0, Bytes(0).getFirstBytesFilledUp(0).getLen());
    EXPECT_EQ(10, Bytes().getFirstBytesFilledUp(10, 255).getLen());
    EXPECT_EQ(10, Bytes(4).getFirstBytesFilledUp(10).getLen());
    EXPECT_THROW(Bytes(12).getFirstBytesFilledUp(-1, 0), std::range_error);

    // get first bytes value test
    std::vector<unsigned char> testv1 = {123, 43, 23, 113, 213, 32, 0};
    std::vector<unsigned char> testv12 = {123, 43};
    std::vector<unsigned char> testv1from2 = {23, 113, 213, 32, 0};
    std::vector<unsigned char> testv2 = {};
    std::vector<unsigned char> testv2f0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv3f65 = {0, 0, 0, 0, 0, 0, 65, 65, 65, 65};
    std::vector<unsigned char> testv4 = {255};
    std::vector<unsigned char> testv40 = {};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);
    EXPECT_EQ(testBytes1.getFirstBytesFilledUp(10), testBytes1.getFirstBytesFilledUp(10));
    EXPECT_EQ(testv2f0, testBytes2.getFirstBytesFilledUp(12).getBytes());
    EXPECT_EQ(testv3f65, testBytes3.getFirstBytesFilledUp(10, 65).getBytes());
    EXPECT_EQ(testv4, testBytes4.getFirstBytesFilledUp(1, 90).getBytes());
}

TEST(BytesClass, popfillup) {
    // pop first bytes len test
    EXPECT_EQ(10, Bytes(12).popFirstBytesFilledUp(10).getLen());
    EXPECT_EQ(0, Bytes(12).popFirstBytesFilledUp(0, 16).getLen());
    EXPECT_EQ(1, Bytes().popFirstBytesFilledUp(1, 16).getLen());
    EXPECT_EQ(67, Bytes(23).popFirstBytesFilledUp(67, 255).getLen());
    EXPECT_THROW(Bytes(12).popFirstBytesFilledUp(-1, 0), std::range_error);

    // pop first bytes value test
    std::vector<unsigned char> testv1 = {123, 43, 23, 113, 213, 32, 0};
    std::vector<unsigned char> testv12 = {123, 43};
    std::vector<unsigned char> testv1from2 = {23, 113, 213, 32, 0};
    std::vector<unsigned char> testv2 = {};
    std::vector<unsigned char> testv2f6 = {6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv3f255 = {0, 0, 0, 0, 0, 0, 255, 255, 255, 255};
    std::vector<unsigned char> testv4 = {255};
    std::vector<unsigned char> testv4f0 = {255, 0};
    std::vector<unsigned char> testv40 = {};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);

    EXPECT_EQ(testv12, testBytes1.popFirstBytesFilledUp(2, 76).getBytes());
    EXPECT_EQ(testv2f6, testBytes2.popFirstBytesFilledUp(10, 6).getBytes());
    EXPECT_EQ(testv3f255, testBytes3.popFirstBytesFilledUp(10, 255).getBytes());
    EXPECT_EQ(testv4f0, testBytes4.popFirstBytesFilledUp(2, 0).getBytes());

    EXPECT_EQ(testv1from2, testBytes1.getBytes());
    EXPECT_EQ(testv2, testBytes2.getBytes());
    EXPECT_EQ(testv2, testBytes3.getBytes());
    EXPECT_EQ(testv40, testBytes4.getBytes());
}

TEST(BytesClass, addByte) {
    // testing add one byte to a bytes object
    std::vector<Bytes> bytes;
    std::vector<unsigned char> addbytes;
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    bytes.push_back(testBytes1);
    bytes.push_back(testBytes2);
    bytes.push_back(testBytes3);
    bytes.push_back(testBytes4);
    addbytes.push_back(0);
    addbytes.push_back(155);
    addbytes.push_back(16);
    addbytes.push_back(255);
    addbytes.push_back(100);
    addbytes.push_back(230);

    for (Bytes byte : bytes) {
        for (unsigned char addbyte : addbytes) {
            std::string beforeAdd = toHex(byte);
            byte.addByte(addbyte);
            EXPECT_EQ(beforeAdd + toHex(addbyte), toHex(byte));
        }
    }
}

TEST(BytesClass, clear) {
    // testing clearing and isEmpty
    std::vector<unsigned char> testv1 = {123, 43, 23, 113, 213, 32, 0};
    std::vector<unsigned char> testv2 = {};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv4 = {255};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);
    EXPECT_FALSE(testBytes1.isEmpty());
    EXPECT_TRUE(testBytes2.isEmpty());
    EXPECT_FALSE(testBytes3.isEmpty());
    EXPECT_FALSE(testBytes4.isEmpty());

    testBytes1.clear();
    testBytes2.clear();
    testBytes3.clear();
    testBytes4.clear();
    EXPECT_EQ(0, testBytes1.getBytes().size());
    EXPECT_EQ(0, testBytes2.getBytes().size());
    EXPECT_EQ(0, testBytes3.getBytes().size());
    EXPECT_EQ(0, testBytes4.getBytes().size());
    EXPECT_TRUE(testBytes1.isEmpty());
    EXPECT_TRUE(testBytes2.isEmpty());
    EXPECT_TRUE(testBytes3.isEmpty());
    EXPECT_TRUE(testBytes4.isEmpty());
}

TEST(Utils, toHex) {
    // testing the byte to hex converter function
    EXPECT_EQ("00", toHex(0));
    EXPECT_EQ("10", toHex(16));
    EXPECT_EQ("0F", toHex(15));
    EXPECT_EQ("FF", toHex(255));
    EXPECT_EQ("20", toHex(32));
    EXPECT_EQ("B2", toHex(178));
    EXPECT_EQ("E7", toHex(231));
    EXPECT_EQ("64", toHex(100));
    EXPECT_EQ("22", toHex(34));

    std::vector<unsigned char> testv1 = {123, 43, 23, 113, 213, 32, 0};
    std::vector<unsigned char> testv2 = {};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv4 = {255};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);
    EXPECT_EQ("7B2B1771D52000", toHex(testBytes1));
    EXPECT_EQ("", toHex(testBytes2));
    EXPECT_EQ("000000000000", toHex(testBytes3));
    EXPECT_EQ("FF", toHex(testBytes4));
}

TEST(Utils, toLong) {
    // testing function toLong
    u_int64_t max_long = -1;
    u_int64_t max_long_one_less = -2;
    u_int64_t zero = 0;
    u_int64_t one = 1;
    Bytes oneBytes = Bytes();
    Bytes zeroBytes = Bytes();
    Bytes maxLongBytes = Bytes();
    Bytes maxLongLessBytes = Bytes();
    Bytes threeBytes = Bytes();
    for (int i = 0; i < 7; i++) {
        maxLongBytes.addByte(255);
        maxLongLessBytes.addByte(255);
        zeroBytes.addByte(0);
        oneBytes.addByte(0);
        if (i < 3) {
            threeBytes.addByte(198);
        }
    }
    zeroBytes.addByte(0);
    oneBytes.addByte(1);
    maxLongBytes.addByte(255);
    maxLongLessBytes.addByte(254);

    EXPECT_EQ(max_long, toLong(maxLongBytes));
    EXPECT_EQ(max_long_one_less, toLong(maxLongLessBytes));
    EXPECT_EQ(zero, toLong(zeroBytes));
    EXPECT_EQ(one, toLong(oneBytes));
    EXPECT_THROW(toLong(Bytes(9)), std::length_error);
    EXPECT_EQ(0, toLong(Bytes()));
    EXPECT_EQ(13027014, toLong(threeBytes));
}

TEST(Utils, bytesOperator) {
    // testing the overloaded operators on bytes
    std::vector<unsigned char> testv1 = {123, 43, 23, 113, 213, 32, 0};
    std::vector<unsigned char> testv2 = {89, 255, 0, 189, 11, 67, 254};
    std::vector<unsigned char> testv12 = {123 + 89, (43 + 255) % 256, 23, (113 + 189) % 256, 224, 99, 254};
    std::vector<unsigned char> testv3 = {0, 0, 0, 0, 0, 0};
    std::vector<unsigned char> testv31 = {1, 1, 1, 1, 1, 1};
    std::vector<unsigned char> testv4 = {255, 255, 255, 255, 255, 255};
    std::vector<unsigned char> testv42 = {254, 254, 254, 254, 254, 254};
    std::vector<unsigned char> testv5 = {};
    Bytes testBytes1(12);
    Bytes testBytes2 = Bytes();
    Bytes testBytes3 = Bytes(0);
    Bytes testBytes4 = Bytes(89);
    Bytes testBytes5;
    testBytes1.setBytes(testv1);
    testBytes2.setBytes(testv2);
    testBytes3.setBytes(testv3);
    testBytes4.setBytes(testv4);
    testBytes5.setBytes(testv5);

    EXPECT_EQ(testv12, (testBytes1 + testBytes2).getBytes());
    EXPECT_EQ(testv12, (testBytes2 + testBytes1).getBytes());
    EXPECT_THROW(testBytes1 + testBytes3, std::length_error);
    EXPECT_THROW(testBytes1 - testBytes3, std::length_error);
    EXPECT_THROW(testBytes2 + testBytes3, std::length_error);
    EXPECT_THROW(testBytes3 - testBytes2, std::length_error);
    EXPECT_EQ(Bytes(), testBytes5 + testBytes5);
    EXPECT_EQ(Bytes(), testBytes5 - testBytes5);
    EXPECT_THROW(testBytes1 + testBytes5, std::length_error);
    EXPECT_THROW(testBytes5 - testBytes1, std::length_error);
    EXPECT_EQ(testv3, (testBytes3 + testBytes3).getBytes());
    EXPECT_EQ(testv3, (testBytes3 - testBytes3).getBytes());
    EXPECT_EQ(testv4, (testBytes4 + testBytes3).getBytes());
    EXPECT_EQ(testv4, (testBytes4 - testBytes3).getBytes());
    EXPECT_EQ(testv31, (testBytes3 - testBytes4).getBytes());
    EXPECT_EQ(testv3, (testBytes4 - testBytes4).getBytes());
    EXPECT_EQ(testv42, (testBytes4 + testBytes4).getBytes());
}
