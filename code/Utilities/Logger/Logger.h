#pragma once
// #define LOGGER_ON

#ifdef LOGGER_ON
#ifndef LOGGER_H
#define LOGGER_H

#include "BitEngine/BitEngine.h"

#include <iostream>
#include <string>

namespace VanitasBot::Utilities {
using namespace std;

// 重置色
const string RESET = "\033[0m";

// 前景色
// clang-format off
const string _RED    = "\033[31m";
const string _GREEN  = "\033[32m";
const string _YELLOW = "\033[33m";
const string _BLUE   = "\033[34m";
const string _WHITE  = "\033[37m";
const string _GRAY   = "\033[90m";
const string _BLACK  = "\033[30m";

// 背景色
const string BG_BLACK  = "\033[40m";
const string BG_RED    = "\033[41m";
const string BG_BLUE   = "\033[44m";
const string BG_GREEN  = "\033[42m";
const string BG_YELLOW = "\033[43m";
const string BG_GRAY   = "\033[47m";
// clang-format on

// 字符映射
const string EmptyCell = string(_GRAY) + "· " + string(RESET);                       // 灰点
const string FullCell = string(BG_GRAY) + string(_GREEN) + "O " + string(RESET);     // 绿圈
const string WhiteAmazon = string(BG_BLUE) + string(_WHITE) + "W " + string(RESET);  // 蓝底白W
const string BlackAmazon = string(BG_RED) + string(_BLACK) + "B " + string(RESET);   // 红底黑B
const string BlockedCell = string(BG_BLACK) + string(_GRAY) + "X " + string(RESET);  // 黑底灰叉

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
