#include "gtest/gtest.h"

#define private public  // setting everything public for testing
#include "block.h"

TEST(BlockClass, constructors) {
    // testing the constructors
    Block block1 = Block();
    Block block2 = Block(286, Bytes(286), Bytes(286), Bytes(286));
    Block block3 = Block(Bytes(28));

    EXPECT_EQ(0, block1.getLen());
    EXPECT_EQ(0, block1.getEncoded().getLen());
    EXPECT_FALSE(block1.isReadyForEncode());
    EXPECT_FALSE(block1.isReadyForDecode());
    EXPECT_EQ(286, block2.getLen());
    EXPECT_EQ(0, block2.getEncoded().getLen());
    EXPECT_TRUE(block2.isReadyForEncode());
    EXPECT_FALSE(block2.isReadyForDecode());
    EXPECT_EQ(28, block3.getLen());
    EXPECT_FALSE(block3.isReadyForEncode());
    EXPECT_FALSE(block3.isReadyForDecode());
    EXPECT_EQ(28, block3.getEncoded().getLen());
}

TEST(BlockClass, returnType) {
    // testing the return types
    EXPECT_EQ(typeid(Bytes), typeid(Block().getSalt()));
    EXPECT_EQ(typeid(Bytes), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).getSalt()));
    EXPECT_EQ(typeid(Bytes), typeid(Block().getPasswordHash()));
    EXPECT_EQ(typeid(Bytes), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).getPasswordHash()));
    EXPECT_EQ(typeid(Bytes), typeid(Block().getData()));
    EXPECT_EQ(typeid(Bytes), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).getData()));
    EXPECT_EQ(typeid(void), typeid(Block().setLen(10)));
    EXPECT_EQ(typeid(void), typeid(Block().setData(Bytes(10))));
    EXPECT_EQ(typeid(void), typeid(Block().setSalt(Bytes(10))));
    EXPECT_EQ(typeid(void), typeid(Block().setPasswordHash(Bytes(10))));
    EXPECT_EQ(typeid(int), typeid(Block().getLen()));
    EXPECT_EQ(typeid(int), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).getLen()));
    EXPECT_EQ(typeid(Bytes), typeid(Block().getEncoded()));
    EXPECT_EQ(typeid(Bytes), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).getEncoded()));
    EXPECT_EQ(typeid(bool), typeid(Block().isReadyForEncode()));
    EXPECT_EQ(typeid(bool), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).isReadyForEncode()));
    EXPECT_EQ(typeid(void), typeid(Block(10, Bytes(10), Bytes(10), Bytes(10)).calcEncoded()));
    EXPECT_EQ(typeid(bool), typeid(Block().isEncoded()));
    EXPECT_EQ(typeid(void), typeid(Block().setEncoded(Bytes(10))));
    EXPECT_EQ(typeid(bool), typeid(Block().isReadyForDecode()));
    EXPECT_EQ(typeid(bool), typeid(Block(Bytes(10)).isReadyForDecode()));
    Block b1(Bytes(10));
    b1.setPasswordHash(Bytes(10));
    b1.setSalt(Bytes(10));
    EXPECT_EQ(typeid(void), typeid(b1.calcData()));
    EXPECT_EQ(typeid(bool), typeid(b1.isDecoded()));
    EXPECT_EQ(typeid(void), typeid(b1.clear()));
}

TEST(BlockClass, setter) {
    // testing the setters of the Block class
    Block block1 = Block();
    Block block2 = Block(286, Bytes(286), Bytes(286), Bytes(286));
    Block block3 = Block(Bytes(28));
    Bytes dummy = Bytes();

    EXPECT_THROW(block1.setData(dummy), std::length_error);
    EXPECT_THROW(block1.setEncoded(dummy), std::length_error);
    EXPECT_THROW(block1.setPasswordHash(dummy), std::length_error);
    EXPECT_THROW(block1.setSalt(dummy), std::length_error);
    EXPECT_THROW(block1.setLen(0), std::length_error);
    EXPECT_NO_THROW(block1.setLen(128));
    EXPECT_THROW(block1.setData(Bytes(127)), std::length_error);
    EXPECT_THROW(block1.setData(Bytes(129)), std::length_error);
    EXPECT_NO_THROW(block1.setData(Bytes(128)));
    EXPECT_THROW(block1.setPasswordHash(Bytes(127)), std::length_error);
    EXPECT_THROW(block1.setPasswordHash(Bytes(129)), std::length_error);
    EXPECT_NO_THROW(block1.setPasswordHash(Bytes(128)));
    EXPECT_THROW(block1.setSalt(Bytes(127)), std::length_error);
    EXPECT_THROW(block1.setSalt(Bytes(129)), std::length_error);
    EXPECT_NO_THROW(block1.setSalt(Bytes(128)));
    EXPECT_THROW(block1.setEncoded(Bytes(127)), std::length_error);
    EXPECT_THROW(block1.setEncoded(Bytes(129)), std::length_error);
    EXPECT_NO_THROW(block1.setEncoded(Bytes(128)));
    EXPECT_THROW(block1.setLen(128), std::length_error);

    EXPECT_THROW(block2.setData(dummy), std::length_error);
    EXPECT_THROW(block2.setEncoded(dummy), std::length_error);
    EXPECT_THROW(block2.setPasswordHash(dummy), std::length_error);
    EXPECT_THROW(block2.setSalt(dummy), std::length_error);
    EXPECT_THROW(block2.setLen(0), std::length_error);
    EXPECT_THROW(block2.setLen(128), std::length_error);
    EXPECT_THROW(block2.setData(Bytes(285)), std::length_error);
    EXPECT_THROW(block2.setData(Bytes(287)), std::length_error);
    EXPECT_NO_THROW(block2.setData(Bytes(286)));
    EXPECT_THROW(block2.setPasswordHash(Bytes(285)), std::length_error);
    EXPECT_THROW(block2.setPasswordHash(Bytes(287)), std::length_error);
    EXPECT_NO_THROW(block2.setPasswordHash(Bytes(286)));
    EXPECT_THROW(block2.setSalt(Bytes(285)), std::length_error);
    EXPECT_THROW(block2.setSalt(Bytes(287)), std::length_error);
    EXPECT_NO_THROW(block2.setSalt(Bytes(286)));
    EXPECT_THROW(block2.setEncoded(Bytes(285)), std::length_error);
    EXPECT_THROW(block2.setEncoded(Bytes(287)), std::length_error);
    EXPECT_NO_THROW(block2.setEncoded(Bytes(286)));
    EXPECT_THROW(block2.setLen(286), std::length_error);

    EXPECT_THROW(block3.setData(dummy), std::length_error);
    EXPECT_THROW(block3.setEncoded(dummy), std::length_error);
    EXPECT_THROW(block3.setPasswordHash(dummy), std::length_error);
    EXPECT_THROW(block3.setSalt(dummy), std::length_error);
    EXPECT_THROW(block3.setLen(0), std::length_error);
    EXPECT_THROW(block3.setLen(128), std::length_error);
    EXPECT_THROW(block3.setData(Bytes(27)), std::length_error);
    EXPECT_THROW(block3.setData(Bytes(29)), std::length_error);
    EXPECT_NO_THROW(block3.setData(Bytes(28)));
    EXPECT_THROW(block3.setPasswordHash(Bytes(27)), std::length_error);
    EXPECT_THROW(block3.setPasswordHash(Bytes(29)), std::length_error);
    EXPECT_NO_THROW(block3.setPasswordHash(Bytes(28)));
    EXPECT_THROW(block3.setEncoded(Bytes(27)), std::length_error);
    EXPECT_THROW(block3.setEncoded(Bytes(29)), std::length_error);
    EXPECT_NO_THROW(block3.setEncoded(Bytes(28)));
    EXPECT_THROW(block3.setSalt(Bytes(27)), std::length_error);
    EXPECT_THROW(block3.setSalt(Bytes(29)), std::length_error);
    EXPECT_NO_THROW(block3.setSalt(Bytes(28)));
    EXPECT_THROW(block3.setLen(28), std::length_error);
}

TEST(BlockClass, getter) {
    // testing the getter of the block class
    Block block1 = Block();
    Bytes encoded1(286);
    Bytes encoded2(28);
    Bytes data(286);
    Bytes data2(28);
    Block block2 = Block(286, data, Bytes(286), Bytes(286));
    Block block3 = Block(encoded2);

    EXPECT_EQ(Bytes(), block1.getEncoded());
    EXPECT_EQ(Bytes(), block2.getEncoded());
    EXPECT_EQ(encoded2, block3.getEncoded());
    block2.setEncoded(encoded1);
    EXPECT_EQ(encoded1, block2.getEncoded());
    EXPECT_EQ(data, block2.getData());
    EXPECT_EQ(Bytes(), block1.getData());
    EXPECT_EQ(Bytes(), block3.getData());
    block3.setData(data2);
    EXPECT_EQ(data2, block3.getData());
}

TEST(BlockClass, clear) {
    // testing the clear method
    Block block1 = Block();
    Block block2 = Block(286, Bytes(286), Bytes(286), Bytes(286));
    Block block3 = Block(Bytes(28));
    block1.clear();
    block2.clear();
    block3.clear();

    EXPECT_EQ(0, block1.getLen());
    EXPECT_EQ(0, block2.getLen());
    EXPECT_EQ(0, block3.getLen());
    EXPECT_EQ(0, block1.getEncoded().getLen());
    EXPECT_EQ(0, block2.getEncoded().getLen());
    EXPECT_EQ(0, block3.getEncoded().getLen());
    EXPECT_FALSE(block1.isReadyForDecode());
    EXPECT_FALSE(block1.isReadyForEncode());
    EXPECT_FALSE(block2.isReadyForDecode());
    EXPECT_FALSE(block2.isReadyForEncode());
    EXPECT_FALSE(block3.isReadyForDecode());
    EXPECT_FALSE(block3.isReadyForEncode());
    EXPECT_THROW(block1.setLen(0), std::length_error);
    EXPECT_THROW(block2.setLen(0), std::length_error);
    EXPECT_THROW(block3.setLen(0), std::length_error);
    EXPECT_NO_THROW(block1.setLen(1));
    EXPECT_NO_THROW(block2.setLen(1));
    EXPECT_NO_THROW(block3.setLen(1));
}

TEST(BlockClass, encode) {
    // testing encoding in a block
    Bytes data(1);
    Bytes password(1);
    Bytes salt(1);
    Bytes data1(286);
    Bytes password1(286);
    Bytes salt1(286);
    Bytes data2(28);
    Bytes password2(28);
    Bytes salt2(28);

    Block block1 = Block();
    Block block2 = Block(286, data1, salt1, password1);
    Block block3 = Block(Bytes(28));

    EXPECT_FALSE(block1.isEncoded());
    EXPECT_FALSE(block2.isEncoded());
    EXPECT_TRUE(block3.isEncoded());

    EXPECT_NO_THROW(block1.setLen(1));
    EXPECT_NO_THROW(block1.setData(data));
    EXPECT_NO_THROW(block1.setSalt(salt));
    EXPECT_NO_THROW(block1.setPasswordHash(password));
    EXPECT_TRUE(block1.isReadyForEncode());
    EXPECT_NO_THROW(block1.calcEncoded());
    EXPECT_EQ(1, block1.getEncoded().getBytes().size());
    EXPECT_EQ((data.getBytes()[0] + password.getBytes()[0] + salt.getBytes()[0]) % 256, block1.getEncoded().getBytes()[0]);
    EXPECT_TRUE(block2.isReadyForEncode());
    EXPECT_NO_THROW(block2.calcEncoded());
    EXPECT_EQ(286, block2.getEncoded().getBytes().size());
    EXPECT_EQ(data1 + password1 + salt1, block2.getEncoded());
    EXPECT_NO_THROW(block3.setData(data2));
    EXPECT_NO_THROW(block3.setSalt(salt2));
    EXPECT_NO_THROW(block3.setPasswordHash(password2));
    EXPECT_TRUE(block3.isReadyForEncode());
    EXPECT_NO_THROW(block3.calcEncoded());
    EXPECT_EQ(28, block3.getEncoded().getBytes().size());
    EXPECT_EQ(data2 + salt2 + password2, block3.getEncoded());

    EXPECT_TRUE(block1.isEncoded());
    EXPECT_TRUE(block2.isEncoded());
    EXPECT_TRUE(block3.isEncoded());
}

TEST(BlockClass, decode) {
    // testing decoding in a block
    Bytes encode(1);
    Bytes password(1);
    Bytes salt(1);
    Bytes data1(286);
    Bytes password1(286);
    Bytes salt1(286);
    Bytes encode1(286);
    Bytes password2(28);
    Bytes salt2(28);
    Bytes encode2(28);

    Block block1 = Block();
    Block block2 = Block(286, data1, salt1, password1);
    Block block3 = Block(encode2);

    EXPECT_FALSE(block1.isDecoded());
    EXPECT_TRUE(block2.isDecoded());
    EXPECT_FALSE(block3.isDecoded());

    EXPECT_NO_THROW(block1.setLen(1));
    EXPECT_NO_THROW(block1.setEncoded(encode));
    EXPECT_NO_THROW(block1.setSalt(salt));
    EXPECT_NO_THROW(block1.setPasswordHash(password));
    EXPECT_TRUE(block1.isReadyForDecode());
    EXPECT_NO_THROW(block1.calcData());
    EXPECT_EQ(1, block1.getData().getBytes().size());
    EXPECT_EQ((unsigned char)(encode.getBytes()[0] - password.getBytes()[0] - salt.getBytes()[0]), block1.getData().getBytes()[0]);
    EXPECT_FALSE(block2.isReadyForDecode());
    EXPECT_NO_THROW(block2.setEncoded(encode1));
    EXPECT_TRUE(block2.isReadyForDecode());
    EXPECT_NO_THROW(block2.calcData());
    EXPECT_EQ(286, block2.getData().getBytes().size());
    EXPECT_EQ(encode1 - password1 - salt1, block2.getData());
    EXPECT_NO_THROW(block3.setSalt(salt2));
    EXPECT_NO_THROW(block3.setPasswordHash(password2));
    EXPECT_TRUE(block3.isReadyForDecode());
    EXPECT_NO_THROW(block3.calcData());
    EXPECT_EQ(28, block3.getData().getBytes().size());
    EXPECT_EQ(encode2 - salt2 - password2, block3.getData());

    EXPECT_TRUE(block1.isDecoded());
    EXPECT_TRUE(block2.isDecoded());
    EXPECT_TRUE(block3.isDecoded());
}
