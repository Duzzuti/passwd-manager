#include "gtest/gtest.h"

int main(int argc, char** argv) {
    // the main function that is running the tests
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}