#include "Logger.h"
namespace VanitasBot::Utilities {

void Logger::showBitmap(BitEngine::Bitmap bitmap, const char* title) {
    std::cout << "--- " << title << " ---" << std::endl;
    std::cout << "  0 1 2 3 4 5 6 7  (x)" << std::endl;
    for (int y = 0; y < BitEngine::AMAZON_BOARD_LENGTH; ++y) {
        std::cout << y << " ";  // 打印行号(y)
        for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; ++x) {
            BitEngine::Index index = BitEngine::XYToIndex(x, y);
            BitEngine::Bitmap mask = BitEngine::makeMask(index);

            // 如果该位置的 bit 为 1，则打印亮色方块或 1
            if (bitmap & mask) {
                std::cout << "1 ";  // 也可以改成 "@ " 或 "* "
            } else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "------------------" << std::endl;
}

void Logger::showBitboard(BitEngine::BitBoard board) {
    std::cout << "=== Current Board ===" << std::endl;
    std::cout << "  0 1 2 3 4 5 6 7  (x)" << std::endl;
    for (int y = 0; y < BitEngine::AMAZON_BOARD_LENGTH; ++y) {
        std::cout << y << " ";  // 打印行号(y)
        for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; ++x) {
            BitEngine::Index index = BitEngine::XYToIndex(x, y);
            BitEngine::Bitmap mask = BitEngine::makeMask(index);

            // 按优先级检查该位置是什么东西
            if (board.blacks & mask) {
                std::cout << "B ";  // B 代表黑棋 (Black)
            } else if (board.whites & mask) {
                std::cout << "W ";  // W 代表白棋 (White)
            } else if (board.arrows & mask) {
                std::cout << "X ";  // X 代表箭矢障碍 (Arrow)
            } else {
                std::cout << ". ";  // . 代表空地
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
