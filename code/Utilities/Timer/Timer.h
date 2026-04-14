#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace VanitasBot::Utilities {
// 规范信息类型
using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Time_ms = std::chrono::milliseconds;

// 静态类Timer
class Timer {
   private:
    // Timer全局单例模式
    Timer() = default;
    ~Timer() = default;
    // 记录初始化时间
    static TimePoint startTime;

   public:
    // Timer全局单例模式
    Timer(const Timer&) = delete;
    // 实现定时器的单例模式
    static Timer& instance();

    // 记录程序开始时间戳
    static inline void startTimer();
    // 获得现在时间戳
    static inline TimePoint getNowTimePoint();
    // 获得过去的时间(ms)
    static inline Time_ms getPassedTime();
};
}  // namespace VanitasBot::Utilities
#endif
