#include <gtest/gtest.h>

#include "block_decrypt.h"
#include "block_encrypt.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"

TEST(BlockClass, DecryptBlock) {
    // test the DecryptBlock class
    std::shared_ptr<Hash> hash = std::make_shared<sha256>();
    std::shared_ptr<Hash> hash2 = std::make_shared<sha384>();
    std::shared_ptr<Hash> hash3 = std::make_shared<sha512>();
    Bytes salt(32);
    salt.fillrandom();
    DecryptBlock block(hash, salt);
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(Bytes(10), block.getResult());

    block.addData(Bytes(10));
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(Bytes(10), block.getResult());

    Bytes b(10);
    b.fillrandom();
    block.addData(b);

    EXPECT_EQ(block.getFreeSpace(), 22);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b - salt.copySubBytes(0, 10), block.getResult());

    Bytes b2(21);
    Bytes b3(100);
    b2.fillrandom();
    b.copyToBytes(b3);
    b2.addcopyToBytes(b3);
    block.addData(b2);
    EXPECT_EQ(block.getFreeSpace(), 1);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b3 - salt.copySubBytes(0, 31), block.getResult());

    Bytes b4(1);
    b4.fillrandom();
    b4.addcopyToBytes(b3);
    block.addData(b4);
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 - salt, block.getResult());

    EXPECT_NO_THROW(block.addData(Bytes(10)));
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 - salt, block.getResult());

    EXPECT_THROW(block.addData(b4), std::length_error);

    DecryptBlock block2(hash, salt);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(block2.addData(b3), std::length_error);

    // test the constructor with an invalid salt length
    EXPECT_THROW(DecryptBlock block3(hash, Bytes(32)), std::length_error);
    EXPECT_THROW(DecryptBlock block4(hash, Bytes(0)), std::length_error);
    EXPECT_THROW(DecryptBlock block5(hash, b3), std::length_error);
    EXPECT_THROW(DecryptBlock block6(hash, b2), std::length_error);
    EXPECT_THROW(DecryptBlock block7(hash2, salt), std::length_error);
    EXPECT_THROW(DecryptBlock block8(hash3, salt), std::length_error);
    EXPECT_THROW(DecryptBlock block9(hash2, b3), std::length_error);
    EXPECT_THROW(DecryptBlock block10(hash3, b3), std::length_error);
    Bytes b5(65);
    b5.addrandom(15);
    b5.addcopyToBytes(b3);
    EXPECT_NO_THROW(DecryptBlock block11(hash2, b3));
    DecryptBlock block11(hash2, b3);
    EXPECT_EQ(block11.getFreeSpace(), 48);
    EXPECT_THROW(DecryptBlock block12(hash3, b3), std::length_error);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(DecryptBlock block13(hash2, b3), std::length_error);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(DecryptBlock block14(hash2, b3), std::length_error);
    EXPECT_NO_THROW(DecryptBlock block15(hash3, b3));
    DecryptBlock block15(hash3, b3);
    EXPECT_EQ(block15.getFreeSpace(), 64);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(DecryptBlock block16(hash3, b3), std::length_error);
}

TEST(BlockClass, EncryptBlock) {
    // test the EncryptBlock class
    std::shared_ptr<Hash> hash = std::make_shared<sha256>();
    std::shared_ptr<Hash> hash2 = std::make_shared<sha384>();
    std::shared_ptr<Hash> hash3 = std::make_shared<sha512>();
    Bytes salt(32);
    salt.fillrandom();
    EncryptBlock block(hash, salt);
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(Bytes(10), block.getResult());

    block.addData(Bytes(10));
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(Bytes(10), block.getResult());

    Bytes b(10);
    b.fillrandom();
    block.addData(b);

    EXPECT_EQ(block.getFreeSpace(), 22);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b + salt.copySubBytes(0, 10), block.getResult());

    Bytes b2(21);
    Bytes b3(100);
    b2.fillrandom();
    b.copyToBytes(b3);
    b2.addcopyToBytes(b3);
    block.addData(b2);
    EXPECT_EQ(block.getFreeSpace(), 1);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b3 + salt.copySubBytes(0, 31), block.getResult());

    Bytes b4(1);
    b4.fillrandom();
    b4.addcopyToBytes(b3);
    block.addData(b4);
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 + salt, block.getResult());

    EXPECT_NO_THROW(block.addData(Bytes(10)));
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 + salt, block.getResult());

    EXPECT_THROW(block.addData(b4), std::length_error);

    EncryptBlock block2(hash, salt);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(block2.addData(b3), std::length_error);

    // test the constructor with an invalid salt length
    EXPECT_THROW(EncryptBlock block3(hash, Bytes(32)), std::length_error);
    EXPECT_THROW(EncryptBlock block4(hash, Bytes(0)), std::length_error);
    EXPECT_THROW(EncryptBlock block5(hash, b3), std::length_error);
    EXPECT_THROW(EncryptBlock block6(hash, b2), std::length_error);
    EXPECT_THROW(EncryptBlock block7(hash2, salt), std::length_error);
    EXPECT_THROW(EncryptBlock block8(hash3, salt), std::length_error);
    EXPECT_THROW(EncryptBlock block9(hash2, b3), std::length_error);
    EXPECT_THROW(EncryptBlock block10(hash3, b3), std::length_error);
    Bytes b5(65);
    b5.addrandom(15);
    b5.addcopyToBytes(b3);
    EXPECT_NO_THROW(EncryptBlock block11(hash2, b3));
    EncryptBlock block11(hash2, b3);
    EXPECT_EQ(block11.getFreeSpace(), 48);
    EXPECT_THROW(EncryptBlock block12(hash3, b3), std::length_error);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(EncryptBlock block13(hash2, b3), std::length_error);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(EncryptBlock block14(hash2, b3), std::length_error);
    EXPECT_NO_THROW(EncryptBlock block15(hash3, b3));
    EncryptBlock block15(hash3, b3);
    EXPECT_EQ(block15.getFreeSpace(), 64);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(EncryptBlock block16(hash3, b3), std::length_error);
}
