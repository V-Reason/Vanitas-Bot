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
    assert(false);  // release模式检测
    freopen("test_cases/test_beg.txt", "r", stdin);
    Timer::resetStartTime();
    IOEngine::initIOEngine();
    TTable::init();

    BitEngine::BitBoard board;
    IOEngine::initBoard(board);
    Logger::showBitboard(board);
    IOEngine::readInputAndRecover(board);

    // 模拟中局：双方各射出约12-14支箭
    board.blacks = 0x0400040000080400ULL;
    board.whites = 0x0020200000202000ULL;
    board.arrows = 0x8244910248240091ULL;
    board.player = BitEngine::Player::BLACK;

    // // 模拟残局：第 30 步左右
    // board.blacks = 0x0000000000040100ULL;  // 黑皇后
    // board.whites = 0x0008100000000000ULL;  // 白皇后
    // board.arrows = 0x0761284A1248021FULL;  // 散落的箭矢
    // board.player = BitEngine::Player::BLACK;

    Logger::showBitboard(board);

    BitEngine::Move chosenMove;
    chosenMove = search(board);

    IOEngine::outputAnswer(chosenMove);

    BitEngine::applyMove(board, chosenMove);
    Logger::showBitboard(board);
    printf("PassedTime: %d", Timer::getPassedTime());

    return 0;
}
