#pragma once

#include <chrono>

class Timer final {
public:
    Timer();
    double getMillisecondsElapsed();
    void restartTimer();
    bool stopTimer();
    bool startTimer();

private:
    bool isRunning = false;

#ifdef _WIN32
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
#else // for linux lol
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::system_clock> stop;
#endif
};
