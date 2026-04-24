#pragma once
#define LOGGER_ON

#ifdef LOGGER_ON
#ifndef LOGGER_H
#define LOGGER_H

#include "BitEngine/BitEngine.h"

#include <iostream>

namespace VanitasBot::Utilities {

// 静态日志类
class Logger {
   public:
    // 打印棋盘信息（控制台可视化）
    static void showBitmap(const BitEngine::Bitmap& bitmap, const char* title = "Bitmap");
    static void showBitboard(const BitEngine::BitBoard& board, const char* title = "BitBoard");
};
}  // namespace VanitasBot::Utilities

#endif
#endif
