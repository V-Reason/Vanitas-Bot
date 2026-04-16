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

// 读取输入并恢复棋盘状态
void readInputAndRecover(BitEngine::BitBoard& board) {
    int turnID;
    scanf("%d", &turnID);

    int x0, y0, x1, y1, x2, y2;
    board.player = BitEngine::Player::WHITE;

    for (int i = 0; i < turnID; i++) {
        scanf("%d %d %d %d %d %d", &x0, &y0, &x1, &y1, &x2, &y2);
        if (x0 == -1) {
            board.player = BitEngine::Player::BLACK;
        } else {
            BitEngine::Move opponentMove = BitEngine::makeMove(
                BitEngine::XYToIndex(x0, y0), BitEngine::XYToIndex(x1, y1), BitEngine::XYToIndex(x2, y2));
            BitEngine::applyMove(board, opponentMove);
        }

        if (i < turnID - 1) {
            scanf("%d %d %d %d %d %d", &x0, &y0, &x1, &y1, &x2, &y2);
            if (x0 >= 0) {
                BitEngine::Move myMove = BitEngine::makeMove(BitEngine::XYToIndex(x0, y0), BitEngine::XYToIndex(x1, y1),
                                                             BitEngine::XYToIndex(x2, y2));
                BitEngine::applyMove(board, myMove);
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
