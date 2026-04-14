// 保证发布版不存在
#ifdef _DEBUG

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
    static Timer& instance() {
        static_assert(false, "Timer instance has not define");
    }

    // 记录程序开始时间戳
    static inline void startTimer() {
        static_assert(false, "startTimer has not define");
    }
    // 获得现在时间戳
    static inline TimePoint getNowTimePoint() {
        static_assert(false, "getNowTimePoint has not define");
    }
    // 获得过去的时间(ms)
    static inline Time_ms getPassedTime() {
        static_assert(false, "getPassedTime has not define");
    }
};
}  // namespace VanitasBot::Utilities

#endif
