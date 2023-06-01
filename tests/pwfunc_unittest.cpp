#include "pwfunc.h"

#include "gtest/gtest.h"
#include "rng.h"
#include "settings.h"
#include "sha256.h"
#include "test_settings.cpp"
#include "test_utils.h"  //provide gen_random for random strings

TEST(PWFUNCClass, returnTypes) {
    // check if the return types are correct
    Hash* hash = new sha256();
    PwFunc pwf = PwFunc(hash);

    EXPECT_EQ(typeid(ErrorStruct<bool>), typeid(pwf.isPasswordValid("test")));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhash("test", 1)));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithConstantSalt("test", 1, "test")));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithCountAndConstantSalt("test", 1, 1, "test")));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithCountSalt("test", 1, 1)));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithQuadraticCountSalt("test", 1, 10, 89, 28, 18)));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhash(Bytes(10), 1)));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithConstantSalt(Bytes(10), 1, "test")));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithCountAndConstantSalt(Bytes(10), 1, 1, "test")));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithCountSalt(Bytes(10), 1, 1)));
    EXPECT_EQ(typeid(Bytes), typeid(pwf.chainhashWithQuadraticCountSalt(Bytes(10), 1, 10, 89, 28, 18)));

    delete hash;
}

TEST(PWFUNCClass, input_output) {
    // check if the input and output has the correct format
    sha256* hash = new sha256();
    PwFunc pwf = PwFunc(hash);

    for (int i = 0; i < TEST_MAX_PW_LEN; i++) {
        Bytes rand_b;
        rand_b.setBytes(RNG::get_random_bytes(2));  // max ca. 65000
        u_int64_t l1 = toLong(rand_b) / 6;          // max ca. 10000 iterations
        if (MIN_ITERATIONS > l1) {
            l1 = MIN_ITERATIONS;
        } else if (MAX_ITERATIONS < l1) {
            l1 = TEST_MAX_PW_ITERS;
        }
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l2 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l3 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l4 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l5 = toLong(rand_b);
        std::string p = gen_random_string(i);
        Bytes tmp = pwf.chainhash(p, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp.getLen());
        Bytes tmp2 = pwf.chainhashWithConstantSalt(p, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp2.getLen());
        Bytes tmp3 = pwf.chainhashWithConstantSalt(p, l1, gen_random_string(100));
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp3.getLen());
        Bytes tmp4 = pwf.chainhashWithCountSalt(p, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp4.getLen());
        Bytes tmp5 = pwf.chainhashWithCountSalt(p, l1, l2);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp5.getLen());
        Bytes tmp6 = pwf.chainhashWithCountAndConstantSalt(p, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp6.getLen());
        Bytes tmp7 = pwf.chainhashWithCountAndConstantSalt(p, l1, l2, gen_random_string(100));
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp7.getLen());
        Bytes tmp8 = pwf.chainhashWithQuadraticCountSalt(p, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp8.getLen());
        Bytes tmp9 = pwf.chainhashWithQuadraticCountSalt(p, l1, l2, l3, l4, l5);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp9.getLen());
    }

    delete hash;
}

TEST(PWFUNCClass, consistency) {
    // check if the output is consistent (same input -> same output)
    sha256* hash = new sha256();
    PwFunc pwf = PwFunc(hash);

    for (int i = 0; i < TEST_MAX_PW_LEN; i++) {
        std::string p = gen_random_string(i);
        std::string s = gen_random_string(100);
        Bytes rand_b;
        rand_b.setBytes(RNG::get_random_bytes(2));
        u_int64_t l1 = toLong(rand_b) / 6;
        if (MIN_ITERATIONS > l1) {
            l1 = MIN_ITERATIONS;
        } else if (MAX_ITERATIONS < l1) {
            l1 = TEST_MAX_PW_ITERS;
        }
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l2 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l3 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l4 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l5 = toLong(rand_b);
        EXPECT_EQ(pwf.chainhash(p, l1), pwf.chainhash(p, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(p, l1), pwf.chainhashWithConstantSalt(p, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(p, l1, s), pwf.chainhashWithConstantSalt(p, l1, s));
        EXPECT_EQ(pwf.chainhashWithCountSalt(p, l1), pwf.chainhashWithCountSalt(p, l1));
        EXPECT_EQ(pwf.chainhashWithCountSalt(p, l1, l2), pwf.chainhashWithCountSalt(p, l1, l2));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(p, l1), pwf.chainhashWithCountAndConstantSalt(p, l1));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(p, l1, l2, s), pwf.chainhashWithCountAndConstantSalt(p, l1, l2, s));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(p, l1), pwf.chainhashWithQuadraticCountSalt(p, l1));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(p, l1, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSalt(p, l1, l2, l3, l4, l5));
    }

    delete hash;
}

TEST(PWFUNCClass, correctness) {
    // checks if the output is correct
    sha256* hash = new sha256();
    PwFunc pwf = PwFunc(hash);
    std::string p = "Password";
    std::string s = "salt";
    std::string phash = "e7cf3ef4f17c3999a94f2c6f612e8a888e5b1026878e4e19398b23bd38ec221a";
    std::string phashs = "c990daffb01d8091e8af9a91227370cfa6f7d1c44e7ab062940486c57676418c";

    std::string phashchain = "7be274414bc74dd332d5a0fafc94e5a1da20d091553260a2c1790a82529380f6";
    for (auto& c : phashchain) c = toupper(c);
    std::string phashchaincnoargs = "cdce8ae59e9719084a7131fc06587b57149d9dd4597ef7e1227be8cb5c978c76";
    for (auto& c : phashchaincnoargs) c = toupper(c);
    std::string phashchainc = "93e7864f8588196472da975c85499afa8fd93faaec4cd0e612fc8f78b8e4515d";
    for (auto& c : phashchainc) c = toupper(c);
    std::string phashchaincountnoargs = "7130c6b356a8e27894f5853d8f92efe9569280c60a3c2f4a5174cc03490d2aa4";
    for (auto& c : phashchaincountnoargs) c = toupper(c);
    std::string phashchaincount = "d347755d295b474bfd8a9d48134c523fadccf6b67633253c16784aa1f66dc244";
    for (auto& c : phashchaincount) c = toupper(c);
    std::string phashchainccount = "278979129cd421d273d7e2fa3ac8721004c6c49da2c7f9f4d451da2267de746a";
    for (auto& c : phashchainccount) c = toupper(c);
    std::string phashchainccountnoargs = "a2930b8c9f4e5d9c10d06534b4cf8b204fbfba8ca12ef4f3aa0092f7ac707269";
    for (auto& c : phashchainccountnoargs) c = toupper(c);
    std::string phashchainquad = "63498771e61f08553a6d510290659c5d80b77e065efbd686180ed7b97195e20e";
    for (auto& c : phashchainquad) c = toupper(c);
    std::string phashchainquadnoargs = "701254884c61f81db5f59c7b386fb8356bc04d59c6edeb31741d21bfc8161dbb";
    for (auto& c : phashchainquadnoargs) c = toupper(c);

    EXPECT_EQ(phashchain, toHex(pwf.chainhash(p, 3)));
    EXPECT_EQ(phashchaincnoargs, toHex(pwf.chainhashWithConstantSalt(p, 3)));
    EXPECT_EQ(phashchainc, toHex(pwf.chainhashWithConstantSalt(p, 3, s)));
    EXPECT_EQ(phashchaincountnoargs, toHex(pwf.chainhashWithCountSalt(p, 3)));
    EXPECT_EQ(phashchaincount, toHex(pwf.chainhashWithCountSalt(p, 3, 100)));
    EXPECT_EQ(phashchainccountnoargs, toHex(pwf.chainhashWithCountAndConstantSalt(p, 3)));
    EXPECT_EQ(phashchainccount, toHex(pwf.chainhashWithCountAndConstantSalt(p, 3, 100, s)));
    EXPECT_EQ(phashchainquadnoargs, toHex(pwf.chainhashWithQuadraticCountSalt(p, 3)));
    EXPECT_EQ(phashchainquad, toHex(pwf.chainhashWithQuadraticCountSalt(p, 3, 90, 5, 8, 3)));

    delete hash;
}
