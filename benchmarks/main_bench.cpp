#include "gtest/gtest.h"
#include "logger.h"

int main(int argc, char** argv) {
    // the main function that is running the tests
    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("data/log.txt", 1024 * 1024 * 10, 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}