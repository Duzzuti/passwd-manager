#include "pwfunc.h"

#include <gtest/gtest.h>

#include "rng.h"
#include "settings.h"
#include "sha256.h"
#include "sha384.h"
#include "sha512.h"
#include "test_settings.cpp"
#include "timer.h"
#include "utility.h"

TEST(PWFUNCClass, returnTypes) {
    // check if the return types are correct
    std::unique_ptr<Hash> hash = std::make_unique<sha256>();
    PwFunc pwf = PwFunc(std::move(hash));

    EXPECT_EQ(typeid(ErrorStruct<bool>), typeid(PwFunc::isPasswordValid("test")));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhash("test", 1)));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithConstantSalt("test", 1, "test")));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithCountAndConstantSalt("test", 1, 1, "test")));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithCountSalt("test", 1, 1)));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithQuadraticCountSalt("test", 1, 10, 89, 28, 18)));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhash(Bytes(10), 1)));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithConstantSalt(Bytes(10), 1, "test")));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithCountAndConstantSalt(Bytes(10), 1, 1, "test")));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithCountSalt(Bytes(10), 1, 1)));
    EXPECT_EQ(typeid(ErrorStruct<Bytes>), typeid(pwf.chainhashWithQuadraticCountSalt(Bytes(10), 1, 10, 89, 28, 18)));
    EXPECT_EQ(typeid(TimedResult), typeid(pwf.chainhashTimed("test", 1)));
    EXPECT_EQ(typeid(TimedResult), typeid(pwf.chainhashWithConstantSaltTimed("test", 1, "test")));
    EXPECT_EQ(typeid(TimedResult), typeid(pwf.chainhashWithCountAndConstantSaltTimed("test", 1, 1, "test")));
    EXPECT_EQ(typeid(TimedResult), typeid(pwf.chainhashWithCountSaltTimed("test", 1, 1)));
    EXPECT_EQ(typeid(TimedResult), typeid(pwf.chainhashWithQuadraticCountSaltTimed("test", 1, 10, 89, 28, 18)));
}

TEST(PWFUNCClass, passwordvalid) {
    // check if the password validation works
    // check if the settings are correctly set for this unittest
    EXPECT_EQ(8, MIN_PASS_LEN);

    // too short
    ErrorStruct<bool> tmp;
    tmp = PwFunc::isPasswordValid("testtes");
    EXPECT_EQ(FAIL, tmp.success);
    EXPECT_EQ("Password is too short, it has to be at least 8 characters long", getErrorMessage(tmp, false));
    EXPECT_THROW(tmp.returnValue(), std::logic_error);
    tmp = PwFunc::isPasswordValid("");
    EXPECT_EQ(FAIL, tmp.success);
    EXPECT_EQ("Password is too short, it has to be at least 8 characters long", getErrorMessage(tmp, false));
    EXPECT_THROW(tmp.returnValue(), std::logic_error);
    tmp = PwFunc::isPasswordValid("1234567");
    EXPECT_EQ(FAIL, tmp.success);
    EXPECT_EQ("Password is too short, it has to be at least 8 characters long", getErrorMessage(tmp, false));
    EXPECT_THROW(tmp.returnValue(), std::logic_error);

    // illegal characters
    for (unsigned char c = 0; c < 255; c++) {
        tmp = PwFunc::isPasswordValid("testtes" + std::string(1, c));
        if (VALID_PASS_CHARSET.find(c) == std::string::npos) {
            EXPECT_EQ(FAIL, tmp.success);
            EXPECT_EQ("Password contains illegal character: '" + std::string(1, c) + "'", getErrorMessage(tmp, false));
            EXPECT_THROW(tmp.returnValue(), std::logic_error);
        } else {
            EXPECT_EQ(SUCCESS, tmp.success);
            EXPECT_EQ("getErrorMessage was called on a succeeded ErrorStruct", getErrorMessage(tmp, false));
            EXPECT_EQ(true, tmp.returnValue());
        }
    }
    for (unsigned char c = 0; c < 255; c++) {
        tmp = PwFunc::isPasswordValid(std::string(1, c) + "testtes");
        if (VALID_PASS_CHARSET.find(c) == std::string::npos) {
            EXPECT_EQ(FAIL, tmp.success);
            EXPECT_EQ("Password contains illegal character: '" + std::string(1, c) + "'", getErrorMessage(tmp, false));
            EXPECT_THROW(tmp.returnValue(), std::logic_error);
        } else {
            EXPECT_EQ(SUCCESS, tmp.success);
            EXPECT_EQ("getErrorMessage was called on a succeeded ErrorStruct", getErrorMessage(tmp, false));
            EXPECT_EQ(true, tmp.returnValue());
        }
    }
    for (unsigned char c = 0; c < 255; c++) {
        tmp = PwFunc::isPasswordValid("tes" + std::string(1, c) + "test");
        if (VALID_PASS_CHARSET.find(c) == std::string::npos) {
            EXPECT_EQ(FAIL, tmp.success);
            EXPECT_EQ("Password contains illegal character: '" + std::string(1, c) + "'", getErrorMessage(tmp, false));
            EXPECT_THROW(tmp.returnValue(), std::logic_error);
        } else {
            EXPECT_EQ(SUCCESS, tmp.success);
            EXPECT_EQ("getErrorMessage was called on a succeeded ErrorStruct", getErrorMessage(tmp, false));
            EXPECT_EQ(true, tmp.returnValue());
        }
    }
}

TEST(PWFUNCClass, input_output) {
    // check if the input and output has the correct format
    for (int kj = 0; kj < 3; kj++) {
        std::unique_ptr<Hash> hash;
        switch (kj) {
            case 0:
                hash = std::make_unique<sha256>();
                break;
            case 1:
                hash = std::make_unique<sha384>();
                break;
            case 2:
                hash = std::make_unique<sha512>();
                break;
        }
        int hash_size = hash->getHashSize();
        PwFunc pwf = PwFunc(std::move(hash));

        for (int i = 0; i < TEST_MAX_PW_LEN; i++) {
            Bytes rand_b(8);
            RNG::fill_random_bytes(rand_b, 2);   // max ca. 65000
            u_int64_t l1 = rand_b.toLong() / 6;  // max ca. 10000 iterations
            if (MIN_ITERATIONS > l1) {
                l1 = MIN_ITERATIONS;
            } else if (MAX_ITERATIONS < l1) {
                l1 = TEST_MAX_PW_ITERS;
            }
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l2 = rand_b.toLong();
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l3 = rand_b.toLong();
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l4 = rand_b.toLong();
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l5 = rand_b.toLong();

            // create same random password string and Bytes
            Bytes passwordbytes(TEST_MAX_PW_LEN);
            RNG::fill_random_bytes(passwordbytes, i);
            std::string password = bytesToString(passwordbytes);

            // testing chainhashes with string data
            Bytes tmp = pwf.chainhash(password, l1).returnValue();
            EXPECT_EQ(hash_size, tmp.getLen());
            Bytes tmp2 = pwf.chainhashWithConstantSalt(password, l1).returnValue();
            EXPECT_EQ(hash_size, tmp2.getLen());
            Bytes tmp3 = pwf.chainhashWithConstantSalt(password, l1, RNG::get_random_string(100)).returnValue();
            EXPECT_EQ(hash_size, tmp3.getLen());
            Bytes tmp4 = pwf.chainhashWithCountSalt(password, l1).returnValue();
            EXPECT_EQ(hash_size, tmp4.getLen());
            Bytes tmp5 = pwf.chainhashWithCountSalt(password, l1, l2).returnValue();
            EXPECT_EQ(hash_size, tmp5.getLen());
            Bytes tmp6 = pwf.chainhashWithCountAndConstantSalt(password, l1).returnValue();
            EXPECT_EQ(hash_size, tmp6.getLen());
            Bytes tmp7 = pwf.chainhashWithCountAndConstantSalt(password, l1, l2, RNG::get_random_string(100)).returnValue();
            EXPECT_EQ(hash_size, tmp7.getLen());
            Bytes tmp8 = pwf.chainhashWithQuadraticCountSalt(password, l1).returnValue();
            EXPECT_EQ(hash_size, tmp8.getLen());
            Bytes tmp9 = pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5).returnValue();
            EXPECT_EQ(hash_size, tmp9.getLen());

            // testing chainhashes with Bytes data
            Bytes tmp10 = pwf.chainhash(passwordbytes, l1).returnValue();
            EXPECT_EQ(hash_size, tmp10.getLen());
            Bytes tmp12 = pwf.chainhashWithConstantSalt(passwordbytes, l1).returnValue();
            EXPECT_EQ(hash_size, tmp12.getLen());
            Bytes tmp13 = pwf.chainhashWithConstantSalt(passwordbytes, l1, RNG::get_random_string(100)).returnValue();
            EXPECT_EQ(hash_size, tmp13.getLen());
            Bytes tmp14 = pwf.chainhashWithCountSalt(passwordbytes, l1).returnValue();
            EXPECT_EQ(hash_size, tmp14.getLen());
            Bytes tmp15 = pwf.chainhashWithCountSalt(passwordbytes, l1, l2).returnValue();
            EXPECT_EQ(hash_size, tmp15.getLen());
            Bytes tmp16 = pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1).returnValue();
            EXPECT_EQ(hash_size, tmp16.getLen());
            Bytes tmp17 = pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, RNG::get_random_string(100)).returnValue();
            EXPECT_EQ(hash_size, tmp17.getLen());
            Bytes tmp18 = pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1).returnValue();
            EXPECT_EQ(hash_size, tmp18.getLen());
            Bytes tmp19 = pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5).returnValue();
            EXPECT_EQ(hash_size, tmp19.getLen());

            // testing the timed chainhashes
            Timer timer;
            timer.start();
            TimedResult tmp20 = pwf.chainhashTimed(password, 56);
            timer.stop();
            EXPECT_EQ(hash_size, tmp20.result.getLen());
            EXPECT_NEAR(56, timer.getTime(), 1);
            timer.start();
            TimedResult tmp21 = pwf.chainhashWithConstantSaltTimed(password, 36, RNG::get_random_string(100));
            timer.stop();
            EXPECT_EQ(hash_size, tmp21.result.getLen());
            EXPECT_NEAR(36, timer.getTime(), 1);
            timer.start();
            TimedResult tmp22 = pwf.chainhashWithCountAndConstantSaltTimed(password, 12, 369, RNG::get_random_string(100));
            timer.stop();
            EXPECT_EQ(hash_size, tmp22.result.getLen());
            EXPECT_NEAR(12, timer.getTime(), 1);
            timer.start();
            TimedResult tmp23 = pwf.chainhashWithCountSaltTimed(password, 1, 369);
            timer.stop();
            EXPECT_EQ(hash_size, tmp23.result.getLen());
            EXPECT_NEAR(1, timer.getTime(), 1);
            timer.start();
            TimedResult tmp24 = pwf.chainhashWithQuadraticCountSaltTimed(password, 89, 369, 369, 369, 369);
            timer.stop();
            EXPECT_EQ(hash_size, tmp24.result.getLen());
            EXPECT_NEAR(89, timer.getTime(), 1);

            // testing the timed chainhashes with Bytes data
            timer.start();
            tmp21 = pwf.chainhashTimed(passwordbytes, 56);
            timer.stop();
            EXPECT_EQ(hash_size, tmp21.result.getLen());
            EXPECT_NEAR(56, timer.getTime(), 1);
            timer.start();
            tmp22 = pwf.chainhashWithConstantSaltTimed(passwordbytes, 36, RNG::get_random_string(100));
            timer.stop();
            EXPECT_EQ(hash_size, tmp22.result.getLen());
            EXPECT_NEAR(36, timer.getTime(), 1);
            timer.start();
            tmp23 = pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 12, 369, RNG::get_random_string(100));
            timer.stop();
            EXPECT_EQ(hash_size, tmp23.result.getLen());
            EXPECT_NEAR(12, timer.getTime(), 1);
            timer.start();
            tmp24 = pwf.chainhashWithCountSaltTimed(passwordbytes, 1, 369);
            timer.stop();
            EXPECT_EQ(hash_size, tmp24.result.getLen());
            EXPECT_NEAR(1, timer.getTime(), 1);
            timer.start();
            tmp20 = pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 89, 369, 369, 369, 369);
            timer.stop();
            EXPECT_EQ(hash_size, tmp20.result.getLen());
            EXPECT_NEAR(89, timer.getTime(), 1);
        }
    }
}

TEST(PWFUNCClass, consistency) {
    // check if the output is consistent (same input -> same output)
    for (int kj = 0; kj < 3; kj++) {
        std::unique_ptr<Hash> hash;
        switch (kj) {
            case 0:
                hash = std::make_unique<sha256>();
                break;
            case 1:
                hash = std::make_unique<sha384>();
                break;
            case 2:
                hash = std::make_unique<sha512>();
                break;
        }
        int hash_size = hash->getHashSize();
        PwFunc pwf = PwFunc(std::move(hash));

        for (int i = 0; i < TEST_MAX_PW_LEN; i++) {
            // create same random password string and Bytes
            Bytes passwordbytes(TEST_MAX_PW_LEN);
            RNG::fill_random_bytes(passwordbytes, i);
            std::string password = bytesToString(passwordbytes);

            // create same random salt
            std::string s = RNG::get_random_string(100);

            Bytes rand_b(8);
            RNG::fill_random_bytes(rand_b, 2);
            u_int64_t l1 = rand_b.toLong() / 6;

            // cap the number of iterations
            if (MIN_ITERATIONS > l1) {
                l1 = MIN_ITERATIONS;
            } else if (MAX_ITERATIONS < l1) {
                l1 = TEST_MAX_PW_ITERS;
            }

            // generating random arguments
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l2 = rand_b.toLong();
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l3 = rand_b.toLong();
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l4 = rand_b.toLong();
            RNG::fill_random_bytes(rand_b, 8);
            u_int64_t l5 = rand_b.toLong();

            // testing chainhashes with string data
            EXPECT_EQ(pwf.chainhash(password, l1).returnValue(), pwf.chainhash(password, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1).returnValue(), pwf.chainhashWithConstantSalt(password, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1, s).returnValue(), pwf.chainhashWithConstantSalt(password, l1, s).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1).returnValue(), pwf.chainhashWithCountSalt(password, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1, l2).returnValue(), pwf.chainhashWithCountSalt(password, l1, l2).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1).returnValue(), pwf.chainhashWithCountAndConstantSalt(password, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1, l2, s).returnValue(), pwf.chainhashWithCountAndConstantSalt(password, l1, l2, s).returnValue());
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1).returnValue(), pwf.chainhashWithQuadraticCountSalt(password, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5).returnValue(), pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5).returnValue());

            // testing chainhashes with Bytes data
            EXPECT_EQ(pwf.chainhash(passwordbytes, l1).returnValue(), pwf.chainhash(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithConstantSalt(passwordbytes, l1).returnValue(), pwf.chainhashWithConstantSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithConstantSalt(passwordbytes, l1, s).returnValue(), pwf.chainhashWithConstantSalt(passwordbytes, l1, s).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountSalt(passwordbytes, l1).returnValue(), pwf.chainhashWithCountSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountSalt(passwordbytes, l1, l2).returnValue(), pwf.chainhashWithCountSalt(passwordbytes, l1, l2).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1).returnValue(), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, s).returnValue(), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, s).returnValue());
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1).returnValue(), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5).returnValue(), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5).returnValue());

            // testing chainhashes with string and Bytes data
            EXPECT_EQ(pwf.chainhash(password, l1).returnValue(), pwf.chainhash(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1).returnValue(), pwf.chainhashWithConstantSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithConstantSalt(password, l1, s).returnValue(), pwf.chainhashWithConstantSalt(passwordbytes, l1, s).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1).returnValue(), pwf.chainhashWithCountSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountSalt(password, l1, l2).returnValue(), pwf.chainhashWithCountSalt(passwordbytes, l1, l2).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1).returnValue(), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSalt(password, l1, l2, s).returnValue(), pwf.chainhashWithCountAndConstantSalt(passwordbytes, l1, l2, s).returnValue());
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1).returnValue(), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1).returnValue());
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSalt(password, l1, l2, l3, l4, l5).returnValue(), pwf.chainhashWithQuadraticCountSalt(passwordbytes, l1, l2, l3, l4, l5).returnValue());

            // TIMED CHAINHASHES (tiemour 0 to gurantee that the same iterations is used)
            // testing chainhashes with string data
            EXPECT_EQ(pwf.chainhashTimed(password, 0), pwf.chainhashTimed(password, 0));
            EXPECT_EQ(pwf.chainhashWithConstantSaltTimed(password, 0), pwf.chainhashWithConstantSaltTimed(password, 0));
            EXPECT_EQ(pwf.chainhashWithConstantSaltTimed(password, 0, s), pwf.chainhashWithConstantSaltTimed(password, 0, s));
            EXPECT_EQ(pwf.chainhashWithCountSaltTimed(password, 0), pwf.chainhashWithCountSaltTimed(password, 0));
            EXPECT_EQ(pwf.chainhashWithCountSaltTimed(password, 0, l2), pwf.chainhashWithCountSaltTimed(password, 0, l2));
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSaltTimed(password, 0), pwf.chainhashWithCountAndConstantSaltTimed(password, 0));
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSaltTimed(password, 0, l2, s), pwf.chainhashWithCountAndConstantSaltTimed(password, 0, l2, s));
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSaltTimed(password, 0), pwf.chainhashWithQuadraticCountSaltTimed(password, 0));
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSaltTimed(password, 0, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSaltTimed(password, 0, l2, l3, l4, l5));

            // testing chainhashes with Bytes data
            EXPECT_EQ(pwf.chainhashTimed(passwordbytes, 0), pwf.chainhashTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithConstantSaltTimed(passwordbytes, 0), pwf.chainhashWithConstantSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithConstantSaltTimed(passwordbytes, 0, s), pwf.chainhashWithConstantSaltTimed(passwordbytes, 0, s));
            EXPECT_EQ(pwf.chainhashWithCountSaltTimed(passwordbytes, 0), pwf.chainhashWithCountSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithCountSaltTimed(passwordbytes, 0, l2), pwf.chainhashWithCountSaltTimed(passwordbytes, 0, l2));
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0), pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0, l2, s), pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0, l2, s));
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 0), pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 0, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 0, l2, l3, l4, l5));

            // testing chainhashes with string and Bytes data
            EXPECT_EQ(pwf.chainhashTimed(password, 0), pwf.chainhashTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithConstantSaltTimed(password, 0), pwf.chainhashWithConstantSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithConstantSaltTimed(password, 0, s), pwf.chainhashWithConstantSaltTimed(passwordbytes, 0, s));
            EXPECT_EQ(pwf.chainhashWithCountSaltTimed(password, 0), pwf.chainhashWithCountSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithCountSaltTimed(password, 0, l2), pwf.chainhashWithCountSaltTimed(passwordbytes, 0, l2));
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSaltTimed(password, 0), pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithCountAndConstantSaltTimed(password, 0, l2, s), pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0, l2, s));
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSaltTimed(password, 0), pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 0));
            EXPECT_EQ(pwf.chainhashWithQuadraticCountSaltTimed(password, 0, l2, l3, l4, l5), pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 0, l2, l3, l4, l5));
        }
    }
}

TEST(PWFUNCClass, correctness) {
    // checks if the output is correct
    std::unique_ptr<Hash> hash = std::make_unique<sha256>();
    PwFunc pwf = PwFunc(std::move(hash));
    // setting the passwords (string and Bytes) and salt
    std::string password = "Password";
    Bytes passwordbytes = stringToBytes(password);
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
    EXPECT_EQ(phashchain, pwf.chainhash(password, 3).returnValue().toHex());
    EXPECT_EQ(phashchaincnoargs, pwf.chainhashWithConstantSalt(password, 3).returnValue().toHex());
    EXPECT_EQ(phashchainc, pwf.chainhashWithConstantSalt(password, 3, s).returnValue().toHex());
    EXPECT_EQ(phashchaincountnoargs, pwf.chainhashWithCountSalt(password, 3).returnValue().toHex());
    EXPECT_EQ(phashchaincount, pwf.chainhashWithCountSalt(password, 3, 100).returnValue().toHex());
    EXPECT_EQ(phashchainccountnoargs, pwf.chainhashWithCountAndConstantSalt(password, 3).returnValue().toHex());
    EXPECT_EQ(phashchainccount, pwf.chainhashWithCountAndConstantSalt(password, 3, 100, s).returnValue().toHex());
    EXPECT_EQ(phashchainquadnoargs, pwf.chainhashWithQuadraticCountSalt(password, 3).returnValue().toHex());
    EXPECT_EQ(phashchainquad, pwf.chainhashWithQuadraticCountSalt(password, 3, 90, 5, 8, 3).returnValue().toHex());

    // testing chainhashes with Bytes data
    EXPECT_EQ(phashchain, pwf.chainhash(passwordbytes, 3).returnValue().toHex());
    EXPECT_EQ(phashchaincnoargs, pwf.chainhashWithConstantSalt(passwordbytes, 3).returnValue().toHex());
    EXPECT_EQ(phashchainc, pwf.chainhashWithConstantSalt(passwordbytes, 3, s).returnValue().toHex());
    EXPECT_EQ(phashchaincountnoargs, pwf.chainhashWithCountSalt(passwordbytes, 3).returnValue().toHex());
    EXPECT_EQ(phashchaincount, pwf.chainhashWithCountSalt(passwordbytes, 3, 100).returnValue().toHex());
    EXPECT_EQ(phashchainccountnoargs, pwf.chainhashWithCountAndConstantSalt(passwordbytes, 3).returnValue().toHex());
    EXPECT_EQ(phashchainccount, pwf.chainhashWithCountAndConstantSalt(passwordbytes, 3, 100, s).returnValue().toHex());
    EXPECT_EQ(phashchainquadnoargs, pwf.chainhashWithQuadraticCountSalt(passwordbytes, 3).returnValue().toHex());
    EXPECT_EQ(phashchainquad, pwf.chainhashWithQuadraticCountSalt(passwordbytes, 3, 90, 5, 8, 3).returnValue().toHex());
}

TEST(PWFUNCClass, timedIters) {
    for (int kj = 0; kj < 3; kj++) {
        std::unique_ptr<Hash> hash;
        switch (kj) {
            case 0:
                hash = std::make_unique<sha256>();
                break;
            case 1:
                hash = std::make_unique<sha384>();
                break;
            case 2:
                hash = std::make_unique<sha512>();
                break;
        }
        int hash_size = hash->getHashSize();
        PwFunc pwf = PwFunc(std::move(hash));
        // create same random password string and Bytes
        Bytes passwordbytes(20);
        RNG::fill_random_bytes(passwordbytes, 20);
        std::string password = bytesToString(passwordbytes);

        // create same random salt
        std::string s = RNG::get_random_string(100);

        Bytes rand_b(8);
        RNG::fill_random_bytes(rand_b, 2);
        u_int64_t l1 = rand_b.toLong() / 6;

        // cap the number of iterations
        if (MIN_ITERATIONS > l1) {
            l1 = MIN_ITERATIONS;
        } else if (MAX_ITERATIONS < l1) {
            l1 = TEST_MAX_PW_ITERS;
        }

        // generating random arguments
        RNG::fill_random_bytes(rand_b, 8);
        u_int64_t l2 = rand_b.toLong();
        RNG::fill_random_bytes(rand_b, 8);
        u_int64_t l3 = rand_b.toLong();
        RNG::fill_random_bytes(rand_b, 8);
        u_int64_t l4 = rand_b.toLong();
        RNG::fill_random_bytes(rand_b, 8);
        u_int64_t l5 = rand_b.toLong();

        TimedResult tmp[5];
        tmp[0] = pwf.chainhashTimed(password, 672);
        tmp[1] = pwf.chainhashWithConstantSaltTimed(password, 167, s);
        tmp[2] = pwf.chainhashWithCountSaltTimed(password, 10, l2);
        tmp[3] = pwf.chainhashWithCountAndConstantSaltTimed(password, 0, l2, s);
        tmp[4] = pwf.chainhashWithQuadraticCountSaltTimed(password, 612, l2, l3, l4, l5);

        EXPECT_EQ(hash_size, tmp[0].result.getLen());
        EXPECT_EQ(tmp[0].result, pwf.chainhash(password, tmp[0].iterations).returnValue());
        EXPECT_EQ(hash_size, tmp[1].result.getLen());
        EXPECT_EQ(tmp[1].result, pwf.chainhashWithConstantSalt(password, tmp[1].iterations, s).returnValue());
        EXPECT_EQ(hash_size, tmp[2].result.getLen());
        EXPECT_EQ(tmp[2].result, pwf.chainhashWithCountSalt(password, tmp[2].iterations, l2).returnValue());
        EXPECT_EQ(hash_size, tmp[3].result.getLen());
        EXPECT_EQ(tmp[3].result, pwf.chainhashWithCountAndConstantSalt(password, tmp[3].iterations, l2, s).returnValue());
        EXPECT_EQ(hash_size, tmp[4].result.getLen());
        EXPECT_EQ(tmp[4].result, pwf.chainhashWithQuadraticCountSalt(password, tmp[4].iterations, l2, l3, l4, l5).returnValue());

        tmp[0] = pwf.chainhashTimed(passwordbytes, 672);
        tmp[1] = pwf.chainhashWithConstantSaltTimed(passwordbytes, 167, s);
        tmp[2] = pwf.chainhashWithCountSaltTimed(passwordbytes, 10, l2);
        tmp[3] = pwf.chainhashWithCountAndConstantSaltTimed(passwordbytes, 0, l2, s);
        tmp[4] = pwf.chainhashWithQuadraticCountSaltTimed(passwordbytes, 612, l2, l3, l4, l5);

        EXPECT_EQ(hash_size, tmp[0].result.getLen());
        EXPECT_EQ(tmp[0].result, pwf.chainhash(passwordbytes, tmp[0].iterations).returnValue());
        EXPECT_EQ(hash_size, tmp[1].result.getLen());
        EXPECT_EQ(tmp[1].result, pwf.chainhashWithConstantSalt(passwordbytes, tmp[1].iterations, s).returnValue());
        EXPECT_EQ(hash_size, tmp[2].result.getLen());
        EXPECT_EQ(tmp[2].result, pwf.chainhashWithCountSalt(passwordbytes, tmp[2].iterations, l2).returnValue());
        EXPECT_EQ(hash_size, tmp[3].result.getLen());
        EXPECT_EQ(tmp[3].result, pwf.chainhashWithCountAndConstantSalt(passwordbytes, tmp[3].iterations, l2, s).returnValue());
        EXPECT_EQ(hash_size, tmp[4].result.getLen());
        EXPECT_EQ(tmp[4].result, pwf.chainhashWithQuadraticCountSalt(passwordbytes, tmp[4].iterations, l2, l3, l4, l5).returnValue());
    }
}
