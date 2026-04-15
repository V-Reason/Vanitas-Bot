#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "BitEngine/BitEngine.h"

#include <iostream>

namespace VanitasBot::Utilities {

// 静态日志类
class Logger {
   public:
    // 打印棋盘信息（控制台可视化）
    static void showBitmap(BitEngine::Bitmap bitmap, const char* title = "Bitmap");
    static void showBitboard(BitEngine::BitBoard board);
};
}  // namespace VanitasBot::Utilities

#endif
