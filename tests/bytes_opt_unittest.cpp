#include "bytes_opt.h"

#include <gtest/gtest.h>

#include <cstring>
#include <memory>

TEST(BytesOptClass, constructor) {
    BytesOpt b1(10);
    EXPECT_EQ(0, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    BytesOpt b2(0);
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b2.getMaxLen());
    BytesOpt b3(1000);
    EXPECT_EQ(0, b3.getLen());
    EXPECT_EQ(1000, b3.getMaxLen());

    EXPECT_THROW(BytesOpt b4(-1), std::invalid_argument);
}

TEST(BytesOptClass, fillrandom) {
    // len checks
    BytesOpt b1(10);
    b1.fillrandom();
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    BytesOpt b2(0);
    b2.fillrandom();
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b2.getMaxLen());
    BytesOpt b3(1000);
    b3.fillrandom();
    EXPECT_EQ(1000, b3.getLen());
    EXPECT_EQ(1000, b3.getMaxLen());

    BytesOpt b4(100);
    b4.addByte(0xad);
    b4.fillrandom();
    EXPECT_EQ(100, b4.getLen());
    EXPECT_EQ(100, b4.getMaxLen());
    EXPECT_EQ(0xad, b4.getBytes()[0]);

    BytesOpt b5(100);
    b5.addrandom(10);
    BytesOpt b6(100);
    b5.copyToBytes(b6);
    b5.fillrandom();
    EXPECT_EQ(100, b5.getLen());
    EXPECT_EQ(100, b5.getMaxLen());
    EXPECT_EQ(10, b6.getLen());
    EXPECT_EQ(100, b6.getMaxLen());
    EXPECT_NE(b5, b6);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b6.getBytes(), 10) == 0);

    BytesOpt b7(100);
    b7.addrandom(100);
    BytesOpt b8(100);
    b7.copyToBytes(b8);
    b7.fillrandom();
    b8.fillrandom();
    EXPECT_EQ(100, b7.getLen());
    EXPECT_EQ(100, b7.getMaxLen());
    EXPECT_EQ(100, b8.getLen());
    EXPECT_EQ(100, b8.getMaxLen());
    EXPECT_EQ(b7, b8);
    EXPECT_TRUE(std::memcmp(b7.getBytes(), b8.getBytes(), 100) == 0);

    BytesOpt b9(100);
    b9.fillrandom();
    BytesOpt b10(100);
    b9.copyToBytes(b10);
    b9.fillrandom();
    EXPECT_EQ(100, b9.getLen());
    EXPECT_EQ(100, b9.getMaxLen());
    EXPECT_EQ(100, b10.getLen());
    EXPECT_EQ(100, b10.getMaxLen());
    EXPECT_EQ(b9, b10);
    EXPECT_TRUE(std::memcmp(b9.getBytes(), b10.getBytes(), 100) == 0);

    // random checks
    BytesOpt b11(100);
    b4.fillrandom();
    BytesOpt b12(100);
    b5.fillrandom();
    EXPECT_NE(b4, b5);
}

TEST(BytesOptClass, addrandom) {
    // len checks
    BytesOpt b1(10);
    b1.addrandom(10);
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    BytesOpt b2(0);
    b2.addrandom(0);
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b2.getMaxLen());
    BytesOpt b3(1000);
    b3.addrandom(1000);
    EXPECT_EQ(1000, b3.getLen());
    EXPECT_EQ(1000, b3.getMaxLen());

    BytesOpt b4(100);
    b4.addrandom(10);
    EXPECT_EQ(10, b4.getLen());
    EXPECT_EQ(100, b4.getMaxLen());
    BytesOpt b5(100);
    b5.addrandom(0);
    EXPECT_EQ(0, b5.getLen());
    EXPECT_EQ(100, b5.getMaxLen());

    // random checks
    BytesOpt b6(100);
    b6.addrandom(100);
    BytesOpt b7(100);
    b7.fillrandom();
    EXPECT_NE(b6, b7);

    BytesOpt b8(100);
    b8.addrandom(10);
    BytesOpt b9(100);
    b9.addrandom(10);
    EXPECT_NE(b8, b9);

    BytesOpt b10(100);
    b10.addrandom(10);
    BytesOpt b11(100);
    b10.copyToBytes(b11);
    EXPECT_EQ(b10, b11);
    b10.addrandom(10);
    EXPECT_NE(b10, b11);
    b11.addrandom(10);
    EXPECT_NE(b10, b11);

    BytesOpt b12(100);
    b12.fillrandom();
    BytesOpt b13(100);
    b12.copyToBytes(b13);
    EXPECT_EQ(b12, b13);
    EXPECT_THROW(b12.addrandom(10), std::length_error);
    EXPECT_THROW(b13.addrandom(10), std::length_error);

    BytesOpt b14(100);
    b14.addrandom(100);
    BytesOpt b15(100);
    b14.copyToBytes(b15);
    EXPECT_EQ(b14, b15);
    EXPECT_THROW(b14.addrandom(10), std::length_error);
    EXPECT_THROW(b15.addrandom(10), std::length_error);

    // exception checks
    BytesOpt b16(100);
    EXPECT_THROW(b16.addrandom(101), std::length_error);

    BytesOpt b17(100);
    EXPECT_THROW(b17.addrandom(-1), std::length_error);

    BytesOpt b18(100);
    b18.addrandom(99);
    EXPECT_THROW(b18.addrandom(2), std::length_error);

    BytesOpt b19(100);
    b19.addrandom(100);
    EXPECT_THROW(b19.addrandom(1), std::length_error);

    BytesOpt b20(100);
    b20.addrandom(99);
    EXPECT_NO_THROW(b20.addrandom(1));
}

TEST(BytesOptClass, consumeBytes) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char test[10];
    b1.consumeBytes(bytes1, 10);
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    std::memcpy(test, b1.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    BytesOpt b2(10);
    b2.fillrandom();
    b2.consumeBytes(bytes1, 10);
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(10, b2.getMaxLen());
    std::memcpy(test, b2.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    BytesOpt b3(10);
    b3.addrandom(5);
    b3.consumeBytes(bytes1, 10);
    EXPECT_EQ(10, b3.getLen());
    EXPECT_EQ(10, b3.getMaxLen());
    std::memcpy(test, b3.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    BytesOpt b4(10);
    b4.addByte(0xad);
    b4.consumeBytes(bytes1, 9);
    EXPECT_EQ(9, b4.getLen());
    EXPECT_EQ(10, b4.getMaxLen());
    std::memcpy(test, b4.getBytes(), 9);
    test[9] = 9;
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    // exception checks
    unsigned char bytes2[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    BytesOpt b5(10);
    EXPECT_THROW(b5.consumeBytes(bytes2, 11), std::length_error);
    BytesOpt b6(0);
    EXPECT_THROW(b6.consumeBytes(bytes2, 11), std::length_error);
    BytesOpt b7(11);
    EXPECT_NO_THROW(b7.consumeBytes(bytes2, 11));
    BytesOpt b8(11);
    EXPECT_THROW(b8.consumeBytes(nullptr, 1), std::invalid_argument);
}

TEST(BytesOptClass, addconsumeBytes) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char test[10];
    b1.addconsumeBytes(bytes1, 10);
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    std::memcpy(test, b1.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    BytesOpt b2(10);
    b2.addrandom(5);
    b2.addconsumeBytes(bytes1, 5);
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(10, b2.getMaxLen());
    std::memcpy(test, b2.getBytes() + 5, 5);
    EXPECT_TRUE(std::memcmp(bytes1, test, 5) == 0);

    BytesOpt b3(10);
    b3.addByte(0xad);
    b3.addconsumeBytes(bytes1, 5);
    EXPECT_EQ(6, b3.getLen());
    EXPECT_EQ(10, b3.getMaxLen());
    std::memcpy(test + 1, bytes1, 5);
    test[0] = 0xad;
    EXPECT_TRUE(std::memcmp(b3.getBytes(), test, 6) == 0);

    // exception checks
    unsigned char bytes2[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    BytesOpt b4(10);
    EXPECT_THROW(b4.addconsumeBytes(bytes2, 11), std::length_error);
    BytesOpt b5(0);
    EXPECT_THROW(b5.addconsumeBytes(bytes2, 11), std::length_error);
    BytesOpt b6(11);
    EXPECT_NO_THROW(b6.addconsumeBytes(bytes2, 11));
    BytesOpt b7(11);
    b7.addByte(0xad);
    EXPECT_THROW(b7.addconsumeBytes(bytes2, 11), std::length_error);
    BytesOpt b8(11);
    EXPECT_THROW(b8.addconsumeBytes(nullptr, 1), std::invalid_argument);
}

TEST(BytesOptClass, getBytes) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    unsigned char* test = b1.getBytes();
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    BytesOpt b2(10);
    b2.fillrandom();
    test = b2.getBytes();
    EXPECT_TRUE(std::memcmp(b2.getBytes(), test, 10) == 0);

    BytesOpt b3(10);
    b3.addrandom(5);
    test = b3.getBytes();
    EXPECT_TRUE(std::memcmp(b3.getBytes(), test, 5) == 0);

    BytesOpt b4(10);
    b4.addByte(0xad);
    test = b4.getBytes();
    EXPECT_TRUE(std::memcmp(b4.getBytes(), test, 1) == 0);

    BytesOpt b5(0);
    test = b5.getBytes();
    EXPECT_TRUE(std::memcmp(b5.getBytes(), test, 0) == 0);
}

TEST(BytesOptClass, getLen) {
    BytesOpt b1(10);
    EXPECT_EQ(0, b1.getLen());
    b1.addByte(0xad);
    EXPECT_EQ(1, b1.getLen());
    b1.addrandom(5);
    EXPECT_EQ(6, b1.getLen());
    b1.addByte(0xad);
    EXPECT_EQ(7, b1.getLen());

    BytesOpt b2(0);
    EXPECT_EQ(0, b2.getLen());

    BytesOpt b3(10);
    BytesOpt b4(10);
    b3.addrandom(6);
    b3.copyToBytes(b4);
    EXPECT_EQ(6, b3.getLen());
    EXPECT_EQ(6, b4.getLen());
    b3.addrandom(4);
    EXPECT_EQ(10, b3.getLen());
    EXPECT_EQ(6, b4.getLen());
}

TEST(BytesOptClass, getMaxLen) {
    BytesOpt b1(10);
    EXPECT_EQ(10, b1.getMaxLen());
    b1.addByte(0xad);
    EXPECT_EQ(10, b1.getMaxLen());
    b1.addrandom(5);
    EXPECT_EQ(10, b1.getMaxLen());
    b1.addByte(0xad);
    EXPECT_EQ(10, b1.getMaxLen());

    BytesOpt b2(0);
    EXPECT_EQ(0, b2.getMaxLen());

    BytesOpt b3(10);
    BytesOpt b4(10);
    b3.addrandom(6);
    b3.copyToBytes(b4);
    EXPECT_EQ(10, b3.getMaxLen());
    EXPECT_EQ(10, b4.getMaxLen());
    b3.addrandom(4);
    EXPECT_EQ(10, b3.getMaxLen());
    EXPECT_EQ(10, b4.getMaxLen());

    BytesOpt b5(11);
    b5.addByte(0xad);
    BytesOpt b6(11);
    b5.copyToBytes(b6);
    EXPECT_EQ(11, b5.getMaxLen());
    EXPECT_EQ(11, b6.getMaxLen());
}

TEST(BytesOptClass, copyToArray) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char test[10];
    b1.consumeBytes(bytes1, 10);
    b1.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    BytesOpt b2(10);
    b2.fillrandom();
    b2.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b2.getBytes(), test, 10) == 0);

    BytesOpt b3(10);
    b3.addrandom(5);
    b3.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), test, 5) == 0);

    BytesOpt b4(10);
    b4.addByte(0xad);
    b4.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b4.getBytes(), test, 1) == 0);

    BytesOpt b5(0);
    b5.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), test, 0) == 0);

    // exception checks
    BytesOpt b6(11);
    b6.fillrandom();
    EXPECT_THROW(b6.copyToArray(test, 10), std::length_error);
    BytesOpt b7(2);
    b7.fillrandom();
    EXPECT_THROW(b7.copyToArray(test, 1), std::length_error);
    BytesOpt b8(10);
    b8.fillrandom();
    EXPECT_NO_THROW(b8.copyToArray(test, 10));
    BytesOpt b9(10);
    b9.addrandom(5);
    EXPECT_THROW(b9.copyToArray(test, 4), std::length_error);
    BytesOpt b10(10);
    b10.addrandom(5);
    EXPECT_THROW(b10.copyToArray(nullptr, 10), std::invalid_argument);
}

TEST(BytesOptClass, copyToBytes) {
    BytesOpt b1(10);
    BytesOpt b2(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    b1.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    BytesOpt b3(10);
    b3.fillrandom();
    b3.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b3.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b3, b2);
    EXPECT_NE(b1, b2);

    BytesOpt b4(10);
    b4.addrandom(5);
    b4.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b2.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b2.getLen());
    EXPECT_EQ(5, b2.getLen());
    EXPECT_EQ(b4.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b4, b2);
    EXPECT_NE(b3, b2);

    BytesOpt b5(5);
    b5.addByte(0xad);
    b5.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b2.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b2.getLen());
    EXPECT_EQ(1, b2.getLen());
    EXPECT_EQ(10, b2.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b2);
    EXPECT_NE(b4, b2);

    BytesOpt b6(0);
    b6.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b2.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b2.getLen());
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(10, b2.getMaxLen());
    EXPECT_EQ(b6, b2);
    EXPECT_NE(b5, b2);

    // exception checks
    BytesOpt b7(11);
    b7.fillrandom();
    EXPECT_THROW(b7.copyToBytes(b2), std::length_error);
    BytesOpt b8(2);
    b8.fillrandom();
    EXPECT_NO_THROW(b8.copyToBytes(b2));
    BytesOpt b9(11);
    b9.addrandom(10);
    BytesOpt b10(10);
    b10.addrandom(5);
    EXPECT_NO_THROW(b9.copyToBytes(b10));
    BytesOpt b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_THROW(b11.copyToBytes(b2), std::length_error);
}

TEST(BytesOptClass, copyConstructor) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    BytesOpt b2(b1);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    BytesOpt b3(10);
    b3.fillrandom();
    BytesOpt b21(b3);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b21.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b21.getLen());
    EXPECT_EQ(10, b21.getLen());
    EXPECT_EQ(b3.getMaxLen(), b21.getMaxLen());
    EXPECT_EQ(b3, b21);
    EXPECT_NE(b1, b21);

    BytesOpt b4(10);
    b4.addrandom(5);
    BytesOpt b22(b4);
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b22.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b22.getLen());
    EXPECT_EQ(5, b22.getLen());
    EXPECT_EQ(b4.getMaxLen(), b22.getMaxLen());
    EXPECT_EQ(b4, b22);
    EXPECT_NE(b3, b22);

    BytesOpt b5(5);
    b5.addByte(0xad);
    BytesOpt b23(b5);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b23.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b23.getLen());
    EXPECT_EQ(1, b23.getLen());
    EXPECT_EQ(b5.getMaxLen(), b23.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b23);
    EXPECT_NE(b4, b23);

    BytesOpt b6(0);
    BytesOpt b24(b6);
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b24.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b24.getLen());
    EXPECT_EQ(0, b24.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(b6.getMaxLen(), b24.getMaxLen());
    EXPECT_EQ(b6, b24);
    EXPECT_NE(b5, b24);

    // exception checks
    BytesOpt b7(11);
    b7.fillrandom();
    EXPECT_NO_THROW(BytesOpt b25(b7));
    BytesOpt b8(2);
    b8.fillrandom();
    EXPECT_NO_THROW(BytesOpt b26(b8));
    BytesOpt b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_NO_THROW(BytesOpt b27(b11));
    EXPECT_NO_THROW(BytesOpt b28(BytesOpt(BytesOpt(10))));
}

TEST(BytesOptClass, copyAssignmentconstructor) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    BytesOpt b2 = b1;
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    BytesOpt b3(10);
    b3.fillrandom();
    BytesOpt b21 = b3;
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b21.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b21.getLen());
    EXPECT_EQ(10, b21.getLen());
    EXPECT_EQ(b3.getMaxLen(), b21.getMaxLen());
    EXPECT_EQ(b3, b21);
    EXPECT_NE(b1, b21);

    BytesOpt b4(10);
    b4.addrandom(5);
    BytesOpt b22 = b4;
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b22.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b22.getLen());
    EXPECT_EQ(5, b22.getLen());
    EXPECT_EQ(b4.getMaxLen(), b22.getMaxLen());
    EXPECT_EQ(b4, b22);
    EXPECT_NE(b3, b22);

    BytesOpt b5(5);
    b5.addByte(0xad);
    BytesOpt b23 = b5;
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b23.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b23.getLen());
    EXPECT_EQ(1, b23.getLen());
    EXPECT_EQ(b5.getMaxLen(), b23.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b23);
    EXPECT_NE(b4, b23);

    BytesOpt b6(0);
    BytesOpt b24 = b6;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b24.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b24.getLen());
    EXPECT_EQ(0, b24.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(b6.getMaxLen(), b24.getMaxLen());
    EXPECT_EQ(b6, b24);
    EXPECT_NE(b5, b24);

    // exception checks
    BytesOpt b7(11);
    b7.fillrandom();
    EXPECT_NO_THROW(BytesOpt b25 = b7);
    BytesOpt b8(2);
    b8.fillrandom();
    EXPECT_NO_THROW(BytesOpt b26 = b8);
    BytesOpt b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_NO_THROW(BytesOpt b27 = b11);
    EXPECT_NO_THROW(BytesOpt b28 = b28);
}

TEST(BytesOptClass, copyAssignment) {
    BytesOpt b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    BytesOpt b2(10);
    b2 = b1;
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    BytesOpt b3(10);
    b3.fillrandom();
    b2 = b3;
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b3.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b3, b2);
    EXPECT_NE(b1, b2);

    BytesOpt b4(10);
    b4.addrandom(5);
    BytesOpt b22(5);
    b22 = b4;
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b22.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b22.getLen());
    EXPECT_EQ(5, b22.getLen());
    EXPECT_EQ(10, b4.getMaxLen());
    EXPECT_EQ(5, b22.getMaxLen());
    EXPECT_EQ(b4, b22);
    EXPECT_NE(b3, b22);

    BytesOpt b5(5);
    b5.addByte(0xad);
    b22 = b5;
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b22.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b22.getLen());
    EXPECT_EQ(1, b22.getLen());
    EXPECT_EQ(b5.getMaxLen(), b22.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b22);
    EXPECT_NE(b4, b22);

    BytesOpt b6(0);
    BytesOpt b24(0);
    b24 = b6;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b24.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b24.getLen());
    EXPECT_EQ(0, b24.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(b6.getMaxLen(), b24.getMaxLen());
    EXPECT_EQ(b6, b24);
    EXPECT_NE(b5, b24);

    BytesOpt b61(0);
    b22 = b61;
    EXPECT_TRUE(std::memcmp(b61.getBytes(), b22.getBytes(), 0) == 0);
    EXPECT_EQ(b61.getLen(), b22.getLen());
    EXPECT_EQ(0, b22.getLen());
    EXPECT_EQ(0, b61.getMaxLen());
    EXPECT_EQ(5, b22.getMaxLen());
    EXPECT_EQ(b61, b22);
    EXPECT_NE(b5, b22);

    // exception checks
    BytesOpt b7(11);
    b7.fillrandom();
    BytesOpt b25(10);
    EXPECT_THROW(b25 = b7, std::length_error);
    BytesOpt b8(2);
    b25.fillrandom();
    b8.fillrandom();
    EXPECT_NO_THROW(b25 = b8);
    BytesOpt b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_THROW(b25 = b11, std::length_error);
    EXPECT_NO_THROW(b25 = b25);
}

TEST(BytesOptClass, addcopyToBytes) {
    BytesOpt b1(10);
    BytesOpt b2(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    b1.addcopyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    BytesOpt b3(10);
    BytesOpt b4(10);
    b3.fillrandom();
    b3.addcopyToBytes(b4);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b4.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b4.getLen());
    EXPECT_EQ(10, b4.getLen());
    EXPECT_EQ(b3.getMaxLen(), b4.getMaxLen());
    EXPECT_EQ(b3, b4);

    BytesOpt b5(10);
    BytesOpt b6(10);
    b5.consumeBytes(bytes1, 5);
    b5.addcopyToBytes(b6);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b6.getBytes(), 5) == 0);
    EXPECT_EQ(b5.getLen(), b6.getLen());
    EXPECT_EQ(5, b6.getLen());
    EXPECT_EQ(b5.getMaxLen(), b6.getMaxLen());
    EXPECT_EQ(b5, b6);

    BytesOpt b7(10);
    BytesOpt b8(10);
    b7.consumeBytes(bytes1, 5);
    b8.consumeBytes(bytes1 + 5, 5);
    b8.addcopyToBytes(b7);
    EXPECT_TRUE(std::memcmp(b8.getBytes(), b7.getBytes() + 5, 5) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b7.getBytes(), 10) == 0);
    EXPECT_EQ(10, b7.getLen());
    EXPECT_EQ(5, b8.getLen());
    EXPECT_EQ(10, b7.getMaxLen());
    EXPECT_EQ(10, b8.getMaxLen());
    EXPECT_NE(b7, b8);

    BytesOpt b9(10);
    BytesOpt b10(10);
    b9.addcopyToBytes(b10);
    EXPECT_TRUE(std::memcmp(b9.getBytes(), b10.getBytes(), 0) == 0);
    EXPECT_EQ(b9.getLen(), b10.getLen());
    EXPECT_EQ(0, b10.getLen());
    EXPECT_EQ(b9.getMaxLen(), b10.getMaxLen());
    EXPECT_EQ(b9, b10);

    // exception checks
    BytesOpt b11(11);
    BytesOpt b12(10);
    b11.fillrandom();
    EXPECT_THROW(b11.addcopyToBytes(b12), std::length_error);
    BytesOpt b13(2);
    BytesOpt b14(10);
    b13.fillrandom();
    EXPECT_NO_THROW(b13.addcopyToBytes(b14));
    BytesOpt b15(11);
    BytesOpt b16(10);
    b15.addrandom(10);
    EXPECT_NO_THROW(b15.addcopyToBytes(b16));
    BytesOpt b17(11);
    BytesOpt b18(10);
    b17.addrandom(5);
    EXPECT_NO_THROW(b17.addcopyToBytes(b18));
    EXPECT_NO_THROW(b17.addcopyToBytes(b18));
    EXPECT_THROW(b17.addcopyToBytes(b18), std::length_error);
    BytesOpt b19(11);
}

TEST(BytesOptClass, copy) {
    BytesOpt b1(20);
    BytesOpt b2(20);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.consumeBytes(bytes1, 10);
    b1.copyToBytes(b2);
    b1.fillrandom();
    EXPECT_NE(b1, b2);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_FALSE(std::memcmp(b1.getBytes(), b2.getBytes(), 20) == 0);

    BytesOpt b3(20);
    BytesOpt b4(20);
    b4.addByte(0xad);
    b3.consumeBytes(bytes1, 10);
    b3.addcopyToBytes(b4);
    b3.fillrandom();
    EXPECT_NE(b3, b4);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b4.getBytes() + 1, 10) == 0);
    EXPECT_FALSE(std::memcmp(b3.getBytes(), b4.getBytes() + 1, 19) == 0);

    BytesOpt b5(20);
    unsigned char bytes2[20];
    b5.consumeBytes(bytes1, 10);
    b5.copyToArray(bytes2, 20);
    b5.fillrandom();
    EXPECT_TRUE(std::memcmp(b5.getBytes(), bytes2, 10) == 0);
    EXPECT_FALSE(std::memcmp(b5.getBytes(), bytes2, 20) == 0);
}

TEST(BytesOptClass, addByte){
    BytesOpt b1(10);
    b1.addByte(0xad);
    EXPECT_EQ(1, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    EXPECT_EQ(0xad, b1.getBytes()[0]);

    BytesOpt b3(10);
    b3.addByte(0xad);
    b3.addByte(0xac);
    EXPECT_EQ(2, b3.getLen());
    EXPECT_EQ(10, b3.getMaxLen());
    EXPECT_EQ(0xad, b3.getBytes()[0]);
    EXPECT_EQ(0xac, b3.getBytes()[1]);

    BytesOpt b5(256);
    for(int i = 0; i < 256; i++){
        b5.addByte(i);
        EXPECT_EQ(i + 1, b5.getLen());
        EXPECT_EQ(256, b5.getMaxLen());
    }
    for(int i = 0; i < 256; i++){
        EXPECT_EQ(i, b5.getBytes()[i]);
    }

    // exception checks
    BytesOpt b6(0);
    EXPECT_THROW(b6.addByte(0xad), std::length_error);
    BytesOpt b7(10);
    EXPECT_NO_THROW(b7.addByte(0xad));
    BytesOpt b8(255);
    
    for(int i = 0; i < 256; i++){
        if(i < 255){
            EXPECT_NO_THROW(b8.addByte(i));
        }else{
            EXPECT_THROW(b8.addByte(i), std::length_error);
        }
    }
}

TEST(BytesOptClass, isEmpty){
    BytesOpt b1(10);
    EXPECT_TRUE(b1.isEmpty());
    b1.addByte(0xad);
    EXPECT_FALSE(b1.isEmpty());
    b1.fillrandom();
    EXPECT_FALSE(b1.isEmpty());

    BytesOpt b2(0);
    EXPECT_TRUE(b2.isEmpty());
}

TEST(BytesOptClass, toLong){
    BytesOpt b0(8);
    EXPECT_EQ(0, b0.toLong());
    BytesOpt b1(0);
    EXPECT_EQ(0, b1.toLong());
    BytesOpt b2(1);
    b2.addByte(0xad);
    EXPECT_EQ(0xad, b2.toLong());
    BytesOpt b3(2);
    b3.addByte(0xad);
    b3.addByte(0xac);
    EXPECT_EQ(0xadac, b3.toLong());
    BytesOpt b4(3);
    b4.addByte(0xad);
    b4.addByte(0xac);
    b4.addByte(0xab);
    EXPECT_EQ(0xadacab, b4.toLong());
    BytesOpt b5(4);
    b5.addByte(0xad);
    b5.addByte(0xac);
    b5.addByte(0xab);
    b5.addByte(0xaa);
    EXPECT_EQ(0xadacabaa, b5.toLong());
    BytesOpt b6(5);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x11);
    b6.addByte(0x22);
    EXPECT_EQ(0x0000001122, b6.toLong());
    BytesOpt b7(6);
    b7.addByte(0x45);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x11);
    b7.addByte(0x22);
    EXPECT_EQ(0x450000001122, b7.toLong());
    BytesOpt b8(7);
    b8.addByte(0x00);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    EXPECT_EQ(0x00ffffffffffff, b8.toLong());
    BytesOpt b9(8);
    b9.addByte(0xff);
    b9.addByte(0xfe);
    b9.addByte(0xfd);
    b9.addByte(0xfc);
    b9.addByte(0xfb);
    b9.addByte(0xfa);
    b9.addByte(0xf9);
    b9.addByte(0xf8);   
    EXPECT_EQ(0xfffefdfcfbfaf9f8, b9.toLong());

    // exception checks
    BytesOpt b10(9);
    b10.fillrandom();
    EXPECT_THROW(b10.toLong(), std::length_error);

    BytesOpt b11(9);
    b11.addrandom(8);
    EXPECT_NO_THROW(b11.toLong());

    BytesOpt b12(10);
    b12.addrandom(9);
    EXPECT_THROW(b12.toLong(), std::length_error);
}

TEST(BytesOptClass, toHex){
    BytesOpt b1(0);
    EXPECT_EQ("", b1.toHex());
    BytesOpt b2(1);
    b2.addByte(0xad);
    EXPECT_EQ("AD", b2.toHex());
    BytesOpt b3(2);
    b3.addByte(0xad);
    b3.addByte(0xac);
    EXPECT_EQ("ADAC", b3.toHex());
    BytesOpt b4(3);
    b4.addByte(0xad);
    b4.addByte(0xac);
    b4.addByte(0xab);
    EXPECT_EQ("ADACAB", b4.toHex());
    BytesOpt b5(4);
    b5.addByte(0xad);
    b5.addByte(0xac);
    b5.addByte(0xab);
    b5.addByte(0xaa);
    EXPECT_EQ("ADACABAA", b5.toHex());
    BytesOpt b6(5);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x11);
    b6.addByte(0x22);
    EXPECT_EQ("0000001122", b6.toHex());
    BytesOpt b7(6);
    b7.addByte(0x45);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x11);
    b7.addByte(0x22);
    EXPECT_EQ("450000001122", b7.toHex());

    BytesOpt b8(256);
    for(int i = 0; i < 256; i++){
        b8.addByte(i);
    }
    std::string hex = b8.toHex();
    for(int i = 0; i < 256; i++){
        std::stringstream ss;
        ss << std::uppercase << std::hex << i;
        std::string hex2 = ss.str();
        if(hex2.length() == 1){
            hex2 = "0" + hex2;
        }
        EXPECT_EQ(hex2, hex.substr(i * 2, 2));
    }
}

TEST(BytesOptClass, fromLong){
    u_int64_t l1 = 0;
    BytesOpt b1 = BytesOpt::fromLong(l1);
    EXPECT_EQ(0, b1.getLen());
    EXPECT_EQ(8, b1.getMaxLen());
    EXPECT_EQ(0, b1.toLong());

    u_int64_t l2 = 0xad;
    BytesOpt b2 = BytesOpt::fromLong(l2);
    EXPECT_EQ(1, b2.getLen());
    EXPECT_EQ(8, b2.getMaxLen());
    EXPECT_EQ(0xad, b2.toLong());

    u_int64_t l3 = 0xadac;
    BytesOpt b3 = BytesOpt::fromLong(l3);
    EXPECT_EQ(2, b3.getLen());
    EXPECT_EQ(8, b3.getMaxLen());
    EXPECT_EQ(0xadac, b3.toLong());

    u_int64_t l4 = 0xadacab;
    BytesOpt b4 = BytesOpt::fromLong(l4);
    EXPECT_EQ(3, b4.getLen());
    EXPECT_EQ(8, b4.getMaxLen());
    EXPECT_EQ(0xadacab, b4.toLong());

    u_int64_t l5 = 0x00ac00aa;
    BytesOpt b5 = BytesOpt::fromLong(l5);
    EXPECT_EQ(3, b5.getLen());
    EXPECT_EQ(8, b5.getMaxLen());
    EXPECT_EQ(0xac00aa, b5.toLong());

    u_int64_t l6 = 0x0000001122;
    BytesOpt b6 = BytesOpt::fromLong(l6);
    EXPECT_EQ(2, b6.getLen());
    EXPECT_EQ(8, b6.getMaxLen());
    EXPECT_EQ(0x1122, b6.toLong());

    u_int64_t l7 = 0x450000001122;
    BytesOpt b7 = BytesOpt::fromLong(l7);
    EXPECT_EQ(6, b7.getLen());
    EXPECT_EQ(8, b7.getMaxLen());
    EXPECT_EQ(0x450000001122, b7.toLong());

    u_int64_t l8 = 0x00ffffffffffff;
    BytesOpt b8 = BytesOpt::fromLong(l8);
    EXPECT_EQ(6, b8.getLen());
    EXPECT_EQ(8, b8.getMaxLen());
    EXPECT_EQ(0xffffffffffff, b8.toLong());

    u_int64_t l9 = 0xfffefdfcfbfaf9f8;
    BytesOpt b9 = BytesOpt::fromLong(l9);
    EXPECT_EQ(8, b9.getLen());
    EXPECT_EQ(8, b9.getMaxLen());
    EXPECT_EQ(0xfffefdfcfbfaf9f8, b9.toLong());

    u_int64_t l10 = 0xfffefdfcfbfaf9f;
    BytesOpt b10 = BytesOpt::fromLong(l10);
    EXPECT_EQ(8, b10.getLen());
    EXPECT_EQ(8, b10.getMaxLen());
    EXPECT_EQ(0xfffefdfcfbfaf9f, b10.toLong());

    // exception checks
    u_int64_t l15 = 0x00fffefdfcfbfaf9f8;
    EXPECT_NO_THROW(BytesOpt b15 = BytesOpt::fromLong(l15));

    u_int64_t l16 = 0x0fffefdfcfbfaf9f;
    EXPECT_NO_THROW(BytesOpt b16 = BytesOpt::fromLong(l16));

    u_int64_t l17 = 0x0fffefdfcfbfaf9f8;
    EXPECT_NO_THROW(BytesOpt b17 = BytesOpt::fromLong(l17));
}

TEST(BytesOptClass, equal){
    BytesOpt b1(10);
    BytesOpt b2(8);
    EXPECT_EQ(b1, b2);
    b1.addByte(0xad);
    EXPECT_NE(b1, b2);
    b2.addByte(0xad);
    EXPECT_EQ(b1, b2);
    b1.addByte(0xac);
    EXPECT_NE(b1, b2);
    b2.addByte(0xac);
    EXPECT_EQ(b1, b2);
    b1.fillrandom();
    b2.fillrandom();
    EXPECT_NE(b1, b2);

    BytesOpt b3(0);
    BytesOpt b4(10000);
    EXPECT_EQ(b3, b4);
    b4.addByte(0xad);
    EXPECT_NE(b3, b4);

    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    BytesOpt b5(15);
    BytesOpt b6(11);
    EXPECT_EQ(b5, b6);
    b5.consumeBytes(bytes1, 10);
    EXPECT_NE(b5, b6);
    b6.consumeBytes(bytes1, 10);
    EXPECT_EQ(b5, b6);
}

TEST(BytesOptClass, plus){
    for(int i = 0; i < 1000; i++){
        BytesOpt b1(10);
        BytesOpt b2(10);
        b1.fillrandom();
        b2.fillrandom();
        BytesOpt b3 = b1 + b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(10, b3.getMaxLen());
        
        for(int j = 0; j < 10; j++){
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] + b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    for(int i = 0; i < 1000; i++){
        BytesOpt b1(5);
        BytesOpt b2(10);
        b1.addrandom(5);
        b2.addrandom(5);
        BytesOpt b3 = b1 + b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(5, b3.getMaxLen());
        
        for(int j = 0; j < 5; j++){
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] + b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    BytesOpt b1(0);
    BytesOpt b2(0);
    BytesOpt b3 = b1 + b2;
    EXPECT_EQ(b1.getLen(), b3.getLen());
    EXPECT_EQ(b2.getLen(), b3.getLen());
    EXPECT_EQ(0, b3.getMaxLen());

    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char bytes2[10] = {126, 255, 255, 34, 17, 0, 0, 0, 0, 253};
    unsigned char bytes3[10] = {126, 0, 1, 37, 21, 5, 6, 7, 8, 6};

    BytesOpt b4(10);
    BytesOpt b5(10);
    b4.consumeBytes(bytes1, 10);
    b5.consumeBytes(bytes2, 10);
    BytesOpt b6 = b4 + b5;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), bytes3, 10) == 0);

    // exception checks
    BytesOpt b7(11);
    BytesOpt b8(10);
    b7.fillrandom();
    EXPECT_THROW(b7 + b8, std::length_error);
    b8.fillrandom();
    EXPECT_THROW(b7 + b8, std::length_error);
    BytesOpt b9(2);
    BytesOpt b10(10);
    b9.fillrandom();
    b10.addrandom(2);
    EXPECT_NO_THROW(b9 + b10);
    BytesOpt b11(0);
    BytesOpt b12(10);
    EXPECT_NO_THROW(b11 + b12);
    b12.addByte(0xad);
    EXPECT_THROW(b11 + b12, std::length_error);

}

TEST(BytesOptClass, minus){
    for(int i = 0; i < 1000; i++){
        BytesOpt b1(10);
        BytesOpt b2(10);
        b1.fillrandom();
        b2.fillrandom();
        BytesOpt b3 = b1 - b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(10, b3.getMaxLen());
        
        for(int j = 0; j < 10; j++){
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] - b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    for(int i = 0; i < 1000; i++){
        BytesOpt b1(5);
        BytesOpt b2(10);
        b1.addrandom(5);
        b2.addrandom(5);
        BytesOpt b3 = b1 - b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(5, b3.getMaxLen());
        
        for(int j = 0; j < 5; j++){
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] - b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    BytesOpt b1(0);
    BytesOpt b2(0);
    BytesOpt b3 = b1 - b2;
    EXPECT_EQ(b1.getLen(), b3.getLen());
    EXPECT_EQ(b2.getLen(), b3.getLen());
    EXPECT_EQ(0, b3.getMaxLen());

    unsigned char bytes1[10] = {126, 0, 1, 37, 21, 5, 6, 7, 8, 6};
    unsigned char bytes2[10] = {126, 255, 255, 34, 17, 0, 0, 0, 0, 253};
    unsigned char bytes3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    BytesOpt b4(10);
    BytesOpt b5(10);
    b4.consumeBytes(bytes1, 10);
    b5.consumeBytes(bytes2, 10);
    BytesOpt b6 = b4 - b5;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), bytes3, 10) == 0);

    // exception checks
    BytesOpt b7(11);
    BytesOpt b8(10);
    b7.fillrandom();
    EXPECT_THROW(b7 - b8, std::length_error);
    b8.fillrandom();
    EXPECT_THROW(b7 - b8, std::length_error);
    BytesOpt b9(2);
    BytesOpt b10(10);
    b9.fillrandom();
    b10.addrandom(2);
    EXPECT_NO_THROW(b9 - b10);
    BytesOpt b11(0);
    BytesOpt b12(10);
    EXPECT_NO_THROW(b11 - b12);
    b12.addByte(0xad);
    EXPECT_THROW(b11 - b12, std::length_error);
}
