#include <gtest/gtest.h>

#include "block_opt_decrypt.h"
#include "block_opt_encrypt.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"

TEST(BlockClass, DecryptBlock) {
    // test the DecryptBlock class
    std::shared_ptr<Hash> hash = std::make_shared<sha256>();
    std::shared_ptr<Hash> hash2 = std::make_shared<sha384>();
    std::shared_ptr<Hash> hash3 = std::make_shared<sha512>();
    BytesOpt salt(32);
    salt.fillrandom();
    DecryptBlockOpt block(hash, salt);
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(BytesOpt(10), block.getResult());

    block.addData(BytesOpt(10));
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(BytesOpt(10), block.getResult());

    BytesOpt b(10);
    b.fillrandom();
    block.addData(b);

    EXPECT_EQ(block.getFreeSpace(), 22);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b - salt.copySubBytes(0, 10), block.getResult());

    BytesOpt b2(21);
    BytesOpt b3(100);
    b2.fillrandom();
    b.copyToBytes(b3);
    b2.addcopyToBytes(b3);
    block.addData(b2);
    EXPECT_EQ(block.getFreeSpace(), 1);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b3 - salt.copySubBytes(0, 31), block.getResult());

    BytesOpt b4(1);
    b4.fillrandom();
    b4.addcopyToBytes(b3);
    block.addData(b4);
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 - salt, block.getResult());

    EXPECT_NO_THROW(block.addData(BytesOpt(10)));
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 - salt, block.getResult());

    EXPECT_THROW(block.addData(b4), std::length_error);

    DecryptBlockOpt block2(hash, salt);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(block2.addData(b3), std::length_error);

    // test the constructor with an invalid salt length
    EXPECT_THROW(DecryptBlockOpt block3(hash, BytesOpt(32)), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block4(hash, BytesOpt(0)), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block5(hash, b3), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block6(hash, b2), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block7(hash2, salt), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block8(hash3, salt), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block9(hash2, b3), std::length_error);
    EXPECT_THROW(DecryptBlockOpt block10(hash3, b3), std::length_error);
    BytesOpt b5(65);
    b5.addrandom(15);
    b5.addcopyToBytes(b3);
    EXPECT_NO_THROW(DecryptBlockOpt block11(hash2, b3));
    DecryptBlockOpt block11(hash2, b3);
    EXPECT_EQ(block11.getFreeSpace(), 48);
    EXPECT_THROW(DecryptBlockOpt block12(hash3, b3), std::length_error);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(DecryptBlockOpt block13(hash2, b3), std::length_error);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(DecryptBlockOpt block14(hash2, b3), std::length_error);
    EXPECT_NO_THROW(DecryptBlockOpt block15(hash3, b3));
    DecryptBlockOpt block15(hash3, b3);
    EXPECT_EQ(block15.getFreeSpace(), 64);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(DecryptBlockOpt block16(hash3, b3), std::length_error);
}

TEST(BlockClass, EncryptBlock) {
    // test the EncryptBlock class
    std::shared_ptr<Hash> hash = std::make_shared<sha256>();
    std::shared_ptr<Hash> hash2 = std::make_shared<sha384>();
    std::shared_ptr<Hash> hash3 = std::make_shared<sha512>();
    BytesOpt salt(32);
    salt.fillrandom();
    EncryptBlockOpt block(hash, salt);
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(BytesOpt(10), block.getResult());

    block.addData(BytesOpt(10));
    EXPECT_EQ(block.getFreeSpace(), 32);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(BytesOpt(10), block.getResult());

    BytesOpt b(10);
    b.fillrandom();
    block.addData(b);

    EXPECT_EQ(block.getFreeSpace(), 22);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b + salt.copySubBytes(0, 10), block.getResult());

    BytesOpt b2(21);
    BytesOpt b3(100);
    b2.fillrandom();
    b.copyToBytes(b3);
    b2.addcopyToBytes(b3);
    block.addData(b2);
    EXPECT_EQ(block.getFreeSpace(), 1);
    EXPECT_THROW(block.getHash(), std::length_error);
    EXPECT_EQ(b3 + salt.copySubBytes(0, 31), block.getResult());

    BytesOpt b4(1);
    b4.fillrandom();
    b4.addcopyToBytes(b3);
    block.addData(b4);
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 + salt, block.getResult());

    EXPECT_NO_THROW(block.addData(BytesOpt(10)));
    EXPECT_EQ(block.getFreeSpace(), 0);
    EXPECT_EQ(hash->hash(b3 - salt), block.getHash());
    EXPECT_EQ(b3 + salt, block.getResult());

    EXPECT_THROW(block.addData(b4), std::length_error);

    EncryptBlockOpt block2(hash, salt);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(block2.addData(b3), std::length_error);

    // test the constructor with an invalid salt length
    EXPECT_THROW(EncryptBlockOpt block3(hash, BytesOpt(32)), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block4(hash, BytesOpt(0)), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block5(hash, b3), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block6(hash, b2), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block7(hash2, salt), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block8(hash3, salt), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block9(hash2, b3), std::length_error);
    EXPECT_THROW(EncryptBlockOpt block10(hash3, b3), std::length_error);
    BytesOpt b5(65);
    b5.addrandom(15);
    b5.addcopyToBytes(b3);
    EXPECT_NO_THROW(EncryptBlockOpt block11(hash2, b3));
    EncryptBlockOpt block11(hash2, b3);
    EXPECT_EQ(block11.getFreeSpace(), 48);
    EXPECT_THROW(EncryptBlockOpt block12(hash3, b3), std::length_error);
    b4.addcopyToBytes(b3);
    EXPECT_THROW(EncryptBlockOpt block13(hash2, b3), std::length_error);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(EncryptBlockOpt block14(hash2, b3), std::length_error);
    EXPECT_NO_THROW(EncryptBlockOpt block15(hash3, b3));
    EncryptBlockOpt block15(hash3, b3);
    EXPECT_EQ(block15.getFreeSpace(), 64);
    b5.addcopyToBytes(b3);
    EXPECT_THROW(EncryptBlockOpt block16(hash3, b3), std::length_error);
}
