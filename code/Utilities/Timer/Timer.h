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
    Timer(): startTime(Clock::now()), timeout(Time_ms(1000)) {}
    ~Timer() = default;
    // 记录初始化时间
    TimePoint startTime;
    // 超时时间（初始为一秒）
    Time_ms timeout;

   public:
    // Timer全局单例模式
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    // 实现定时器的单例模式
    static Timer& instance() {
        static Timer timer;
        return timer;
    }

    // 记录程序开始时间戳
    inline void startTimer() {
        startTime = Clock::now();
    }

    // 获得现在时间戳
    inline TimePoint getNowTimePoint() {
        return Clock::now();
    }

    // 获得过去的时间(ms)
    inline int getPassedTime() {
        auto now = Clock::now();
        return std::chrono::duration_cast<Time_ms>(now - startTime).count();
    }

    // 设置超时时间
    inline void setTimeout(Time_ms timeout) {
        this->timeout = timeout;
    }

    // 检查是否超时
    inline bool isTimeout() {
        return getPassedTime() >= timeout.count();
    }
};
}  // namespace VanitasBot::Utilities
#endif
