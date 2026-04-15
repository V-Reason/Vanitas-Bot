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
    Timer();
    ~Timer() = default;
    // 记录初始化时间
    TimePoint startTime;
    // 超时时间（初始为一秒）
    Time_ms timeout;

   public:
    // Timer全局单例模式
    Timer(const Timer&) = delete;
    // 实现定时器的单例模式
    static Timer& instance();

    // 记录程序开始时间戳
    inline void startTimer();
    // 获得现在时间戳
    inline TimePoint getNowTimePoint();
    // 获得过去的时间(ms)
    inline Time_ms getPassedTime();
    // 设置超时时间
    inline void setTimeout(Time_ms timeout);
    // 检查是否超时
    inline bool isTimeout();
};
}  // namespace VanitasBot::Utilities
#endif
