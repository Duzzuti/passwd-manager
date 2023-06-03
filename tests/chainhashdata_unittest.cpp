#include "chainhash_data.h"

#include "gtest/gtest.h"

TEST(ChainHashDataClass, returnTypes){
    // testing the return types of the ChainHashData class
    ChainHashData chd{Format(CHAINHASH_QUADRATIC)};
    EXPECT_EQ(typeid(bool), typeid(chd.isComplete()));
    EXPECT_EQ(typeid(bool), typeid(chd.isCompletedFormat(Format(CHAINHASH_NORMAL))));
    EXPECT_EQ(typeid(unsigned char), typeid(chd.getPartsNumber()));
    EXPECT_EQ(typeid(Bytes), typeid(chd.getDataBlock()));
    EXPECT_EQ(typeid(Bytes), typeid(chd.getPart("A")));
    EXPECT_EQ(typeid(size_t), typeid(chd.getLen()));
    EXPECT_EQ(typeid(void), typeid(chd.addBytes(Bytes(8))));
    EXPECT_EQ(typeid(bool), typeid(chd == chd));
}

TEST(ChainHashDataClass, isComplete){
    // testing the isComplete methods of the ChainHashData class
    ChainHashData chd{Format(CHAINHASH_QUADRATIC)};
    EXPECT_FALSE(chd.isComplete());
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_NORMAL)));
    chd.addBytes(Bytes(8));
    EXPECT_FALSE(chd.isComplete());
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_NORMAL)));
    chd.addBytes(Bytes(8));
    EXPECT_FALSE(chd.isComplete());
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_NORMAL)));
    chd.addBytes(Bytes(8));
    EXPECT_FALSE(chd.isComplete());
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_NORMAL)));
    chd.addBytes(Bytes(8));
    EXPECT_TRUE(chd.isComplete());
    EXPECT_TRUE(chd.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
    EXPECT_FALSE(chd.isCompletedFormat(Format(CHAINHASH_NORMAL)));

    ChainHashData chd2{Format(CHAINHASH_NORMAL)};
    EXPECT_TRUE(chd2.isComplete());
    EXPECT_FALSE(chd2.isCompletedFormat(Format(CHAINHASH_CONSTANT_SALT)));
    EXPECT_TRUE(chd2.isCompletedFormat(Format(CHAINHASH_NORMAL)));

    ChainHashData chd3{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_FALSE(chd3.isComplete());
    EXPECT_FALSE(chd3.isCompletedFormat(Format(CHAINHASH_CONSTANT_COUNT_SALT)));
    EXPECT_FALSE(chd3.isCompletedFormat(Format(CHAINHASH_CONSTANT_SALT)));
    chd3.addBytes(Bytes(0));
    EXPECT_TRUE(chd3.isComplete());
    EXPECT_FALSE(chd3.isCompletedFormat(Format(CHAINHASH_CONSTANT_COUNT_SALT)));
    EXPECT_TRUE(chd3.isCompletedFormat(Format(CHAINHASH_CONSTANT_SALT)));

    ChainHashData chd4{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_FALSE(chd4.isComplete());
    EXPECT_FALSE(chd4.isCompletedFormat(Format(CHAINHASH_CONSTANT_COUNT_SALT)));
    EXPECT_FALSE(chd4.isCompletedFormat(Format(CHAINHASH_COUNT_SALT)));
    chd4.addBytes(Bytes(8));
    EXPECT_FALSE(chd4.isComplete());
    EXPECT_FALSE(chd4.isCompletedFormat(Format(CHAINHASH_CONSTANT_COUNT_SALT)));
    EXPECT_FALSE(chd4.isCompletedFormat(Format(CHAINHASH_COUNT_SALT)));
    chd4.addBytes(Bytes(247));
    EXPECT_TRUE(chd4.isComplete());
    EXPECT_TRUE(chd4.isCompletedFormat(Format(CHAINHASH_CONSTANT_COUNT_SALT)));
    EXPECT_FALSE(chd4.isCompletedFormat(Format(CHAINHASH_COUNT_SALT)));

    ChainHashData chd5{Format(CHAINHASH_COUNT_SALT)};
    EXPECT_FALSE(chd5.isComplete());
    EXPECT_FALSE(chd5.isCompletedFormat(Format(CHAINHASH_COUNT_SALT)));
    EXPECT_FALSE(chd5.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
    chd5.addBytes(Bytes(8));
    EXPECT_TRUE(chd5.isComplete());
    EXPECT_TRUE(chd5.isCompletedFormat(Format(CHAINHASH_COUNT_SALT)));
    EXPECT_FALSE(chd5.isCompletedFormat(Format(CHAINHASH_QUADRATIC)));
}

TEST(ChainHashDataClass, getPartsNumber){
    // testing the getPartsNumber method of the ChainHashData class
    ChainHashData chd{Format(CHAINHASH_NORMAL)};
    EXPECT_EQ(0, chd.getPartsNumber());

    ChainHashData chd2{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_EQ(0, chd2.getPartsNumber());
    chd2.addBytes(Bytes(255));
    EXPECT_EQ(1, chd2.getPartsNumber());

    ChainHashData chd3{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_EQ(0, chd3.getPartsNumber());
    chd3.addBytes(Bytes(8));
    EXPECT_EQ(1, chd3.getPartsNumber());
    chd3.addBytes(Bytes());
    EXPECT_EQ(2, chd3.getPartsNumber());

    ChainHashData chd4{Format(CHAINHASH_COUNT_SALT)};
    EXPECT_EQ(0, chd4.getPartsNumber());
    chd4.addBytes(Bytes(8));
    EXPECT_EQ(1, chd4.getPartsNumber());

    ChainHashData chd5{Format(CHAINHASH_QUADRATIC)};
    EXPECT_EQ(0, chd5.getPartsNumber());
    chd5.addBytes(Bytes(8));
    EXPECT_EQ(1, chd5.getPartsNumber());
    chd5.addBytes(Bytes(8));
    EXPECT_EQ(2, chd5.getPartsNumber());
    chd5.addBytes(Bytes(8));
    EXPECT_EQ(3, chd5.getPartsNumber());
    chd5.addBytes(Bytes(8));
    EXPECT_EQ(4, chd5.getPartsNumber());
}

TEST(ChainHashDataClass, getDataBlock){
    // testing the getDataBlock method of the ChainHashData class
    ChainHashData chd{Format(CHAINHASH_NORMAL)};
    EXPECT_EQ(Bytes(), chd.getDataBlock());

    ChainHashData chd2{Format(CHAINHASH_CONSTANT_SALT)};
    ChainHashData chd3{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_THROW(chd2.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd3.getDataBlock(), std::logic_error);
    Bytes b{255};
    chd2.addBytes(b);
    EXPECT_EQ(b, chd2.getDataBlock());
    chd3.addBytes(Bytes());
    EXPECT_EQ(Bytes(), chd3.getDataBlock());

    ChainHashData chd4{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd5{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_THROW(chd4.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd5.getDataBlock(), std::logic_error);
    Bytes startnum{8};
    Bytes startnum2{8};
    chd4.addBytes(startnum);
    chd5.addBytes(startnum2);
    EXPECT_THROW(chd4.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd5.getDataBlock(), std::logic_error);
    Bytes b2{247};
    chd4.addBytes(b2);
    chd5.addBytes(Bytes());
    startnum.addBytes(b2);
    EXPECT_EQ(startnum, chd4.getDataBlock());
    EXPECT_EQ(startnum2, chd5.getDataBlock());

    ChainHashData chd6{Format(CHAINHASH_COUNT_SALT)};
    EXPECT_THROW(chd6.getDataBlock(), std::logic_error);
    Bytes startnum3{8};
    chd6.addBytes(startnum3);
    EXPECT_EQ(startnum3, chd6.getDataBlock());

    ChainHashData chd7{Format(CHAINHASH_QUADRATIC)};
    ChainHashData chd8{Format(CHAINHASH_QUADRATIC)};
    EXPECT_THROW(chd7.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd8.getDataBlock(), std::logic_error);
    Bytes startnum4{8};
    Bytes startnum5{8};
    chd7.addBytes(startnum4);
    chd8.addBytes(startnum5);
    EXPECT_THROW(chd7.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd8.getDataBlock(), std::logic_error);
    Bytes b3{8};
    Bytes b4{8};
    Bytes b5{8};
    Bytes b6{8};
    Bytes b7{8};
    Bytes b8{8};
    chd7.addBytes(b3);
    chd8.addBytes(b4);
    EXPECT_THROW(chd7.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd8.getDataBlock(), std::logic_error);
    startnum4.addBytes(b3);
    startnum5.addBytes(b4);
    chd7.addBytes(b5);
    chd8.addBytes(b6);
    EXPECT_THROW(chd7.getDataBlock(), std::logic_error);
    EXPECT_THROW(chd8.getDataBlock(), std::logic_error);
    startnum4.addBytes(b5);
    startnum5.addBytes(b6);
    chd7.addBytes(b7);
    chd8.addBytes(b8);
    startnum4.addBytes(b7);
    startnum5.addBytes(b8);
    EXPECT_EQ(startnum4, chd7.getDataBlock());
    EXPECT_EQ(startnum5, chd8.getDataBlock());
}

TEST(ChainHashDataClass, getPart){
    // testing the getPart method of the ChainHashData class
    ChainHashData chd{Format(CHAINHASH_NORMAL)};
    EXPECT_THROW(chd.getPart("S"), std::logic_error);

    ChainHashData chd2{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_THROW(chd2.getPart("S"), std::logic_error);
    Bytes b{255};
    chd2.addBytes(b);
    EXPECT_EQ(b, chd2.getPart("S"));
    EXPECT_THROW(chd2.getPart("A"), std::logic_error);

    ChainHashData chd3{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd4{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_THROW(chd3.getPart("S"), std::logic_error);
    EXPECT_THROW(chd3.getPart("SN"), std::logic_error);
    Bytes startnum{8};
    chd3.addBytes(startnum);
    chd4.addBytes(startnum);
    EXPECT_EQ(startnum, chd3.getPart("SN"));
    EXPECT_THROW(chd3.getPart("S"), std::logic_error);
    Bytes b2{247};
    chd3.addBytes(b2);
    chd4.addBytes(Bytes());
    EXPECT_EQ(b2, chd3.getPart("S"));
    EXPECT_EQ(Bytes(), chd4.getPart("S"));
    EXPECT_EQ(startnum, chd3.getPart("SN"));
    EXPECT_EQ(startnum, chd4.getPart("SN"));
    EXPECT_THROW(chd3.getPart("A"), std::logic_error);
    EXPECT_THROW(chd4.getPart("A"), std::logic_error);

    ChainHashData chd5{Format(CHAINHASH_COUNT_SALT)};
    EXPECT_THROW(chd5.getPart("SN"), std::logic_error);
    Bytes startnum2{8};
    chd5.addBytes(startnum2);
    EXPECT_EQ(startnum2, chd5.getPart("SN"));
    EXPECT_THROW(chd5.getPart("S"), std::logic_error);

    ChainHashData chd6{Format(CHAINHASH_QUADRATIC)};
    ChainHashData chd7{Format(CHAINHASH_QUADRATIC)};
    EXPECT_THROW(chd6.getPart("SN"), std::logic_error);
    EXPECT_THROW(chd6.getPart("A"), std::logic_error);
    EXPECT_THROW(chd6.getPart("B"), std::logic_error);
    EXPECT_THROW(chd6.getPart("C"), std::logic_error);
    Bytes startnum3{8};
    Bytes startnum4{8};
    chd6.addBytes(startnum3);
    chd7.addBytes(startnum4);
    EXPECT_EQ(startnum3, chd6.getPart("SN"));
    EXPECT_EQ(startnum4, chd7.getPart("SN"));
    EXPECT_THROW(chd6.getPart("A"), std::logic_error);
    EXPECT_THROW(chd7.getPart("A"), std::logic_error);
    EXPECT_THROW(chd6.getPart("B"), std::logic_error);
    EXPECT_THROW(chd7.getPart("B"), std::logic_error);
    EXPECT_THROW(chd6.getPart("C"), std::logic_error);
    EXPECT_THROW(chd7.getPart("C"), std::logic_error);
    Bytes b3{8};
    Bytes b4{8};
    chd6.addBytes(b3);
    chd7.addBytes(b4);
    EXPECT_EQ(startnum3, chd6.getPart("SN"));
    EXPECT_EQ(startnum4, chd7.getPart("SN"));
    EXPECT_EQ(b3, chd6.getPart("A"));
    EXPECT_EQ(b4, chd7.getPart("A"));
    EXPECT_THROW(chd6.getPart("B"), std::logic_error);
    EXPECT_THROW(chd7.getPart("B"), std::logic_error);
    EXPECT_THROW(chd6.getPart("C"), std::logic_error);
    EXPECT_THROW(chd7.getPart("C"), std::logic_error);
    Bytes b5{8};
    Bytes b6{8};
    chd6.addBytes(b5);
    chd7.addBytes(b6);
    EXPECT_EQ(startnum3, chd6.getPart("SN"));
    EXPECT_EQ(startnum4, chd7.getPart("SN"));
    EXPECT_EQ(b3, chd6.getPart("A"));
    EXPECT_EQ(b4, chd7.getPart("A"));
    EXPECT_EQ(b5, chd6.getPart("B"));
    EXPECT_EQ(b6, chd7.getPart("B"));
    EXPECT_THROW(chd6.getPart("C"), std::logic_error);
    EXPECT_THROW(chd7.getPart("C"), std::logic_error);
    Bytes b7{8};
    Bytes b8{8};
    chd6.addBytes(b7);
    chd7.addBytes(b8);
    EXPECT_EQ(startnum3, chd6.getPart("SN"));
    EXPECT_EQ(startnum4, chd7.getPart("SN"));
    EXPECT_EQ(b3, chd6.getPart("A"));
    EXPECT_EQ(b4, chd7.getPart("A"));
    EXPECT_EQ(b5, chd6.getPart("B"));
    EXPECT_EQ(b6, chd7.getPart("B"));
    EXPECT_EQ(b7, chd6.getPart("C"));
    EXPECT_EQ(b8, chd7.getPart("C"));
    EXPECT_THROW(chd6.getPart("D"), std::logic_error);
    EXPECT_THROW(chd7.getPart("D"), std::logic_error);
}

TEST(ChainHashDataClass, getLen){
    // testing the getLen method of the ChainHashData class

    ChainHashData chd{Format(CHAINHASH_NORMAL)};
    EXPECT_EQ(0, chd.getLen());

    ChainHashData chd2{Format(CHAINHASH_CONSTANT_SALT)};
    ChainHashData chd3{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_EQ(0, chd2.getLen());
    EXPECT_EQ(0, chd3.getLen());
    chd2.addBytes(Bytes(255));
    chd3.addBytes(Bytes());
    EXPECT_EQ(255, chd2.getLen());
    EXPECT_EQ(0, chd3.getLen());

    ChainHashData chd4{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd5{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_EQ(0, chd4.getLen());
    EXPECT_EQ(0, chd5.getLen());
    chd4.addBytes(Bytes(8));
    chd5.addBytes(Bytes(8));
    EXPECT_EQ(8, chd4.getLen());
    EXPECT_EQ(8, chd5.getLen());
    chd4.addBytes(Bytes(247));
    chd5.addBytes(Bytes(0));
    EXPECT_EQ(255, chd4.getLen());
    EXPECT_EQ(8, chd5.getLen());
    
    ChainHashData chd6{Format(CHAINHASH_COUNT_SALT)};
    EXPECT_EQ(0, chd6.getLen());
    chd6.addBytes(Bytes(8));
    EXPECT_EQ(8, chd6.getLen());

    ChainHashData chd7{Format(CHAINHASH_QUADRATIC)};
    EXPECT_EQ(0, chd7.getLen());
    chd7.addBytes(Bytes(8));
    EXPECT_EQ(8, chd7.getLen());
    chd7.addBytes(Bytes(8));
    EXPECT_EQ(16, chd7.getLen());
    chd7.addBytes(Bytes(8));
    EXPECT_EQ(24, chd7.getLen());
    chd7.addBytes(Bytes(8));
    EXPECT_EQ(32, chd7.getLen());
}

TEST(ChainHashDataClass, addBytes){
    // testing the addBytes method of the ChainHashData class

    ChainHashData chd{Format(CHAINHASH_NORMAL)};
    EXPECT_THROW(chd.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd.addBytes(Bytes(255)), std::logic_error);
    EXPECT_THROW(chd.addBytes(Bytes(8)), std::logic_error);

    ChainHashData chd2{Format(CHAINHASH_CONSTANT_SALT)};
    ChainHashData chd3{Format(CHAINHASH_CONSTANT_SALT)};
    ChainHashData chd4{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_NO_THROW(chd2.addBytes(Bytes(0)));
    EXPECT_NO_THROW(chd3.addBytes(Bytes(255)));
    EXPECT_THROW(chd4.addBytes(Bytes(256)), std::logic_error);
    // cannot add more parts
    EXPECT_THROW(chd2.addBytes(Bytes(8)), std::logic_error);
    EXPECT_THROW(chd2.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd3.addBytes(Bytes(0)), std::logic_error);

    ChainHashData chd5{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd6{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd7{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_NO_THROW(chd5.addBytes(Bytes(8)));
    EXPECT_THROW(chd6.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd6.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd7.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd6.addBytes(Bytes(8)));
    EXPECT_NO_THROW(chd7.addBytes(Bytes(8)));
    EXPECT_THROW(chd5.addBytes(Bytes(248)), std::logic_error);
    EXPECT_THROW(chd6.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd5.addBytes(Bytes(0)));
    EXPECT_NO_THROW(chd6.addBytes(Bytes(247)));
    EXPECT_NO_THROW(chd7.addBytes(Bytes(100)));
    // cannot add more parts
    EXPECT_THROW(chd5.addBytes(Bytes(8)), std::logic_error);
    EXPECT_THROW(chd6.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd7.addBytes(Bytes(255)), std::logic_error);

    ChainHashData chd8{Format(CHAINHASH_COUNT_SALT)};
    EXPECT_THROW(chd8.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd8.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd8.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd8.addBytes(Bytes(8)));
    // cannot add more parts
    EXPECT_THROW(chd8.addBytes(Bytes(8)), std::logic_error);
    EXPECT_THROW(chd8.addBytes(Bytes(0)), std::logic_error);

    ChainHashData chd9{Format(CHAINHASH_QUADRATIC)};
    EXPECT_THROW(chd9.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd9.addBytes(Bytes(8)));
    EXPECT_THROW(chd9.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd9.addBytes(Bytes(8)));
    EXPECT_THROW(chd9.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd9.addBytes(Bytes(8)));
    EXPECT_THROW(chd9.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(255)), std::logic_error);
    EXPECT_NO_THROW(chd9.addBytes(Bytes(8)));
    // cannot add more parts
    EXPECT_THROW(chd9.addBytes(Bytes(0)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(4)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(255)), std::logic_error);
    EXPECT_THROW(chd9.addBytes(Bytes(8)), std::logic_error);


}

TEST(ChainHashDataClass, operator){
    // testing the operator overloads of the ChainHashData class

    ChainHashData chd{Format(CHAINHASH_NORMAL)};
    ChainHashData chd2{Format(CHAINHASH_NORMAL)};
    ChainHashData chd3{Format(CHAINHASH_NORMAL)};
    EXPECT_TRUE(chd == chd2);
    EXPECT_TRUE(chd2 == chd);
    EXPECT_TRUE(chd == chd);
    EXPECT_TRUE(chd == chd3);
    EXPECT_TRUE(chd3 == chd2);

    ChainHashData chd4{Format(CHAINHASH_CONSTANT_SALT)};
    ChainHashData chd5{Format(CHAINHASH_CONSTANT_SALT)};
    ChainHashData chd6{Format(CHAINHASH_CONSTANT_SALT)};
    EXPECT_FALSE(chd4 == chd5);
    EXPECT_FALSE(chd5 == chd6);
    EXPECT_FALSE(chd6 == chd6);
    Bytes b1{0};
    Bytes b2{255};
    chd4.addBytes(b1);
    chd5.addBytes(b2);
    EXPECT_FALSE(chd4 == chd5);
    EXPECT_FALSE(chd5 == chd6);
    EXPECT_FALSE(chd6 == chd6);
    EXPECT_TRUE(chd4 == chd4);
    EXPECT_TRUE(chd5 == chd5);
    chd6.addBytes(b2);
    EXPECT_FALSE(chd4 == chd5);
    EXPECT_FALSE(chd4 == chd6);
    EXPECT_TRUE(chd5 == chd6);
    EXPECT_TRUE(chd6 == chd6);

    ChainHashData chd7{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd8{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd9{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd10{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    ChainHashData chd11{Format(CHAINHASH_CONSTANT_COUNT_SALT)};
    EXPECT_FALSE(chd7 == chd8);
    EXPECT_FALSE(chd8 == chd9);
    EXPECT_FALSE(chd9 == chd9);
    Bytes startnum{8};
    Bytes startnum2{8};
    chd7.addBytes(startnum);
    chd8.addBytes(startnum);
    chd9.addBytes(startnum2);
    chd10.addBytes(startnum2);
    chd11.addBytes(startnum);
    EXPECT_FALSE(chd7 == chd8);
    EXPECT_FALSE(chd8 == chd9);
    EXPECT_FALSE(chd9 == chd9);
    EXPECT_FALSE(chd9 == chd10);
    Bytes b3{0};
    Bytes b4{247};
    chd7.addBytes(b3);
    EXPECT_TRUE(chd7 == chd7);
    EXPECT_FALSE(chd7 == chd8);
    chd8.addBytes(b3);
    EXPECT_TRUE(chd8 == chd8);
    EXPECT_TRUE(chd7 == chd8);
    chd11.addBytes(b4);
    EXPECT_FALSE(chd7 == chd11);
    EXPECT_FALSE(chd8 == chd11);
    EXPECT_TRUE(chd11 == chd11);
    chd10.addBytes(b4);
    EXPECT_TRUE(chd10 == chd10);
    EXPECT_FALSE(chd10 == chd11);
    EXPECT_FALSE(chd9 == chd10);
    chd9.addBytes(b4);
    EXPECT_TRUE(chd9 == chd9);
    EXPECT_TRUE(chd9 == chd10);
    EXPECT_FALSE(chd9 == chd11);
    EXPECT_FALSE(chd10 == chd11);
    
    ChainHashData chd12{Format(CHAINHASH_COUNT_SALT)};
    ChainHashData chd13{Format(CHAINHASH_COUNT_SALT)};
    ChainHashData chd14{Format(CHAINHASH_COUNT_SALT)};
    Bytes startnum3{8};
    EXPECT_FALSE(chd12 == chd13);
    EXPECT_FALSE(chd13 == chd14);
    EXPECT_FALSE(chd14 == chd14);
    chd12.addBytes(startnum3);
    chd13.addBytes(startnum3);
    chd14.addBytes(Bytes(8));
    EXPECT_FALSE(chd14 == chd12);
    EXPECT_FALSE(chd14 == chd13);
    EXPECT_TRUE(chd12 == chd12);
    EXPECT_TRUE(chd13 == chd13);
    EXPECT_TRUE(chd12 == chd13);
    EXPECT_TRUE(chd14 == chd14);

    ChainHashData chd15{Format(CHAINHASH_QUADRATIC)};
    ChainHashData chd16{Format(CHAINHASH_QUADRATIC)};
    ChainHashData chd17{Format(CHAINHASH_QUADRATIC)};
    ChainHashData chd18{Format(CHAINHASH_QUADRATIC)};
    Bytes startnum4{8};
    EXPECT_FALSE(chd15 == chd16);
    EXPECT_FALSE(chd16 == chd17);
    EXPECT_FALSE(chd17 == chd17);
    EXPECT_FALSE(chd18 == chd18);
    EXPECT_FALSE(chd15 == chd18);
    chd15.addBytes(startnum4);
    chd16.addBytes(startnum4);
    chd17.addBytes(startnum4);
    chd18.addBytes(Bytes(8));
    EXPECT_FALSE(chd15 == chd16);
    EXPECT_FALSE(chd16 == chd17);
    EXPECT_FALSE(chd17 == chd17);
    EXPECT_FALSE(chd18 == chd18);
    EXPECT_FALSE(chd15 == chd18);
    Bytes b5{8};
    Bytes b6{8};
    Bytes b7{8};
    chd15.addBytes(b5);
    chd16.addBytes(b5);
    chd17.addBytes(b5);
    chd18.addBytes(b5);
    EXPECT_FALSE(chd15 == chd16);
    EXPECT_FALSE(chd16 == chd17);
    EXPECT_FALSE(chd17 == chd17);
    EXPECT_FALSE(chd18 == chd18);
    EXPECT_FALSE(chd15 == chd18);
    chd15.addBytes(b6);
    chd16.addBytes(b6);
    chd17.addBytes(b6);
    chd18.addBytes(b6);
    EXPECT_FALSE(chd15 == chd16);
    EXPECT_FALSE(chd16 == chd17);
    EXPECT_FALSE(chd17 == chd17);
    EXPECT_FALSE(chd18 == chd18);
    EXPECT_FALSE(chd15 == chd18);
    chd15.addBytes(Bytes(8));
    chd16.addBytes(b7);
    chd17.addBytes(b7);
    chd18.addBytes(b7);
    EXPECT_FALSE(chd15 == chd16);
    EXPECT_TRUE(chd16 == chd17);
    EXPECT_TRUE(chd17 == chd17);
    EXPECT_TRUE(chd18 == chd18);
    EXPECT_FALSE(chd15 == chd18);
    EXPECT_TRUE(chd16 == chd16);
    EXPECT_TRUE(chd15 == chd15);
    EXPECT_FALSE(chd16 == chd18);
}
