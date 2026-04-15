// 保证发布版不存在
#ifdef _DEBUG

#include "code/Utilities/Timer/Timer.h"

namespace VanitasBot::Utilities {

Timer::Timer(): startTime(Clock::now()), timeout(Time_ms(1000)) {}

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

inline void Timer::setTimeout(Time_ms timeout) {
    this->timeout = timeout;
}

inline bool Timer::isTimeout() {
    return getPassedTime() >= timeout;
}

}  // namespace VanitasBot::Utilities

#endif
