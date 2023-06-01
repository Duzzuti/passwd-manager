#include "timer.h"

void timer::start(){
    if (started) {
        throw std::runtime_error("Timer has already been started");
    }
    started = true;
    stopped = false;
    last_time = std::chrono::steady_clock::now();
}

void timer::stop(){
        this->require_started();
        this->stopped = true;
        recordTime();
}

unsigned long timer::recordTime(){
        require_started();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
        laps.push_back(elapsed.count());
        last_time = now;
        return elapsed.count();
}

unsigned long timer::peekTime(){
        require_started();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
        return elapsed.count();
}

unsigned long timer::getAverageTime() {
        require_stopped();
        if (laps.empty()) {
        return 0;
        } else {
        unsigned long sum = 0;
        for (auto lap : laps) {
            sum += lap;
        }
        return sum / laps.size();
        }
}

unsigned long timer::getTime(){
        require_stopped();
        return getAverageTime() * laps.size();
}

size_t timer::getLaps(){
        require_stopped();
        return laps.size();
}

