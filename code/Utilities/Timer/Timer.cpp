// 保证发布版不存在
#ifdef _DEBUG

#include "code/Utilities/Timer/Timer.h"

namespace VanitasBot::Utilities {

Timer::TimePoint Timer::startTime = Clock::now();

Timer& Timer::instance() {
    static Timer timer;
    return timer;
}

inline void Timer::startTimer() {
    startTime = Clock::now();
}

inline Timer::TimePoint Timer::getNowTimePoint() {
    return Clock::now();
}

inline Timer::Time_ms Timer::getPassedTime() {
    auto now = Clock::now();
    return std::chrono::duration_cast<Time_ms>(now - startTime);
}

}  // namespace VanitasBot::Utilities

#endif
