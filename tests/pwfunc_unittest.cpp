#include "pwfunc.h"

#include "gtest/gtest.h"
#include "rng.h"
#include "settings.h"
#include "sha256.h"
#include "test_settings.cpp"
#include "utility.h"

TEST(PWFUNCClass, returnTypes) {
    // check if the return types are correct
    Hash* hash = new sha256();
    PwFunc pwf = PwFunc(hash);

    EXPECT_EQ(typeid(ErrorStruct<bool>), typeid(PwFunc::isPasswordValid("test")));
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

TEST(PWFUNCClass, passwordvalid) {
    // check if the password validation works
    // check if the settings are correctly set for this unittest
    EXPECT_EQ(8, MIN_PASS_LEN);

    // too short
    ErrorStruct<bool> tmp;
    tmp = PwFunc::isPasswordValid("testtes");
    EXPECT_EQ(false, tmp.success);
    EXPECT_EQ("Password is too short, it has to be at least 8 characters long", tmp.error);
    EXPECT_EQ(false, tmp.returnValue);
    tmp = PwFunc::isPasswordValid("");
    EXPECT_EQ(false, tmp.success);
    EXPECT_EQ("Password is too short, it has to be at least 8 characters long", tmp.error);
    EXPECT_EQ(false, tmp.returnValue);
    tmp = PwFunc::isPasswordValid("1234567");
    EXPECT_EQ(false, tmp.success);
    EXPECT_EQ("Password is too short, it has to be at least 8 characters long", tmp.error);
    EXPECT_EQ(false, tmp.returnValue);

    // illegal characters
    for (unsigned char c = 0; c < 255; c++) {
        tmp = PwFunc::isPasswordValid("testtes" + std::string(1, c));
        if (VALID_PASS_CHARSET.find(c) == std::string::npos) {
            EXPECT_EQ(false, tmp.success);
            EXPECT_EQ("Password contains illegal character: '" + std::string(1, c) + "'", tmp.error);
            EXPECT_EQ(false, tmp.returnValue);
        } else {
            EXPECT_EQ(true, tmp.success);
            EXPECT_EQ("", tmp.error);
            EXPECT_EQ(true, tmp.returnValue);
        }
    }
    for (unsigned char c = 0; c < 255; c++) {
        tmp = PwFunc::isPasswordValid(std::string(1, c) + "testtes");
        if (VALID_PASS_CHARSET.find(c) == std::string::npos) {
            EXPECT_EQ(false, tmp.success);
            EXPECT_EQ("Password contains illegal character: '" + std::string(1, c) + "'", tmp.error);
            EXPECT_EQ(false, tmp.returnValue);
        } else {
            EXPECT_EQ(true, tmp.success);
            EXPECT_EQ("", tmp.error);
            EXPECT_EQ(true, tmp.returnValue);
        }
    }
    for (unsigned char c = 0; c < 255; c++) {
        tmp = PwFunc::isPasswordValid("tes" + std::string(1, c) + "test");
        if (VALID_PASS_CHARSET.find(c) == std::string::npos) {
            EXPECT_EQ(false, tmp.success);
            EXPECT_EQ("Password contains illegal character: '" + std::string(1, c) + "'", tmp.error);
            EXPECT_EQ(false, tmp.returnValue);
        } else {
            EXPECT_EQ(true, tmp.success);
            EXPECT_EQ("", tmp.error);
            EXPECT_EQ(true, tmp.returnValue);
        }
    }
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

        // create same random password string and Bytes
        Bytes passwordbytes;
        std::string password;
        passwordbytes.setBytes(RNG::get_random_bytes(i));
        password = charVecToString(passwordbytes.getBytes());

        // testing chainhashes with string data
        Bytes tmp = pwf.chainhash(password, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp.getLen());
        Bytes tmp2 = pwf.chainhashWithConstantSalt(password, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp2.getLen());
        Bytes tmp3 = pwf.chainhashWithConstantSalt(password, l1, charVecToString(Bytes(100).getBytes()));
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp3.getLen());
        Bytes tmp4 = pwf.chainhashWithCountSalt(password, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp4.getLen());
        Bytes tmp5 = pwf.chainhashWithCountSalt(password, l1, l2);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp5.getLen());
        Bytes tmp6 = pwf.chainhashWithCountAndConstantSalt(password, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp6.getLen());
        Bytes tmp7 = pwf.chainhashWithCountAndConstantSalt(password, l1, l2, charVecToString(Bytes(100).getBytes()));
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp7.getLen());
        Bytes tmp8 = pwf.chainhashWithQuadraticCountSalt(password, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp8.getLen());
        Bytes tmp9 = pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp9.getLen());

        // testing chainhashes with Bytes data
        Bytes tmp10 = pwf.chainhash(passwordbytes, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp10.getLen());
        Bytes tmp12 = pwf.chainhashWithConstantSalt(passwordbytes, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp12.getLen());
        Bytes tmp13 = pwf.chainhashWithConstantSalt(passwordbytes, l1, charVecToString(Bytes(100).getBytes()));
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp13.getLen());
        Bytes tmp14 = pwf.chainhashWithCountSalt(passwordbytes, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp14.getLen());
        Bytes tmp15 = pwf.chainhashWithCountSalt(passwordbytes, l1, l2);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp15.getLen());
        Bytes tmp16 = pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp16.getLen());
        Bytes tmp17 = pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, charVecToString(Bytes(100).getBytes()));
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp17.getLen());
        Bytes tmp18 = pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp18.getLen());
        Bytes tmp19 = pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5);
        EXPECT_EQ(SHA256_DIGEST_LENGTH, tmp19.getLen());
    }

    delete hash;
}

TEST(PWFUNCClass, consistency) {
    // check if the output is consistent (same input -> same output)
    sha256* hash = new sha256();
    PwFunc pwf = PwFunc(hash);

    for (int i = 0; i < TEST_MAX_PW_LEN; i++) {
        // create same random password string and Bytes
        Bytes passwordbytes;
        std::string password;
        passwordbytes.setBytes(RNG::get_random_bytes(i));
        password = charVecToString(passwordbytes.getBytes());

        // create same random salt
        std::string s = charVecToString(Bytes(100).getBytes());

        Bytes rand_b;
        rand_b.setBytes(RNG::get_random_bytes(2));
        u_int64_t l1 = toLong(rand_b) / 6;

        // cap the number of iterations
        if (MIN_ITERATIONS > l1) {
            l1 = MIN_ITERATIONS;
        } else if (MAX_ITERATIONS < l1) {
            l1 = TEST_MAX_PW_ITERS;
        }

        // generating random arguments
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l2 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l3 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l4 = toLong(rand_b);
        rand_b.setBytes(RNG::get_random_bytes(8));
        u_int64_t l5 = toLong(rand_b);

        // testing chainhashes with string data
        EXPECT_EQ(pwf.chainhash(password, l1), pwf.chainhash(password, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1), pwf.chainhashWithConstantSalt(password, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1, s), pwf.chainhashWithConstantSalt(password, l1, s));
        EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1), pwf.chainhashWithCountSalt(password, l1));
        EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1, l2), pwf.chainhashWithCountSalt(password, l1, l2));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1), pwf.chainhashWithCountAndConstantSalt(password, l1));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1, l2, s), pwf.chainhashWithCountAndConstantSalt(password, l1, l2, s));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1), pwf.chainhashWithQuadraticCountSalt(password, l1));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5));

        // testing chainhashes with Bytes data
        EXPECT_EQ(pwf.chainhash(passwordbytes, l1), pwf.chainhash(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(passwordbytes, l1), pwf.chainhashWithConstantSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(passwordbytes, l1, s), pwf.chainhashWithConstantSalt(passwordbytes, l1, s));
        EXPECT_EQ(pwf.chainhashWithCountSalt(passwordbytes, l1), pwf.chainhashWithCountSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithCountSalt(passwordbytes, l1, l2), pwf.chainhashWithCountSalt(passwordbytes, l1, l2));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, s), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, s));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5));

        // testing chainhashes with string and Bytes data
        EXPECT_EQ(pwf.chainhash(password, l1), pwf.chainhash(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1), pwf.chainhashWithConstantSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1, s), pwf.chainhashWithConstantSalt(passwordbytes, l1, s));
        EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1), pwf.chainhashWithCountSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1, l2), pwf.chainhashWithCountSalt(passwordbytes, l1, l2));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1, l2, s), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, s));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1));
        EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5));
    }

    delete hash;
}

TEST(PWFUNCClass, correctness) {
    // checks if the output is correct
    sha256* hash = new sha256();
    PwFunc pwf = PwFunc(hash);
    // setting the passwords (string and Bytes) and salt
    std::string password = "Password";
    Bytes passwordbytes;
    passwordbytes.setBytes(std::vector<unsigned char>(password.begin(), password.end()));
    std::string s = "salt";

    // password hash and password hash hash
    std::string phash = "e7cf3ef4f17c3999a94f2c6f612e8a888e5b1026878e4e19398b23bd38ec221a";
    std::string phashs = "c990daffb01d8091e8af9a91227370cfa6f7d1c44e7ab062940486c57676418c";

    // set expected values
    std::string phashchain = "7be274414bc74dd332d5a0fafc94e5a1da20d091553260a2c1790a82529380f6";
    for (auto& c : phashchain) c = toupper(c);
    std::string phashchaincnoargs = "7be274414bc74dd332d5a0fafc94e5a1da20d091553260a2c1790a82529380f6";
    for (auto& c : phashchaincnoargs) c = toupper(c);
    std::string phashchainc = "039ab013c15e3ab761494d988a3e9298b4d00dac7ccade1f87a790676b7dfde0";
    for (auto& c : phashchainc) c = toupper(c);
    std::string phashchaincountnoargs = "730054a5d0584622d3d11c33fcd351d09a7e216e6206bc0fcb027457ec93c822";
    for (auto& c : phashchaincountnoargs) c = toupper(c);
    std::string phashchaincount = "9d2013058d1d46ba1ffc9951a884d1e015a3aa6cbb6296505ed357890e187a2b";
    for (auto& c : phashchaincount) c = toupper(c);
    std::string phashchainccount = "93c329aa4a97175b6b28a38348991ad4d5f96a99aabe95c932c034c7f6ad1ad9";
    for (auto& c : phashchainccount) c = toupper(c);
    std::string phashchainccountnoargs = "730054a5d0584622d3d11c33fcd351d09a7e216e6206bc0fcb027457ec93c822";
    for (auto& c : phashchainccountnoargs) c = toupper(c);
    std::string phashchainquad = "89fd7f1a7d50f2fb881d8f97e88a407b9f029b900262237d0b6ccda0c071f16e";
    for (auto& c : phashchainquad) c = toupper(c);
    std::string phashchainquadnoargs = "3e767758357a8f0b82b046ae08a4a60d29bc27ba04f902d33af8eb392c1aeae4";
    for (auto& c : phashchainquadnoargs) c = toupper(c);

    // testing chainhashes with string data
    EXPECT_EQ(phashchain, toHex(pwf.chainhash(password, 3)));
    EXPECT_EQ(phashchaincnoargs, toHex(pwf.chainhashWithConstantSalt(password, 3)));
    EXPECT_EQ(phashchainc, toHex(pwf.chainhashWithConstantSalt(password, 3, s)));
    EXPECT_EQ(phashchaincountnoargs, toHex(pwf.chainhashWithCountSalt(password, 3)));
    EXPECT_EQ(phashchaincount, toHex(pwf.chainhashWithCountSalt(password, 3, 100)));
    EXPECT_EQ(phashchainccountnoargs, toHex(pwf.chainhashWithCountAndConstantSalt(password, 3)));
    EXPECT_EQ(phashchainccount, toHex(pwf.chainhashWithCountAndConstantSalt(password, 3, 100, s)));
    EXPECT_EQ(phashchainquadnoargs, toHex(pwf.chainhashWithQuadraticCountSalt(password, 3)));
    EXPECT_EQ(phashchainquad, toHex(pwf.chainhashWithQuadraticCountSalt(password, 3, 90, 5, 8, 3)));

    // testing chainhashes with Bytes data
    EXPECT_EQ(phashchain, toHex(pwf.chainhash(passwordbytes, 3)));
    EXPECT_EQ(phashchaincnoargs, toHex(pwf.chainhashWithConstantSalt(passwordbytes, 3)));
    EXPECT_EQ(phashchainc, toHex(pwf.chainhashWithConstantSalt(passwordbytes, 3, s)));
    EXPECT_EQ(phashchaincountnoargs, toHex(pwf.chainhashWithCountSalt(passwordbytes, 3)));
    EXPECT_EQ(phashchaincount, toHex(pwf.chainhashWithCountSalt(passwordbytes, 3, 100)));
    EXPECT_EQ(phashchainccountnoargs, toHex(pwf.chainhashWithCountAndConstantSalt(passwordbytes, 3)));
    EXPECT_EQ(phashchainccount, toHex(pwf.chainhashWithCountAndConstantSalt(passwordbytes, 3, 100, s)));
    EXPECT_EQ(phashchainquadnoargs, toHex(pwf.chainhashWithQuadraticCountSalt(passwordbytes, 3)));
    EXPECT_EQ(phashchainquad, toHex(pwf.chainhashWithQuadraticCountSalt(passwordbytes, 3, 90, 5, 8, 3)));

    delete hash;
}
