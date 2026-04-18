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

uint64_t perft(BitEngine::BitBoard& board, int depth) {
    if (depth == 0)
        return 1;

    BitEngine::MoveList moves;
    BitEngine::generateAllMoves(board, moves);

    uint64_t nodes = 0;
    for (int i = 0; i < moves.count; ++i) {
        BitEngine::applyMove(board, moves.moves[i]);
        nodes += perft(board, depth - 1);
        BitEngine::resetMove(board, moves.moves[i]);
    }
    return nodes;
}

// 在 main 函数里调用：
void runPerftTest() {
    BitEngine::BitBoard initialBoard;  // 你的初始棋盘
    IOEngine::initBoard(initialBoard);
    auto start = std::chrono::high_resolution_clock::now();

    // 开局测 Depth 2 即可，大概 400万 个节点
    uint64_t nodes = perft(initialBoard, 3);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Nodes: " << nodes << std::endl;
    std::cout << "Time: " << diff.count() << " s\n";
    std::cout << "NPS (Nodes Per Second): " << (nodes / diff.count()) / 1000000.0 << " M/s\n";
}

int main() {
    assert(false);  // release模式检测
    freopen("test_cases/test_beg.txt", "r", stdin);
    Timer::resetStartTime();
    IOEngine::initIOEngine();

    BitEngine::BitBoard board;
    IOEngine::initBoard(board);
    // Logger::showBitboard(board);
    IOEngine::readInputAndRecover(board);
    Logger::showBitboard(board);

    BitEngine::Move chosenMove;
    chosenMove = search(board);

    IOEngine::outputAnswer(chosenMove);

    BitEngine::applyMove(board, chosenMove);
    Logger::showBitboard(board);

    return 0;
}
