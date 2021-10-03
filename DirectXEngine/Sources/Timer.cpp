#include "Timer.h"
#include <cassert>

Timer::Timer() {
    start = stop = std::chrono::high_resolution_clock::now();
}

double Timer::getMillisecondsElapsed() {

    if (isRunning) {
        auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
        return elapsed.count();
    }
    else {
        auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
        return elapsed.count();
    }
}

void Timer::restartTimer() {
    isRunning = true;
    start = std::chrono::high_resolution_clock::now();
}

bool Timer::stopTimer() {
    assert(isRunning && "Timer is not running");
    stop = std::chrono::high_resolution_clock::now();
    isRunning = false;
    return true;
}

bool Timer::startTimer() {
    assert(!isRunning && "Timer is running");
    start = std::chrono::high_resolution_clock::now();
    isRunning = true;
    return true;
}
