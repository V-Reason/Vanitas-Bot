// DEBUG宏，控制assert
// #define NDEBUG
#define DEBUG
#include "BitEngine/BitEngine.h"
#include "IOEngine/IOEngine.h"
#include "SearchEngine/SearchEngine.h"
#include "Utilities/Logger/Logger.h"

#include <cassert>
#include <cstdio>

int main() {
#ifdef DEBUG
    freopen("test_cases/test_1.txt", "r", stdin);
#endif
    using namespace VanitasBot;

    BitEngine::BitBoard board;
    IOEngine::IOEngine io;

    io.initBoard(board);
    Utilities::Logger::showBitboard(board);
    io.readInputAndRecover(board);
    Utilities::Logger::showBitboard(board);

    BitEngine::Move chosenMove;
    chosenMove = SearchEngine::search(board);

    printf("%d", Utilities::Timer::getPassedTime());
    BitEngine::applyMove(board, chosenMove);
    Utilities::Logger::showBitboard(board);

    io.outputAnswer(chosenMove);

    return 0;
}
