//
// Created by Ed Lang on 6/1/23.
//

#pragma once
#include <chrono>
#include <vector>

class Timer {
    std::chrono::steady_clock::time_point last_time;
    std::vector<unsigned long> laps;
    bool started = false;
    bool stopped = false;

    void require_started() {
        if (!started) {
            throw std::runtime_error("Timer must be started before calling this method");
        }
    }

    void require_stopped() {
        if (!stopped) {
            throw std::runtime_error("Timer must be stopped before calling this method");
        }
    }

   public:
        void start(); //  start the timer
        void stop(); // stop the timer
        unsigned long recordTime(); // save the time needed since the last start() or recordTime() call
        unsigned long peekTime();  // returns the timedelta between the start and stop call
        unsigned long getAverageTime(); // returns the average time per record and stop (getTime() / getLaps())
        unsigned long getTime(); // returns the timedelta between the start and stop call
        size_t getLaps(); // returns the number of recordTime() and stop() calls (the total number of measurements you made)/ returns the number of laps (record and stop calls)
};