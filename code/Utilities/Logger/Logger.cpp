// #define LOGGER_ON

#ifdef LOGGER_ON
#include "Logger.h"

#include <string>

namespace VanitasBot::Utilities {
using namespace std;
// 重置色
#define RESET "\033[0m"

// 前景色
#define _RED    "\033[31m"
#define _GREEN  "\033[32m"
#define _YELLOW "\033[33m"
#define _BLUE   "\033[34m"
#define _WHITE  "\033[37m"
#define _GRAY   "\033[90m"
#define _BLACK  "\033[30m"

// 背景色
#define BG_BLACK  "\033[40m"
#define BG_RED    "\033[41m"
#define BG_BLUE   "\033[44m"
#define BG_GREEN  "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_GRAY   "\033[47m"

// 字符映射
const string EmptyCell = string(_GRAY) + "· " + string(RESET);                       // 灰点
const string FullCell = string(BG_GRAY) + string(_GREEN) + "O " + string(RESET);     // 绿圈
const string WhiteAmazon = string(BG_BLUE) + string(_WHITE) + "W " + string(RESET);  // 蓝底白W
const string BlackAmazon = string(BG_RED) + string(_BLACK) + "B " + string(RESET);   // 红底黑B
const string BlockedCell = string(BG_BLACK) + string(_GRAY) + "X " + string(RESET);  // 黑底灰叉

void Logger::showBitmap(const BitEngine::Bitmap& bitmap, const char* title) {
    std::cout << "--- " << title << " ---" << std::endl;
    std::cout << "  0 1 2 3 4 5 6 7  (x)" << std::endl;
    for (int y = 0; y < BitEngine::AMAZON_BOARD_LENGTH; ++y) {
        std::cout << y << " ";  // 打印行号(y)
        for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; ++x) {
            BitEngine::Index index = BitEngine::XYToIndex(x, y);
            BitEngine::Bitmap mask = BitEngine::makeMask(index);

            // 如果该位置的 bit 为 1，则打印亮色方块或 1
            if (bitmap & mask) {
                std::cout << FullCell;  // 也可以改成 "@ " 或 "* "
            } else {
                std::cout << EmptyCell;
            }
        }
        std::cout << std::endl;
    }
    std::cout << "------------------" << std::endl;
}

void Logger::showBitboard(const BitEngine::BitBoard& board, const char* title) {
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "  0 1 2 3 4 5 6 7  (x)" << std::endl;
    for (int y = 0; y < BitEngine::AMAZON_BOARD_LENGTH; ++y) {
        std::cout << y << " ";  // 打印行号(y)
        for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; ++x) {
            BitEngine::Index index = BitEngine::XYToIndex(x, y);
            BitEngine::Bitmap mask = BitEngine::makeMask(index);

            // 按优先级检查该位置是什么东西
            if (board.blacks & mask) {
                std::cout << BlackAmazon;  // B 代表黑棋 (Black)
            } else if (board.whites & mask) {
                std::cout << WhiteAmazon;  // W 代表白棋 (White)
            } else if (board.arrows & mask) {
                std::cout << BlockedCell;  // X 代表箭矢障碍 (Arrow)
            } else {
                std::cout << EmptyCell;  // . 代表空地
            }
        }
        std::cout << std::endl;
    }

    // 打印当前轮到谁行动
    std::cout << "Player Turn: ";
    if (board.player == BitEngine::Player::BLACK) {
        std::cout << "BLACK (B)" << std::endl;
    } else {
        std::cout << "WHITE (W)" << std::endl;
    }
    std::cout << "=====================" << std::endl;
}
}  // namespace VanitasBot::Utilities
#endif
