#include "bytes_opt.h"

#include <gtest/gtest.h>

#include <cstring>
#include <memory>

TEST(BytesClass, constructor) {
    Bytes b1(10);
    EXPECT_EQ(0, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    Bytes b2(0);
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b2.getMaxLen());
    Bytes b3(1000);
    EXPECT_EQ(0, b3.getLen());
    EXPECT_EQ(1000, b3.getMaxLen());

    EXPECT_THROW(Bytes b4(-1), std::invalid_argument);
}

TEST(BytesClass, fillrandom) {
    // len checks
    Bytes b1(10);
    b1.fillrandom();
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    Bytes b2(0);
    b2.fillrandom();
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b2.getMaxLen());
    Bytes b3(1000);
    b3.fillrandom();
    EXPECT_EQ(1000, b3.getLen());
    EXPECT_EQ(1000, b3.getMaxLen());

    Bytes b4(100);
    b4.addByte(0xad);
    b4.fillrandom();
    EXPECT_EQ(100, b4.getLen());
    EXPECT_EQ(100, b4.getMaxLen());
    EXPECT_EQ(0xad, b4.getBytes()[0]);

    Bytes b5(100);
    b5.addrandom(10);
    Bytes b6(100);
    b5.copyToBytes(b6);
    b5.fillrandom();
    EXPECT_EQ(100, b5.getLen());
    EXPECT_EQ(100, b5.getMaxLen());
    EXPECT_EQ(10, b6.getLen());
    EXPECT_EQ(100, b6.getMaxLen());
    EXPECT_NE(b5, b6);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b6.getBytes(), 10) == 0);

    Bytes b7(100);
    b7.addrandom(100);
    Bytes b8(100);
    b7.copyToBytes(b8);
    b7.fillrandom();
    b8.fillrandom();
    EXPECT_EQ(100, b7.getLen());
    EXPECT_EQ(100, b7.getMaxLen());
    EXPECT_EQ(100, b8.getLen());
    EXPECT_EQ(100, b8.getMaxLen());
    EXPECT_EQ(b7, b8);
    EXPECT_TRUE(std::memcmp(b7.getBytes(), b8.getBytes(), 100) == 0);

    Bytes b9(100);
    b9.fillrandom();
    Bytes b10(100);
    b9.copyToBytes(b10);
    b9.fillrandom();
    EXPECT_EQ(100, b9.getLen());
    EXPECT_EQ(100, b9.getMaxLen());
    EXPECT_EQ(100, b10.getLen());
    EXPECT_EQ(100, b10.getMaxLen());
    EXPECT_EQ(b9, b10);
    EXPECT_TRUE(std::memcmp(b9.getBytes(), b10.getBytes(), 100) == 0);

    // random checks
    Bytes b11(100);
    b4.fillrandom();
    Bytes b12(100);
    b5.fillrandom();
    EXPECT_NE(b4, b5);
}

TEST(BytesClass, addrandom) {
    // len checks
    Bytes b1(10);
    b1.addrandom(10);
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    Bytes b2(0);
    b2.addrandom(0);
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b2.getMaxLen());
    Bytes b3(1000);
    b3.addrandom(1000);
    EXPECT_EQ(1000, b3.getLen());
    EXPECT_EQ(1000, b3.getMaxLen());

    Bytes b4(100);
    b4.addrandom(10);
    EXPECT_EQ(10, b4.getLen());
    EXPECT_EQ(100, b4.getMaxLen());
    Bytes b5(100);
    b5.addrandom(0);
    EXPECT_EQ(0, b5.getLen());
    EXPECT_EQ(100, b5.getMaxLen());

    // random checks
    Bytes b6(100);
    b6.addrandom(100);
    Bytes b7(100);
    b7.fillrandom();
    EXPECT_NE(b6, b7);

    Bytes b8(100);
    b8.addrandom(10);
    Bytes b9(100);
    b9.addrandom(10);
    EXPECT_NE(b8, b9);

    Bytes b10(100);
    b10.addrandom(10);
    Bytes b11(100);
    b10.copyToBytes(b11);
    EXPECT_EQ(b10, b11);
    b10.addrandom(10);
    EXPECT_NE(b10, b11);
    b11.addrandom(10);
    EXPECT_NE(b10, b11);

    Bytes b12(100);
    b12.fillrandom();
    Bytes b13(100);
    b12.copyToBytes(b13);
    EXPECT_EQ(b12, b13);
    EXPECT_THROW(b12.addrandom(10), std::length_error);
    EXPECT_THROW(b13.addrandom(10), std::length_error);

    Bytes b14(100);
    b14.addrandom(100);
    Bytes b15(100);
    b14.copyToBytes(b15);
    EXPECT_EQ(b14, b15);
    EXPECT_THROW(b14.addrandom(10), std::length_error);
    EXPECT_THROW(b15.addrandom(10), std::length_error);

    // exception checks
    Bytes b16(100);
    EXPECT_THROW(b16.addrandom(101), std::length_error);

    Bytes b17(100);
    EXPECT_THROW(b17.addrandom(-1), std::length_error);

    Bytes b18(100);
    b18.addrandom(99);
    EXPECT_THROW(b18.addrandom(2), std::length_error);

    Bytes b19(100);
    b19.addrandom(100);
    EXPECT_THROW(b19.addrandom(1), std::length_error);

    Bytes b20(100);
    b20.addrandom(99);
    EXPECT_NO_THROW(b20.addrandom(1));
}

TEST(BytesClass, setBytes) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char test[10];
    b1.setBytes(bytes1, 10);
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    std::memcpy(test, b1.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    Bytes b2(10);
    b2.fillrandom();
    b2.setBytes(bytes1, 10);
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(10, b2.getMaxLen());
    std::memcpy(test, b2.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    Bytes b3(10);
    b3.addrandom(5);
    b3.setBytes(bytes1, 10);
    EXPECT_EQ(10, b3.getLen());
    EXPECT_EQ(10, b3.getMaxLen());
    std::memcpy(test, b3.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    Bytes b4(10);
    b4.addByte(0xad);
    b4.setBytes(bytes1, 9);
    EXPECT_EQ(9, b4.getLen());
    EXPECT_EQ(10, b4.getMaxLen());
    std::memcpy(test, b4.getBytes(), 9);
    test[9] = 9;
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    // exception checks
    unsigned char bytes2[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Bytes b5(10);
    EXPECT_THROW(b5.setBytes(bytes2, 11), std::length_error);
    Bytes b6(0);
    EXPECT_THROW(b6.setBytes(bytes2, 11), std::length_error);
    Bytes b7(11);
    EXPECT_NO_THROW(b7.setBytes(bytes2, 11));
    Bytes b8(11);
    EXPECT_THROW(b8.setBytes(nullptr, 1), std::invalid_argument);
}

TEST(BytesClass, addBytes) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char test[10];
    b1.addBytes(bytes1, 10);
    EXPECT_EQ(10, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    std::memcpy(test, b1.getBytes(), 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    Bytes b2(10);
    b2.addrandom(5);
    b2.addBytes(bytes1, 5);
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(10, b2.getMaxLen());
    std::memcpy(test, b2.getBytes() + 5, 5);
    EXPECT_TRUE(std::memcmp(bytes1, test, 5) == 0);

    Bytes b3(10);
    b3.addByte(0xad);
    b3.addBytes(bytes1, 5);
    EXPECT_EQ(6, b3.getLen());
    EXPECT_EQ(10, b3.getMaxLen());
    std::memcpy(test + 1, bytes1, 5);
    test[0] = 0xad;
    EXPECT_TRUE(std::memcmp(b3.getBytes(), test, 6) == 0);

    // exception checks
    unsigned char bytes2[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Bytes b4(10);
    EXPECT_THROW(b4.addBytes(bytes2, 11), std::length_error);
    Bytes b5(0);
    EXPECT_THROW(b5.addBytes(bytes2, 11), std::length_error);
    Bytes b6(11);
    EXPECT_NO_THROW(b6.addBytes(bytes2, 11));
    Bytes b7(11);
    b7.addByte(0xad);
    EXPECT_THROW(b7.addBytes(bytes2, 11), std::length_error);
    Bytes b8(11);
    EXPECT_THROW(b8.addBytes(nullptr, 1), std::invalid_argument);
}

TEST(BytesClass, getBytes) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    unsigned char* test = b1.getBytes();
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    Bytes b2(10);
    b2.fillrandom();
    test = b2.getBytes();
    EXPECT_TRUE(std::memcmp(b2.getBytes(), test, 10) == 0);

    Bytes b3(10);
    b3.addrandom(5);
    test = b3.getBytes();
    EXPECT_TRUE(std::memcmp(b3.getBytes(), test, 5) == 0);

    Bytes b4(10);
    b4.addByte(0xad);
    test = b4.getBytes();
    EXPECT_TRUE(std::memcmp(b4.getBytes(), test, 1) == 0);

    Bytes b5(0);
    test = b5.getBytes();
    EXPECT_TRUE(std::memcmp(b5.getBytes(), test, 0) == 0);
}

TEST(BytesClass, getLen) {
    Bytes b1(10);
    EXPECT_EQ(0, b1.getLen());
    b1.addByte(0xad);
    EXPECT_EQ(1, b1.getLen());
    b1.addrandom(5);
    EXPECT_EQ(6, b1.getLen());
    b1.addByte(0xad);
    EXPECT_EQ(7, b1.getLen());

    Bytes b2(0);
    EXPECT_EQ(0, b2.getLen());

    Bytes b3(10);
    Bytes b4(10);
    b3.addrandom(6);
    b3.copyToBytes(b4);
    EXPECT_EQ(6, b3.getLen());
    EXPECT_EQ(6, b4.getLen());
    b3.addrandom(4);
    EXPECT_EQ(10, b3.getLen());
    EXPECT_EQ(6, b4.getLen());
}

TEST(BytesClass, getMaxLen) {
    Bytes b1(10);
    EXPECT_EQ(10, b1.getMaxLen());
    b1.addByte(0xad);
    EXPECT_EQ(10, b1.getMaxLen());
    b1.addrandom(5);
    EXPECT_EQ(10, b1.getMaxLen());
    b1.addByte(0xad);
    EXPECT_EQ(10, b1.getMaxLen());

    Bytes b2(0);
    EXPECT_EQ(0, b2.getMaxLen());

    Bytes b3(10);
    Bytes b4(10);
    b3.addrandom(6);
    b3.copyToBytes(b4);
    EXPECT_EQ(10, b3.getMaxLen());
    EXPECT_EQ(10, b4.getMaxLen());
    b3.addrandom(4);
    EXPECT_EQ(10, b3.getMaxLen());
    EXPECT_EQ(10, b4.getMaxLen());

    Bytes b5(11);
    b5.addByte(0xad);
    Bytes b6(11);
    b5.copyToBytes(b6);
    EXPECT_EQ(11, b5.getMaxLen());
    EXPECT_EQ(11, b6.getMaxLen());
}

TEST(BytesClass, copyToArray) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char test[10];
    b1.setBytes(bytes1, 10);
    b1.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(bytes1, test, 10) == 0);

    Bytes b2(10);
    b2.fillrandom();
    b2.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b2.getBytes(), test, 10) == 0);

    Bytes b3(10);
    b3.addrandom(5);
    b3.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), test, 5) == 0);

    Bytes b4(10);
    b4.addByte(0xad);
    b4.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b4.getBytes(), test, 1) == 0);

    Bytes b5(0);
    b5.copyToArray(test, 10);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), test, 0) == 0);

    // exception checks
    Bytes b6(11);
    b6.fillrandom();
    EXPECT_THROW(b6.copyToArray(test, 10), std::length_error);
    Bytes b7(2);
    b7.fillrandom();
    EXPECT_THROW(b7.copyToArray(test, 1), std::length_error);
    Bytes b8(10);
    b8.fillrandom();
    EXPECT_NO_THROW(b8.copyToArray(test, 10));
    Bytes b9(10);
    b9.addrandom(5);
    EXPECT_THROW(b9.copyToArray(test, 4), std::length_error);
    Bytes b10(10);
    b10.addrandom(5);
    EXPECT_THROW(b10.copyToArray(nullptr, 10), std::invalid_argument);
}

TEST(BytesClass, copyToBytes) {
    Bytes b1(10);
    Bytes b2(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    b1.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    Bytes b3(10);
    b3.fillrandom();
    b3.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b3.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b3, b2);
    EXPECT_NE(b1, b2);

    Bytes b4(10);
    b4.addrandom(5);
    b4.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b2.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b2.getLen());
    EXPECT_EQ(5, b2.getLen());
    EXPECT_EQ(b4.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b4, b2);
    EXPECT_NE(b3, b2);

    Bytes b5(5);
    b5.addByte(0xad);
    b5.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b2.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b2.getLen());
    EXPECT_EQ(1, b2.getLen());
    EXPECT_EQ(10, b2.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b2);
    EXPECT_NE(b4, b2);

    Bytes b6(0);
    b6.copyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b2.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b2.getLen());
    EXPECT_EQ(0, b2.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(10, b2.getMaxLen());
    EXPECT_EQ(b6, b2);
    EXPECT_NE(b5, b2);

    // exception checks
    Bytes b7(11);
    b7.fillrandom();
    EXPECT_THROW(b7.copyToBytes(b2), std::length_error);
    Bytes b8(2);
    b8.fillrandom();
    EXPECT_NO_THROW(b8.copyToBytes(b2));
    Bytes b9(11);
    b9.addrandom(10);
    Bytes b10(10);
    b10.addrandom(5);
    EXPECT_NO_THROW(b9.copyToBytes(b10));
    Bytes b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_THROW(b11.copyToBytes(b2), std::length_error);
}

TEST(BytesClass, copyConstructor) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    Bytes b2(b1);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    Bytes b3(10);
    b3.fillrandom();
    Bytes b21(b3);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b21.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b21.getLen());
    EXPECT_EQ(10, b21.getLen());
    EXPECT_EQ(b3.getMaxLen(), b21.getMaxLen());
    EXPECT_EQ(b3, b21);
    EXPECT_NE(b1, b21);

    Bytes b4(10);
    b4.addrandom(5);
    Bytes b22(b4);
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b22.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b22.getLen());
    EXPECT_EQ(5, b22.getLen());
    EXPECT_EQ(b4.getMaxLen(), b22.getMaxLen());
    EXPECT_EQ(b4, b22);
    EXPECT_NE(b3, b22);

    Bytes b5(5);
    b5.addByte(0xad);
    Bytes b23(b5);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b23.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b23.getLen());
    EXPECT_EQ(1, b23.getLen());
    EXPECT_EQ(b5.getMaxLen(), b23.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b23);
    EXPECT_NE(b4, b23);

    Bytes b6(0);
    Bytes b24(b6);
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b24.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b24.getLen());
    EXPECT_EQ(0, b24.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(b6.getMaxLen(), b24.getMaxLen());
    EXPECT_EQ(b6, b24);
    EXPECT_NE(b5, b24);

    // exception checks
    Bytes b7(11);
    b7.fillrandom();
    EXPECT_NO_THROW(Bytes b25(b7));
    Bytes b8(2);
    b8.fillrandom();
    EXPECT_NO_THROW(Bytes b26(b8));
    Bytes b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_NO_THROW(Bytes b27(b11));
    EXPECT_NO_THROW(Bytes b28(Bytes(Bytes(10))));
}

TEST(BytesClass, copyAssignmentconstructor) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    Bytes b2 = b1;
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    Bytes b3(10);
    b3.fillrandom();
    Bytes b21 = b3;
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b21.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b21.getLen());
    EXPECT_EQ(10, b21.getLen());
    EXPECT_EQ(b3.getMaxLen(), b21.getMaxLen());
    EXPECT_EQ(b3, b21);
    EXPECT_NE(b1, b21);

    Bytes b4(10);
    b4.addrandom(5);
    Bytes b22 = b4;
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b22.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b22.getLen());
    EXPECT_EQ(5, b22.getLen());
    EXPECT_EQ(b4.getMaxLen(), b22.getMaxLen());
    EXPECT_EQ(b4, b22);
    EXPECT_NE(b3, b22);

    Bytes b5(5);
    b5.addByte(0xad);
    Bytes b23 = b5;
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b23.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b23.getLen());
    EXPECT_EQ(1, b23.getLen());
    EXPECT_EQ(b5.getMaxLen(), b23.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b23);
    EXPECT_NE(b4, b23);

    Bytes b6(0);
    Bytes b24 = b6;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b24.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b24.getLen());
    EXPECT_EQ(0, b24.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(b6.getMaxLen(), b24.getMaxLen());
    EXPECT_EQ(b6, b24);
    EXPECT_NE(b5, b24);

    // exception checks
    Bytes b7(11);
    b7.fillrandom();
    EXPECT_NO_THROW(Bytes b25 = b7);
    Bytes b8(2);
    b8.fillrandom();
    EXPECT_NO_THROW(Bytes b26 = b8);
    Bytes b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_NO_THROW(Bytes b27 = b11);
    EXPECT_NO_THROW(Bytes b28 = b28);
}

TEST(BytesClass, copyAssignment) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    Bytes b2(10);
    b2 = b1;
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    Bytes b3(10);
    b3.fillrandom();
    b2 = b3;
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b3.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b3, b2);
    EXPECT_NE(b1, b2);

    Bytes b4(10);
    b4.addrandom(5);
    Bytes b22(5);
    b22 = b4;
    EXPECT_TRUE(std::memcmp(b4.getBytes(), b22.getBytes(), 5) == 0);
    EXPECT_EQ(b4.getLen(), b22.getLen());
    EXPECT_EQ(5, b22.getLen());
    EXPECT_EQ(10, b4.getMaxLen());
    EXPECT_EQ(5, b22.getMaxLen());
    EXPECT_EQ(b4, b22);
    EXPECT_NE(b3, b22);

    Bytes b5(5);
    b5.addByte(0xad);
    b22 = b5;
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b22.getBytes(), 1) == 0);
    EXPECT_EQ(b5.getLen(), b22.getLen());
    EXPECT_EQ(1, b22.getLen());
    EXPECT_EQ(b5.getMaxLen(), b22.getMaxLen());
    EXPECT_EQ(5, b5.getMaxLen());
    EXPECT_EQ(b5, b22);
    EXPECT_NE(b4, b22);

    Bytes b6(0);
    Bytes b24(0);
    b24 = b6;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), b24.getBytes(), 0) == 0);
    EXPECT_EQ(b6.getLen(), b24.getLen());
    EXPECT_EQ(0, b24.getLen());
    EXPECT_EQ(0, b6.getMaxLen());
    EXPECT_EQ(b6.getMaxLen(), b24.getMaxLen());
    EXPECT_EQ(b6, b24);
    EXPECT_NE(b5, b24);

    Bytes b61(0);
    b22 = b61;
    EXPECT_TRUE(std::memcmp(b61.getBytes(), b22.getBytes(), 0) == 0);
    EXPECT_EQ(b61.getLen(), b22.getLen());
    EXPECT_EQ(0, b22.getLen());
    EXPECT_EQ(0, b61.getMaxLen());
    EXPECT_EQ(5, b22.getMaxLen());
    EXPECT_EQ(b61, b22);
    EXPECT_NE(b5, b22);

    // exception checks
    Bytes b7(11);
    b7.fillrandom();
    Bytes b25(10);
    EXPECT_THROW(b25 = b7, std::length_error);
    Bytes b8(2);
    b25.fillrandom();
    b8.fillrandom();
    EXPECT_NO_THROW(b25 = b8);
    Bytes b11(11);
    b11.addrandom(5);
    b11.addrandom(6);
    EXPECT_THROW(b25 = b11, std::length_error);
    EXPECT_NO_THROW(b25 = b25);
}

TEST(BytesClass, addcopyToBytes) {
    Bytes b1(10);
    Bytes b2(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    b1.addcopyToBytes(b2);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b2.getBytes(), 10) == 0);
    EXPECT_EQ(b1.getLen(), b2.getLen());
    EXPECT_EQ(10, b2.getLen());
    EXPECT_EQ(b1.getMaxLen(), b2.getMaxLen());
    EXPECT_EQ(b1, b2);

    Bytes b3(10);
    Bytes b4(10);
    b3.fillrandom();
    b3.addcopyToBytes(b4);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b4.getBytes(), 10) == 0);
    EXPECT_EQ(b3.getLen(), b4.getLen());
    EXPECT_EQ(10, b4.getLen());
    EXPECT_EQ(b3.getMaxLen(), b4.getMaxLen());
    EXPECT_EQ(b3, b4);

    Bytes b5(10);
    Bytes b6(10);
    b5.setBytes(bytes1, 5);
    b5.addcopyToBytes(b6);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b6.getBytes(), 5) == 0);
    EXPECT_EQ(b5.getLen(), b6.getLen());
    EXPECT_EQ(5, b6.getLen());
    EXPECT_EQ(b5.getMaxLen(), b6.getMaxLen());
    EXPECT_EQ(b5, b6);

    Bytes b7(10);
    Bytes b8(10);
    b7.setBytes(bytes1, 5);
    b8.setBytes(bytes1 + 5, 5);
    b8.addcopyToBytes(b7);
    EXPECT_TRUE(std::memcmp(b8.getBytes(), b7.getBytes() + 5, 5) == 0);
    EXPECT_TRUE(std::memcmp(bytes1, b7.getBytes(), 10) == 0);
    EXPECT_EQ(10, b7.getLen());
    EXPECT_EQ(5, b8.getLen());
    EXPECT_EQ(10, b7.getMaxLen());
    EXPECT_EQ(10, b8.getMaxLen());
    EXPECT_NE(b7, b8);

    Bytes b9(10);
    Bytes b10(10);
    b9.addcopyToBytes(b10);
    EXPECT_TRUE(std::memcmp(b9.getBytes(), b10.getBytes(), 0) == 0);
    EXPECT_EQ(b9.getLen(), b10.getLen());
    EXPECT_EQ(0, b10.getLen());
    EXPECT_EQ(b9.getMaxLen(), b10.getMaxLen());
    EXPECT_EQ(b9, b10);

    // exception checks
    Bytes b11(11);
    Bytes b12(10);
    b11.fillrandom();
    EXPECT_THROW(b11.addcopyToBytes(b12), std::length_error);
    Bytes b13(2);
    Bytes b14(10);
    b13.fillrandom();
    EXPECT_NO_THROW(b13.addcopyToBytes(b14));
    Bytes b15(11);
    Bytes b16(10);
    b15.addrandom(10);
    EXPECT_NO_THROW(b15.addcopyToBytes(b16));
    Bytes b17(11);
    Bytes b18(10);
    b17.addrandom(5);
    EXPECT_NO_THROW(b17.addcopyToBytes(b18));
    EXPECT_NO_THROW(b17.addcopyToBytes(b18));
    EXPECT_THROW(b17.addcopyToBytes(b18), std::length_error);
    Bytes b19(11);
}

TEST(BytesClass, copySubBytes) {
    Bytes b1(10);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    Bytes b2 = b1.copySubBytes(2, 5);
    EXPECT_TRUE(std::memcmp(bytes1 + 2, b2.getBytes(), 3) == 0);
    EXPECT_EQ(3, b2.getLen());
    EXPECT_EQ(3, b2.getMaxLen());
    EXPECT_EQ(0x20304, b2.toLong());

    Bytes b3(10);
    b3.fillrandom();
    Bytes b4 = b3.copySubBytes(0, 10);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b4.getBytes(), 10) == 0);
    EXPECT_EQ(10, b4.getLen());
    EXPECT_EQ(10, b4.getMaxLen());
    EXPECT_EQ(b3, b4);

    Bytes b5(10);
    b5.fillrandom();
    Bytes b6 = b5.copySubBytes(0, 5);
    EXPECT_TRUE(std::memcmp(b5.getBytes(), b6.getBytes(), 5) == 0);
    EXPECT_EQ(5, b6.getLen());
    EXPECT_EQ(5, b6.getMaxLen());

    Bytes b7(10);
    b7.fillrandom();
    Bytes b8 = b7.copySubBytes(5, 10);
    EXPECT_TRUE(std::memcmp(b7.getBytes() + 5, b8.getBytes(), 5) == 0);
    EXPECT_EQ(5, b8.getLen());
    EXPECT_EQ(5, b8.getMaxLen());

    Bytes b9(10);
    b9.fillrandom();
    Bytes b10 = b9.copySubBytes(5, 5);
    EXPECT_TRUE(std::memcmp(b9.getBytes() + 5, b10.getBytes(), 0) == 0);
    EXPECT_EQ(0, b10.getLen());
    EXPECT_EQ(0, b10.getMaxLen());
    EXPECT_TRUE(b10.isEmpty());

    Bytes b11(10);
    b11.fillrandom();
    Bytes b12 = b11.copySubBytes(0, 0);
    EXPECT_TRUE(std::memcmp(b11.getBytes(), b12.getBytes(), 0) == 0);
    EXPECT_EQ(0, b12.getLen());
    EXPECT_EQ(0, b12.getMaxLen());
    EXPECT_TRUE(b12.isEmpty());

    // exception checks
    Bytes b13(10);
    b13.fillrandom();
    EXPECT_THROW(b13.copySubBytes(0, 11), std::length_error);
    Bytes b14(10);
    EXPECT_THROW(b14.copySubBytes(0, 1), std::length_error);
    Bytes b25(10);
    b25.addByte(0xad);
    EXPECT_THROW(b25.copySubBytes(1, 2), std::length_error);
    EXPECT_FALSE(b25.copySubBytes(0, 1).isEmpty());
    EXPECT_TRUE(b25.copySubBytes(0, 0).isEmpty());
    EXPECT_TRUE(b25.copySubBytes(1, 1).isEmpty());
    Bytes b15(10);
    EXPECT_TRUE(b15.copySubBytes(0, 0).isEmpty());
    Bytes b16(10);
    b16.fillrandom();
    EXPECT_THROW(b16.copySubBytes(1, 0), std::invalid_argument);
    Bytes b17(10);
    b17.fillrandom();
    EXPECT_THROW(b17.copySubBytes(11, 11), std::length_error);
    Bytes b18(10);
    b18.fillrandom();
    EXPECT_THROW(b18.copySubBytes(-1, 2), std::invalid_argument);
}

TEST(BytesClass, copy) {
    Bytes b1(20);
    Bytes b2(20);
    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    b1.setBytes(bytes1, 10);
    b1.copyToBytes(b2);
    b1.fillrandom();
    EXPECT_NE(b1, b2);
    EXPECT_TRUE(std::memcmp(b1.getBytes(), b2.getBytes(), 10) == 0);
    EXPECT_FALSE(std::memcmp(b1.getBytes(), b2.getBytes(), 20) == 0);

    Bytes b3(20);
    Bytes b4(20);
    b4.addByte(0xad);
    b3.setBytes(bytes1, 10);
    b3.addcopyToBytes(b4);
    b3.fillrandom();
    EXPECT_NE(b3, b4);
    EXPECT_TRUE(std::memcmp(b3.getBytes(), b4.getBytes() + 1, 10) == 0);
    EXPECT_FALSE(std::memcmp(b3.getBytes(), b4.getBytes() + 1, 19) == 0);

    Bytes b5(20);
    unsigned char bytes2[20];
    b5.setBytes(bytes1, 10);
    b5.copyToArray(bytes2, 20);
    b5.fillrandom();
    EXPECT_TRUE(std::memcmp(b5.getBytes(), bytes2, 10) == 0);
    EXPECT_FALSE(std::memcmp(b5.getBytes(), bytes2, 20) == 0);

    Bytes b6(20);
    b6.setBytes(bytes1, 10);
    Bytes b7 = b6.copySubBytes(2, 5);
    b6.fillrandom();
    EXPECT_NE(b6, b7);
    EXPECT_TRUE(std::memcmp(bytes1 + 2, b7.getBytes(), 3) == 0);
}

TEST(BytesClass, addByte) {
    Bytes b1(10);
    b1.addByte(0xad);
    EXPECT_EQ(1, b1.getLen());
    EXPECT_EQ(10, b1.getMaxLen());
    EXPECT_EQ(0xad, b1.getBytes()[0]);

    Bytes b3(10);
    b3.addByte(0xad);
    b3.addByte(0xac);
    EXPECT_EQ(2, b3.getLen());
    EXPECT_EQ(10, b3.getMaxLen());
    EXPECT_EQ(0xad, b3.getBytes()[0]);
    EXPECT_EQ(0xac, b3.getBytes()[1]);

    Bytes b5(256);
    for (int i = 0; i < 256; i++) {
        b5.addByte(i);
        EXPECT_EQ(i + 1, b5.getLen());
        EXPECT_EQ(256, b5.getMaxLen());
    }
    for (int i = 0; i < 256; i++) {
        EXPECT_EQ(i, b5.getBytes()[i]);
    }

    // exception checks
    Bytes b6(0);
    EXPECT_THROW(b6.addByte(0xad), std::length_error);
    Bytes b7(10);
    EXPECT_NO_THROW(b7.addByte(0xad));
    Bytes b8(255);

    for (int i = 0; i < 256; i++) {
        if (i < 255) {
            EXPECT_NO_THROW(b8.addByte(i));
        } else {
            EXPECT_THROW(b8.addByte(i), std::length_error);
        }
    }
}

TEST(BytesClass, isEmpty) {
    Bytes b1(10);
    EXPECT_TRUE(b1.isEmpty());
    b1.addByte(0xad);
    EXPECT_FALSE(b1.isEmpty());
    b1.fillrandom();
    EXPECT_FALSE(b1.isEmpty());

    Bytes b2(0);
    EXPECT_TRUE(b2.isEmpty());
}

TEST(BytesClass, toLong) {
    Bytes b0(8);
    EXPECT_EQ(0, b0.toLong());
    Bytes b1(0);
    EXPECT_EQ(0, b1.toLong());
    Bytes b2(1);
    b2.addByte(0xad);
    EXPECT_EQ(0xad, b2.toLong());
    Bytes b3(2);
    b3.addByte(0xad);
    b3.addByte(0xac);
    EXPECT_EQ(0xadac, b3.toLong());
    Bytes b4(3);
    b4.addByte(0xad);
    b4.addByte(0xac);
    b4.addByte(0xab);
    EXPECT_EQ(0xadacab, b4.toLong());
    Bytes b5(4);
    b5.addByte(0xad);
    b5.addByte(0xac);
    b5.addByte(0xab);
    b5.addByte(0xaa);
    EXPECT_EQ(0xadacabaa, b5.toLong());
    Bytes b6(5);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x11);
    b6.addByte(0x22);
    EXPECT_EQ(0x0000001122, b6.toLong());
    Bytes b7(6);
    b7.addByte(0x45);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x11);
    b7.addByte(0x22);
    EXPECT_EQ(0x450000001122, b7.toLong());
    Bytes b8(7);
    b8.addByte(0x00);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    b8.addByte(0xff);
    EXPECT_EQ(0x00ffffffffffff, b8.toLong());
    Bytes b9(8);
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
    Bytes b10(9);
    b10.fillrandom();
    EXPECT_THROW(b10.toLong(), std::length_error);

    Bytes b11(9);
    b11.addrandom(8);
    EXPECT_NO_THROW(b11.toLong());

    Bytes b12(10);
    b12.addrandom(9);
    EXPECT_THROW(b12.toLong(), std::length_error);
}

TEST(BytesClass, toHex) {
    Bytes b1(0);
    EXPECT_EQ("", b1.toHex());
    Bytes b2(1);
    b2.addByte(0xad);
    EXPECT_EQ("AD", b2.toHex());
    Bytes b3(2);
    b3.addByte(0xad);
    b3.addByte(0xac);
    EXPECT_EQ("ADAC", b3.toHex());
    Bytes b4(3);
    b4.addByte(0xad);
    b4.addByte(0xac);
    b4.addByte(0xab);
    EXPECT_EQ("ADACAB", b4.toHex());
    Bytes b5(4);
    b5.addByte(0xad);
    b5.addByte(0xac);
    b5.addByte(0xab);
    b5.addByte(0xaa);
    EXPECT_EQ("ADACABAA", b5.toHex());
    Bytes b6(5);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x00);
    b6.addByte(0x11);
    b6.addByte(0x22);
    EXPECT_EQ("0000001122", b6.toHex());
    Bytes b7(6);
    b7.addByte(0x45);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x00);
    b7.addByte(0x11);
    b7.addByte(0x22);
    EXPECT_EQ("450000001122", b7.toHex());

    Bytes b8(256);
    for (int i = 0; i < 256; i++) {
        b8.addByte(i);
    }
    std::string hex = b8.toHex();
    for (int i = 0; i < 256; i++) {
        std::stringstream ss;
        ss << std::uppercase << std::hex << i;
        std::string hex2 = ss.str();
        if (hex2.length() == 1) {
            hex2 = "0" + hex2;
        }
        EXPECT_EQ(hex2, hex.substr(i * 2, 2));
    }
}

TEST(BytesClass, fromLong) {
    u_int64_t l1 = 0;
    Bytes b1 = Bytes::fromLong(l1);
    EXPECT_EQ(0, b1.getLen());
    EXPECT_EQ(8, b1.getMaxLen());
    EXPECT_EQ(0, b1.toLong());

    u_int64_t l2 = 0xad;
    Bytes b2 = Bytes::fromLong(l2);
    EXPECT_EQ(1, b2.getLen());
    EXPECT_EQ(8, b2.getMaxLen());
    EXPECT_EQ(0xad, b2.toLong());

    u_int64_t l3 = 0xadac;
    Bytes b3 = Bytes::fromLong(l3);
    EXPECT_EQ(2, b3.getLen());
    EXPECT_EQ(8, b3.getMaxLen());
    EXPECT_EQ(0xadac, b3.toLong());

    u_int64_t l4 = 0xadacab;
    Bytes b4 = Bytes::fromLong(l4);
    EXPECT_EQ(3, b4.getLen());
    EXPECT_EQ(8, b4.getMaxLen());
    EXPECT_EQ(0xadacab, b4.toLong());

    u_int64_t l5 = 0x00ac00aa;
    Bytes b5 = Bytes::fromLong(l5);
    EXPECT_EQ(3, b5.getLen());
    EXPECT_EQ(8, b5.getMaxLen());
    EXPECT_EQ(0xac00aa, b5.toLong());

    u_int64_t l6 = 0x0000001122;
    Bytes b6 = Bytes::fromLong(l6);
    EXPECT_EQ(2, b6.getLen());
    EXPECT_EQ(8, b6.getMaxLen());
    EXPECT_EQ(0x1122, b6.toLong());

    u_int64_t l7 = 0x450000001122;
    Bytes b7 = Bytes::fromLong(l7);
    EXPECT_EQ(6, b7.getLen());
    EXPECT_EQ(8, b7.getMaxLen());
    EXPECT_EQ(0x450000001122, b7.toLong());

    u_int64_t l8 = 0x00ffffffffffff;
    Bytes b8 = Bytes::fromLong(l8);
    EXPECT_EQ(6, b8.getLen());
    EXPECT_EQ(8, b8.getMaxLen());
    EXPECT_EQ(0xffffffffffff, b8.toLong());

    u_int64_t l9 = 0xfffefdfcfbfaf9f8;
    Bytes b9 = Bytes::fromLong(l9);
    EXPECT_EQ(8, b9.getLen());
    EXPECT_EQ(8, b9.getMaxLen());
    EXPECT_EQ(0xfffefdfcfbfaf9f8, b9.toLong());

    u_int64_t l10 = 0xfffefdfcfbfaf9f;
    Bytes b10 = Bytes::fromLong(l10);
    EXPECT_EQ(8, b10.getLen());
    EXPECT_EQ(8, b10.getMaxLen());
    EXPECT_EQ(0xfffefdfcfbfaf9f, b10.toLong());

    // exception checks
    u_int64_t l15 = 0x00fffefdfcfbfaf9f8;
    EXPECT_NO_THROW(Bytes b15 = Bytes::fromLong(l15));

    u_int64_t l16 = 0x0fffefdfcfbfaf9f;
    EXPECT_NO_THROW(Bytes b16 = Bytes::fromLong(l16));

    u_int64_t l17 = 0x0fffefdfcfbfaf9f8;
    EXPECT_NO_THROW(Bytes b17 = Bytes::fromLong(l17));
}

TEST(BytesClass, equal) {
    Bytes b1(10);
    Bytes b2(8);
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

    Bytes b3(0);
    Bytes b4(10000);
    EXPECT_EQ(b3, b4);
    b4.addByte(0xad);
    EXPECT_NE(b3, b4);

    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Bytes b5(15);
    Bytes b6(11);
    EXPECT_EQ(b5, b6);
    b5.setBytes(bytes1, 10);
    EXPECT_NE(b5, b6);
    b6.setBytes(bytes1, 10);
    EXPECT_EQ(b5, b6);
}

TEST(BytesClass, plus) {
    for (int i = 0; i < 1000; i++) {
        Bytes b1(10);
        Bytes b2(10);
        b1.fillrandom();
        b2.fillrandom();
        Bytes b3 = b1 + b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(10, b3.getMaxLen());

        for (int j = 0; j < 10; j++) {
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] + b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    for (int i = 0; i < 1000; i++) {
        Bytes b1(5);
        Bytes b2(10);
        b1.addrandom(5);
        b2.addrandom(5);
        Bytes b3 = b1 + b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(5, b3.getMaxLen());

        for (int j = 0; j < 5; j++) {
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] + b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    Bytes b1(0);
    Bytes b2(0);
    Bytes b3 = b1 + b2;
    EXPECT_EQ(b1.getLen(), b3.getLen());
    EXPECT_EQ(b2.getLen(), b3.getLen());
    EXPECT_EQ(0, b3.getMaxLen());

    unsigned char bytes1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned char bytes2[10] = {126, 255, 255, 34, 17, 0, 0, 0, 0, 253};
    unsigned char bytes3[10] = {126, 0, 1, 37, 21, 5, 6, 7, 8, 6};

    Bytes b4(10);
    Bytes b5(10);
    b4.setBytes(bytes1, 10);
    b5.setBytes(bytes2, 10);
    Bytes b6 = b4 + b5;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), bytes3, 10) == 0);

    // exception checks
    Bytes b7(11);
    Bytes b8(10);
    b7.fillrandom();
    EXPECT_THROW(b7 + b8, std::length_error);
    b8.fillrandom();
    EXPECT_THROW(b7 + b8, std::length_error);
    Bytes b9(2);
    Bytes b10(10);
    b9.fillrandom();
    b10.addrandom(2);
    EXPECT_NO_THROW(b9 + b10);
    Bytes b11(0);
    Bytes b12(10);
    EXPECT_NO_THROW(b11 + b12);
    b12.addByte(0xad);
    EXPECT_THROW(b11 + b12, std::length_error);
}

TEST(BytesClass, minus) {
    for (int i = 0; i < 1000; i++) {
        Bytes b1(10);
        Bytes b2(10);
        b1.fillrandom();
        b2.fillrandom();
        Bytes b3 = b1 - b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(10, b3.getMaxLen());

        for (int j = 0; j < 10; j++) {
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] - b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    for (int i = 0; i < 1000; i++) {
        Bytes b1(5);
        Bytes b2(10);
        b1.addrandom(5);
        b2.addrandom(5);
        Bytes b3 = b1 - b2;
        EXPECT_EQ(b1.getLen(), b3.getLen());
        EXPECT_EQ(b2.getLen(), b3.getLen());
        EXPECT_EQ(5, b3.getMaxLen());

        for (int j = 0; j < 5; j++) {
            EXPECT_EQ((unsigned char)(b1.getBytes()[j] - b2.getBytes()[j]), b3.getBytes()[j]);
        }
    }
    Bytes b1(0);
    Bytes b2(0);
    Bytes b3 = b1 - b2;
    EXPECT_EQ(b1.getLen(), b3.getLen());
    EXPECT_EQ(b2.getLen(), b3.getLen());
    EXPECT_EQ(0, b3.getMaxLen());

    unsigned char bytes1[10] = {126, 0, 1, 37, 21, 5, 6, 7, 8, 6};
    unsigned char bytes2[10] = {126, 255, 255, 34, 17, 0, 0, 0, 0, 253};
    unsigned char bytes3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    Bytes b4(10);
    Bytes b5(10);
    b4.setBytes(bytes1, 10);
    b5.setBytes(bytes2, 10);
    Bytes b6 = b4 - b5;
    EXPECT_TRUE(std::memcmp(b6.getBytes(), bytes3, 10) == 0);

    // exception checks
    Bytes b7(11);
    Bytes b8(10);
    b7.fillrandom();
    EXPECT_THROW(b7 - b8, std::length_error);
    b8.fillrandom();
    EXPECT_THROW(b7 - b8, std::length_error);
    Bytes b9(2);
    Bytes b10(10);
    b9.fillrandom();
    b10.addrandom(2);
    EXPECT_NO_THROW(b9 - b10);
    Bytes b11(0);
    Bytes b12(10);
    EXPECT_NO_THROW(b11 - b12);
    b12.addByte(0xad);
    EXPECT_THROW(b11 - b12, std::length_error);
}
