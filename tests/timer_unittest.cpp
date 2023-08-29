#include "timer.h"

#include <gtest/gtest.h>

TEST(TimerClass, returnTypes) {
    // test that the return types are correct
    Timer t;
    EXPECT_EQ(typeid(void), typeid(t.start()));
    EXPECT_EQ(typeid(u_int64_t), typeid(t.recordTime()));
    EXPECT_EQ(typeid(u_int64_t), typeid(t.recordTime()));
    EXPECT_EQ(typeid(u_int64_t), typeid(t.peekTime()));
    EXPECT_EQ(typeid(void), typeid(t.stop()));
    EXPECT_EQ(typeid(u_int64_t), typeid(t.getAverageTime()));
    EXPECT_EQ(typeid(u_int64_t), typeid(t.getTime()));
    EXPECT_EQ(typeid(size_t), typeid(t.getLaps()));
}

TEST(TimerClass, throws) {
    // test that the timer starts and stops
    EXPECT_NO_THROW(Timer().start());
    EXPECT_THROW(Timer().stop(), std::logic_error);
    EXPECT_THROW(Timer().recordTime(), std::logic_error);
    EXPECT_THROW(Timer().peekTime(), std::logic_error);
    EXPECT_THROW(Timer().getAverageTime(), std::logic_error);
    EXPECT_THROW(Timer().getTime(), std::logic_error);
    EXPECT_THROW(Timer().getLaps(), std::logic_error);
    Timer t;
    t.start();
    EXPECT_THROW(t.start(), std::logic_error);
    EXPECT_NO_THROW(t.recordTime());
    EXPECT_NO_THROW(t.peekTime());
    EXPECT_THROW(t.getAverageTime(), std::logic_error);
    EXPECT_THROW(t.getTime(), std::logic_error);
    EXPECT_THROW(t.getLaps(), std::logic_error);
    EXPECT_NO_THROW(t.stop());
    t = Timer();
    t.start();
    t.stop();
    EXPECT_THROW(t.stop(), std::logic_error);
    EXPECT_THROW(t.recordTime(), std::logic_error);
    EXPECT_THROW(t.peekTime(), std::logic_error);
    EXPECT_NO_THROW(t.getAverageTime());
    EXPECT_NO_THROW(t.getTime());
    EXPECT_NO_THROW(t.getLaps());
    EXPECT_NO_THROW(t.start());
}

TEST(TimerClass, behaviour) {
    // test the behaviour of the timer
    u_int64_t lap1;
    u_int64_t lap2;
    u_int64_t peek1;
    u_int64_t peek2;
    Timer t;

    // testing one measurement
    // with start, stop, recordTime, peekTime, getAverageTime, getTime, getLaps

    t.start();
    usleep(100000);
    peek1 = t.peekTime();
    usleep(100000);
    lap1 = t.recordTime();
    usleep(100000);
    lap2 = t.recordTime();
    usleep(10000);
    peek2 = t.peekTime();
    usleep(30000);
    t.stop();

    EXPECT_EQ(3, t.getLaps());
    EXPECT_NEAR(100, peek1, 1);
    EXPECT_NEAR(200, lap1, 2);
    EXPECT_NEAR(100, lap2, 1);
    EXPECT_EQ(10, peek2);
    EXPECT_NEAR(340, t.getTime(), 2);
    EXPECT_NEAR(340 / 3, t.getAverageTime(), 1);
}