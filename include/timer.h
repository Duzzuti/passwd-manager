//
// Created by Ed Lang on 6/1/23.
//

#pragma once
#include <chrono>
#include <iostream>
#include <vector>

class Timer {
    /*
    timer class for performance measurement
    class measures in milliseconds
    */
   private:
    // last lap/start time
    std::chrono::steady_clock::time_point last_time;
    std::vector<u_int64_t> laps;  // lap times
    bool started = false;
    bool stopped = false;

   private:
    // throw an exception if the timer is not in the correct state
    void require_started();
    void require_stopped();
    void require_not_started();
    void require_not_stopped();

   public:
    void start();                //  start the timer
    void stop();                 // stop the timer
    u_int64_t recordTime();      // save the time needed since the last start() or recordTime() call
    u_int64_t peekTime();        // returns the time needed since the last start() or recordTime() call
    u_int64_t getAverageTime();  // returns the average time per record and stop (getTime() / getLaps())
    u_int64_t getTime();         // returns the timedelta between the start and stop call
    size_t getLaps();            // returns the number of recordTime() and stop() calls (the total number of measurements you made)/ returns the number of laps (record and stop calls)
};