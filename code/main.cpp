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
    freopen("test_cases/test_ori.txt", "r", stdin);
#endif
    using namespace VanitasBot;

    Utilities::Timer::resetStartTime();

    BitEngine::BitBoard board;

    IOEngine::initIOEngine();
    IOEngine::initBoard(board);
    Utilities::Logger::showBitboard(board);
    IOEngine::readInputAndRecover(board);
    Utilities::Logger::showBitboard(board);

    printf("PassedTimeBefore: %d\n", Utilities::Timer::getPassedTime());

    // BitEngine::Move chosenMove;
    // chosenMove = SearchEngine::search(board);

    // printf("PassedTimeAfter: %d\n", Utilities::Timer::getPassedTime());

    // printf("ScoreBefore: %d\n", SearchEngine::evaluate(board));
    // BitEngine::applyMove(board, chosenMove);
    // Utilities::Logger::showBitboard(board);
    // BitEngine::SwitchPlayer(board);
    // printf("ScoreAfter: %d\n", SearchEngine::evaluate(board));

    // io.outputAnswer(chosenMove);

    return 0;
}
