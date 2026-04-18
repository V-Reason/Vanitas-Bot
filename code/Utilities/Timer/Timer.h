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
    static constexpr TimeoutConfig timeoutConfigs[] = {{100, nullptr}, {500, nullptr}, {900, nullptr}};

    // 检查并更新超时状态（O(1)时间复杂度，无if判断）
    static inline void checkTimeouts() {
        int passed = getPassedTime();
        for (int i = 0; i < 3; ++i) {
            const auto& config = timeoutConfigs[i];
            config.isTimeOut && (*config.isTimeOut = passed >= config.timeoutMs);
        }
    }
};

}  // namespace VanitasBot::Utilities
#endif
