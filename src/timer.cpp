/*
implementation of timer.h
*/
#include "timer.h"

#include "logger.h"

void Timer::start() {
    // start the timer
    this->require_not_started();  // it has not been started
    this->started = true;
    this->stopped = false;
    this->laps.clear();
    this->last_time = std::chrono::steady_clock::now();
}

void Timer::stop() {
    // stop the timer
    this->require_not_stopped();  // it has not been stopped
    this->require_started();      // it has been started
    this->recordTime();
    this->stopped = true;
    this->started = false;
}

u_int64_t Timer::recordTime() {
    // save the time needed since the last start() or recordTime() call
    this->require_started();      // it has been started
    this->require_not_stopped();  // it has not been stopped
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_time);
    this->laps.push_back(elapsed.count());
    this->last_time = now;
    return elapsed.count();
}

u_int64_t Timer::peekTime() {
    // returns the timedelta between the start and stop call
    this->require_started();      // it has been started
    this->require_not_stopped();  // it has not been stopped
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_time);
    return elapsed.count();
}

u_int64_t Timer::getAverageTime() {
    // returns the average time per record and stop (getTime() / getLaps())
    this->require_stopped();  // it has been stopped
    if (this->laps.empty()) {
        return 0;
    } else {
        u_int64_t sum = 0;
        for (auto lap : this->laps) {
            sum += lap;
        }
        return sum / this->laps.size();
    }
}

u_int64_t Timer::getSlowest() {
    // returns the slowest time per record and stop (max(laps))
    this->require_stopped();  // it has been stopped
    if (this->laps.empty()) {
        return 0;
    } else {
        u_int64_t max = 0;
        for (auto lap : this->laps) {
            if (lap > max) {
                max = lap;
            }
        }
        return max;
    }
}

u_int64_t Timer::getTime() {
    // returns the timedelta between the start and stop call
    this->require_stopped();  // it has been stopped
    return this->getAverageTime() * this->laps.size();
}

size_t Timer::getLaps() {
    // returns the number of recordTime() and stop() calls (the total number of measurements you made)/ returns the number of laps (record and stop calls)
    this->require_stopped();
    return this->laps.size();
}

void Timer::require_started() {
    if (!this->started) {
        PLOG_ERROR << "Timer must be started before calling this method";
        throw std::logic_error("Timer must be started before calling this method");
    }
}

void Timer::require_stopped() {
    if (!this->stopped) {
        PLOG_ERROR << "Timer must be stopped before calling this method";
        throw std::logic_error("Timer must be stopped before calling this method");
    }
}

void Timer::require_not_started() {
    if (this->started) {
        PLOG_ERROR << "This method is only available before the timer gets started";
        throw std::logic_error("This method is only available before the timer gets started");
    }
}

void Timer::require_not_stopped() {
    if (this->stopped) {
        PLOG_ERROR << "This method is only available before the timer gets stopped";
        throw std::logic_error("This method is only available before the timer gets stopped");
    }
}