#include "format.h"

#include "gtest/gtest.h"

#include "settings.h"

TEST(FormatClass, returnTypes){
    // tests if the return types of the functions are correct
    Format f{CHAINHASH_NORMAL};
    EXPECT_EQ(typeid(CHModes), typeid(f.getChainMode()));
    EXPECT_EQ(typeid(std::vector<NameLen>), typeid(f.getNameLenList()));
}

TEST(FormatClass, constructor){
    // tests if the constructor throws for illegal values
    EXPECT_THROW(Format f{CHModes(0)}, std::invalid_argument);
    EXPECT_THROW(Format f{CHModes(-1)}, std::invalid_argument);
    EXPECT_THROW(Format f{CHModes(-255)}, std::invalid_argument);
    EXPECT_THROW(Format f{CHModes(MAX_CHAINHASHMODE_NUMBER + 1)}, std::invalid_argument);
}

TEST(FormatClass, getChainHashMode){
    // tests if the getChainHashMode function returns the correct chainhash mode
    Format f{CHAINHASH_NORMAL};
    EXPECT_EQ(CHAINHASH_NORMAL, f.getChainMode());
    Format f2{CHAINHASH_CONSTANT_SALT};
    EXPECT_EQ(CHAINHASH_CONSTANT_SALT, f2.getChainMode());
    Format f3{CHAINHASH_COUNT_SALT};
    EXPECT_EQ(CHAINHASH_COUNT_SALT, f3.getChainMode());
    Format f4{CHAINHASH_CONSTANT_COUNT_SALT};
    EXPECT_EQ(CHAINHASH_CONSTANT_COUNT_SALT, f4.getChainMode());
    Format f5{CHAINHASH_QUADRATIC};
    EXPECT_EQ(CHAINHASH_QUADRATIC, f5.getChainMode());
}

TEST(FormatClass, getNameLenList){
    // tests if the getNameLenList function returns the correct list
    Format f{CHAINHASH_NORMAL};
    EXPECT_EQ(0, f.getNameLenList().size());

    Format f2{CHAINHASH_CONSTANT_SALT};
    std::vector<NameLen> name_lens2;
    name_lens2.push_back(NameLen{"S", 0});
    for(int i = 0; i < f2.getNameLenList().size(); i++){
        EXPECT_EQ(name_lens2[i].name, f2.getNameLenList()[i].name);
        EXPECT_EQ(name_lens2[i].len, f2.getNameLenList()[i].len);
    }
    
    Format f3{CHAINHASH_COUNT_SALT};
    std::vector<NameLen> name_lens3;
    name_lens3.push_back(NameLen{"SN", 8});
    for(int i = 0; i < f3.getNameLenList().size(); i++){
        EXPECT_EQ(name_lens3[i].name, f3.getNameLenList()[i].name);
        EXPECT_EQ(name_lens3[i].len, f3.getNameLenList()[i].len);
    }
    
    Format f4{CHAINHASH_CONSTANT_COUNT_SALT};
    std::vector<NameLen> name_lens4;
    name_lens4.push_back(NameLen{"SN", 8});
    name_lens4.push_back(NameLen{"S", 0});
    for(int i = 0; i < f4.getNameLenList().size(); i++){
        EXPECT_EQ(name_lens4[i].name, f4.getNameLenList()[i].name);
        EXPECT_EQ(name_lens4[i].len, f4.getNameLenList()[i].len);
    }

    Format f5{CHAINHASH_QUADRATIC};
    std::vector<NameLen> name_lens5;
    name_lens5.push_back(NameLen{"SN", 8});
    name_lens5.push_back(NameLen{"A", 8});
    name_lens5.push_back(NameLen{"B", 8});
    name_lens5.push_back(NameLen{"C", 8});
    for(int i = 0; i < f5.getNameLenList().size(); i++){
        EXPECT_EQ(name_lens5[i].name, f5.getNameLenList()[i].name);
        EXPECT_EQ(name_lens5[i].len, f5.getNameLenList()[i].len);
    }
}

TEST(FormatClass, operatorEquals){
    // tests if the operator== works correctly
    Format f{CHAINHASH_NORMAL};
    Format f2{CHAINHASH_NORMAL};
    EXPECT_TRUE(f == f2);
    Format f3{CHAINHASH_CONSTANT_SALT};
    EXPECT_FALSE(f == f3);
    Format f4{CHAINHASH_COUNT_SALT};
    EXPECT_FALSE(f == f4);
    Format f5{CHAINHASH_CONSTANT_COUNT_SALT};
    EXPECT_FALSE(f == f5);
    Format f6{CHAINHASH_QUADRATIC};
    EXPECT_FALSE(f == f6);
    Format f7{CHAINHASH_CONSTANT_SALT};
    EXPECT_TRUE(f3 == f7);
    Format f8{CHAINHASH_COUNT_SALT};
    EXPECT_TRUE(f4 == f8);
    Format f9{CHAINHASH_CONSTANT_COUNT_SALT};
    EXPECT_TRUE(f5 == f9);
    Format f10{CHAINHASH_QUADRATIC};
    EXPECT_TRUE(f6 == f10);
}