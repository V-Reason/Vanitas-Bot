#define DEBUG

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,popcnt,lzcnt")

#include "BitEngine/BitEngine.h"
#include "HashEngine/HashEngine.h"
#include "IOEngine/IOEngine.h"
#include "SearchEngine/SearchEngine.h"
#include "TTable/TTable.h"
#include "Utilities/Logger/Logger.h"

#include <cassert>

using namespace VanitasBot;
using namespace VanitasBot::BitEngine;
using namespace VanitasBot::SearchEngine;
using namespace VanitasBot::Utilities;

int main() {
#ifdef DEBUG
    assert(false);  // release模式检测
    freopen("test_cases/test_beg.txt", "r", stdin);
#endif
    Timer::resetStartTime();
    IOEngine::initIOEngine();

    BitEngine::BitBoard board;
    IOEngine::initBoard(board);
#ifdef LOGGER_ON
    Logger::showBitboard(board);
#endif
    IOEngine::readInputAndRecover(board);

    // // 模拟中局：双方各射出约12-14支箭
    // board.blacks = 0x0400040000080400ULL;
    // board.whites = 0x0020200000202000ULL;
    // board.arrows = 0x8244910248240091ULL;
    // board.player = BitEngine::Player::BLACK;

    // // 模拟中后局：第 30 步左右
    // board.blacks = 0x0000000000040100ULL;  // 黑皇后
    // board.whites = 0x0008100000000000ULL;  // 白皇后
    // board.arrows = 0x0761284A1248021FULL;  // 散落的箭矢
    // board.player = BitEngine::Player::BLACK;

    // 模拟残局：空格剩余 12 个，棋盘高度破碎
    board.blacks = (1ULL << 0) | (1ULL << 8) | (1ULL << 55) | (1ULL << 63);
    board.whites = (1ULL << 27) | (1ULL << 28) | (1ULL << 35) | (1ULL << 36);
    board.arrows = 0x7E7EBDDBDBBD7E7EULL;
    board.player = BitEngine::Player::BLACK;

#ifdef LOGGER_ON
    Logger::showBitboard(board);
#endif

    BitEngine::Move chosenMove;
    chosenMove = search(board);

    IOEngine::outputAnswer(chosenMove);

    BitEngine::applyMove(board, chosenMove);
#ifdef LOGGER_ON
    Logger::showBitboard(board);
    printf("PassedTime: %d", Timer::getPassedTime());
#endif
    return 0;
}
