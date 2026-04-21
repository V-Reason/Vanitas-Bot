#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace VanitasBot::Utilities {
// 规范信息类型
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Time_ms = std::chrono::milliseconds;

// 超时配置结构
struct TimeoutConfig {
    int timeoutMs;
    bool* isTimeOut;
};

class Timer {
   private:
    // 全局开始时间点（程序启动时初始化）
    static inline TimePoint startTimePoint = Clock::now();

   public:
    // 获取已过去的时间（毫秒）
    static inline int getPassedTime() {
        return std::chrono::duration_cast<Time_ms>(Clock::now() - startTimePoint).count();
    }

    // 重置开始时间点
    static inline void resetStartTime() {
        startTimePoint = Clock::now();
    }

    // 编译期不定长数组配置示例
    // static constexpr TimeoutConfig timeoutConfigs[]
    //     = {{100, nullptr}, {500, nullptr}, {900, nullptr}};
    static inline TimeoutConfig timeoutConfigs[] = {{980, nullptr}};

    // 检查并更新超时状态（O(1)时间复杂度，无if判断）
    // 注意：循环次数 i 必须和上述数组长度一致
    // static inline void checkTimeouts() {
    //     int passed = getPassedTime();
    //     for (int i = 0; i < 3; ++i) {
    //         const auto& config = timeoutConfigs[i];
    //         config.isTimeOut && (*config.isTimeOut = passed >= config.timeoutMs);
    //     }
    // }

    // 特化版本
    static inline bool checkTimeouts() {
        if (getPassedTime() > timeoutConfigs[0].timeoutMs) {
            *timeoutConfigs[0].isTimeOut = true;
            return true;
        }
        return false;
    }
};

// 使用示例：
/*
// 定义超时标志
bool timeout100ms = false;
bool timeout500ms = false;
bool timeout900ms = false;

// 配置超时检测
Timer::timeoutConfigs[0].isTimeOut = &timeout100ms;
Timer::timeoutConfigs[1].isTimeOut = &timeout500ms;
Timer::timeoutConfigs[2].isTimeOut = &timeout900ms;

// 检查超时
Timer::checkTimeouts();

// 使用超时标志
if (timeout100ms) {
// 100ms超时处理
}
*/

}  // namespace VanitasBot::Utilities
#endif
