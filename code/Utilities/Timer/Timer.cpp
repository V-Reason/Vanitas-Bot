#include "code/Utilities/Timer/Timer.h"

namespace VanitasBot::Utilities {
// 静态类Timer
class Timer {
   private:
    // Timer全局单例模式
    Timer() = default;
    ~Timer() = default;

   public:
    // Timer全局单例模式
    Timer(const Timer&) = delete;
    static Timer& instance() {}

    // 记录程序开始时间戳
    static inline void startTimer() {}
    // 获得现在时间戳
    static inline TimePoint getNowTimePoint() {}
    // 获得过去的时间(ms)
    static inline Time_ms getPassedTime() {}
};
}  // namespace VanitasBot::Utilities
