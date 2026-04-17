#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace VanitasBot::Utilities {
// 规范信息类型
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Time_ms = std::chrono::milliseconds;

// 默认超时时间配置（1秒）
constexpr Time_ms DEFAULT_TIMEOUT = Time_ms(1000);

// 静态类Timer
class Timer {
   private:
    // Timer全局单例模式
    Timer() = default;
    ~Timer() = default;
    // 记录初始化时间（程序启动时初始化）
    static inline TimePoint startTime = Clock::now();
    // 超时时间（初始为一秒）
    static inline Time_ms timeout = DEFAULT_TIMEOUT;
    // 超时标志指针
    bool* isTimeOut = nullptr;

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

    // 获取全局开始时间
    inline static TimePoint getStartTime() {
        return startTime;
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

    // 注册超时标志指针
    inline void registerTimeout(bool* isTimeOut) {
        this->isTimeOut = isTimeOut;
        if (isTimeOut) {
            *isTimeOut = false;
        }
    }

    // 检查超时并更新标志
    inline void checkTimeOut() {
        if (isTimeOut) {
            *isTimeOut = getPassedTime() >= timeout.count();
        }
    }
};
}  // namespace VanitasBot::Utilities
#endif
