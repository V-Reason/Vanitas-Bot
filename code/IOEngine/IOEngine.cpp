#include "IOEngine.h"

#include <cstdio>

namespace VanitasBot::IOEngine {

// 历史记录数组
static constexpr int MAX_HISTORY = 100000000;
static HistoryState historyStack[MAX_HISTORY];
static int historyTop = 0;

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
    // 直接初始化为白色，若为黑色readInputAndRecover会修改
    board.player = BitEngine::Player::WHITE;

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

// 读取输入并应用最新走法到棋盘，保存到历史记录
void readInputAndRecover(BitEngine::BitBoard& board) {
    static int lastTurnID = 0;
    int turnID;
    scanf("%d", &turnID);

    int x0, y0, x1, y1, x2, y2;

    for (int i = lastTurnID; i < turnID; i++) {
        scanf("%d %d %d %d %d %d", &x0, &y0, &x1, &y1, &x2, &y2);
        if (x0 == -1) {
            board.player = BitEngine::Player::BLACK;
        } else {
            BitEngine::Move opponentMove = BitEngine::makeMove(
                BitEngine::XYToIndex(x0, y0), BitEngine::XYToIndex(x1, y1), BitEngine::XYToIndex(x2, y2));
            pushHistory(board, opponentMove);
            BitEngine::applyMove(board, opponentMove);
        }

        if (i < turnID - 1) {
            scanf("%d %d %d %d %d %d", &x0, &y0, &x1, &y1, &x2, &y2);
            if (x0 >= 0) {
                BitEngine::Move myMove = BitEngine::makeMove(BitEngine::XYToIndex(x0, y0), BitEngine::XYToIndex(x1, y1),
                                                             BitEngine::XYToIndex(x2, y2));
                pushHistory(board, myMove);
                BitEngine::applyMove(board, myMove);
            }
        }
    }

    lastTurnID = turnID;
}

// 输出答案，直接输出Move的六个坐标
void OutputAnswer(BitEngine::Move chosenMove) {
    int fromX, fromY, toX, toY, arrowX, arrowY;

    BitEngine::indexToXY(BitEngine::getFrom(chosenMove), fromX, fromY);
    BitEngine::indexToXY(BitEngine::getTo(chosenMove), toX, toY);
    BitEngine::indexToXY(BitEngine::getArrow(chosenMove), arrowX, arrowY);

    printf("%d %d %d %d %d %d\n", fromX, fromY, toX, toY, arrowX, arrowY);
}

// 保存当前状态到历史栈
void pushHistory(const BitEngine::BitBoard& board, BitEngine::Move move) {
    if (historyTop < MAX_HISTORY) {
        historyStack[historyTop].board = board;
        historyStack[historyTop].move = move;
        historyTop++;
    }
}

// 从历史栈恢复状态
void popHistory(BitEngine::BitBoard& board) {
    if (historyTop > 0) {
        historyTop--;
        board = historyStack[historyTop].board;
    }
}

// 清空历史栈
void clearHistory() {
    historyTop = 0;
}

// 获取历史栈深度
int getHistoryDepth() {
    return historyTop;
}

}  // namespace VanitasBot::IOEngine
