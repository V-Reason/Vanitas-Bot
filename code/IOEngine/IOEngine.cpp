#include "IOEngine.h"

#include <cstdio>

namespace VanitasBot::IOEngine {

// 初始化IO配置
void initIOEngine() {
    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);
}

// 初始化棋盘
void initBoard(BitEngine::BitBoard& board) {
    board.blacks = 0;
    board.whites = 0;
    board.arrows = 0;

    // 黑方四个棋子位置：(2,0), (5,0), (0,2), (7,2)
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(2, 0)));
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(5, 0)));
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(0, 2)));
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(7, 2)));

    // 白方四个棋子位置：(0,5), (7,5), (2,7), (5,7)
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(0, 5)));
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(7, 5)));
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(2, 7)));
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(5, 7)));
}

// 将int[][]类型的棋盘转换为BitEngine::BitBoard类型
void readInputAndRecover(const int gridInfo[8][8], BitEngine::BitBoard& board) {
    board.blacks = 0;
    board.whites = 0;
    board.arrows = 0;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (gridInfo[x][y] == 1) {
                BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(x, y)));
            } else if (gridInfo[x][y] == -1) {
                BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(x, y)));
            } else if (gridInfo[x][y] == 2) {
                BitEngine::setBit(board.arrows, BitEngine::makeMask(BitEngine::XYToIndex(x, y)));
            }
        }
    }
}

// 输出答案，将Move转换为六个坐标变量
void OutputAnswer(BitEngine::Move chosenMove, int& startX, int& startY, int& resultX, int& resultY, int& obstacleX,
                  int& obstacleY) {
    BitEngine::indexToXY(BitEngine::getFrom(chosenMove), startX, startY);
    BitEngine::indexToXY(BitEngine::getTo(chosenMove), resultX, resultY);
    BitEngine::indexToXY(BitEngine::getArrow(chosenMove), obstacleX, obstacleY);
}

}  // namespace VanitasBot::IOEngine
