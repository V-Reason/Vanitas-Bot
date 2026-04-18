#pragma once
#ifndef IO_ENGINE_H
#define IO_ENGINE_H

#include "BitEngine/BitEngine.h"

#include <iostream>

namespace VanitasBot::IOEngine {

inline void initIOEngine() {
    std::ios::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cin.tie(nullptr);
    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);
}

inline void initBoard(BitEngine::BitBoard& board) {
    board.blacks = 0x0000000000810024ULL;
    board.whites = 0x2400810000000000ULL;
    board.arrows = 0;
    board.player = BitEngine::Player::BLACK;

    // BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(2, 0)));
    // BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(5, 0)));
    // BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(0, 2)));
    // BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(7, 2)));

    // BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(0, 5)));
    // BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(7, 5)));
    // BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(2, 7)));
    // BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(5, 7)));

    // printf("blacks: 0x%016llxULL\n", (unsigned long long)board.blacks);
    // printf("whites: 0x%016llxULL\n", (unsigned long long)board.whites);
}

inline void readInputAndRecover(BitEngine::BitBoard& board) {
    int turnID;
    scanf("%d", &turnID);

    int x0, y0, x1, y1, x2, y2;
    int loopCnt = 2 * turnID - 1;

    for (int i = 0; i < loopCnt; ++i) {
        scanf("%d %d %d %d %d %d", &x0, &y0, &x1, &y1, &x2, &y2);
        if (x0 == -1) {
            continue;
        } else {
            BitEngine::Move move = BitEngine::makeMove(BitEngine::XYToIndex(x0, y0),
                                                       BitEngine::XYToIndex(x1, y1),
                                                       BitEngine::XYToIndex(x2, y2));
            // pushHistory(board, move);
            BitEngine::applyMove(board, move);
        }
    }
}

inline void outputAnswer(BitEngine::Move chosenMove) {
    int fromX, fromY, toX, toY, arrowX, arrowY;

    BitEngine::indexToXY(BitEngine::getFrom(chosenMove), fromX, fromY);
    BitEngine::indexToXY(BitEngine::getTo(chosenMove), toX, toY);
    BitEngine::indexToXY(BitEngine::getArrow(chosenMove), arrowX, arrowY);

    printf("%d %d %d %d %d %d\n", fromX, fromY, toX, toY, arrowX, arrowY);
}

// class IOEngine {
//    public:
//     IOEngine();

//     // 初始化棋盘
//     void initBoard(BitEngine::BitBoard& board);

//     // 读取输入并恢复状态
//     void readInputAndRecover(BitEngine::BitBoard& board);

//     // 输出答案
//     void outputAnswer(BitEngine::Move chosenMove);

//     // 根据回合数回溯历史
//     void rollbackToTurn(BitEngine::BitBoard& board, int turn);

//     // 获取历史栈深度
//     int getHistoryDepth() const;

//    private:
//     // 历史记录结构
//     struct HistoryState {
//         BitEngine::BitBoard board;
//         BitEngine::Move move;
//     };

//     // 保存当前状态到历史栈
//     void pushHistory(const BitEngine::BitBoard& board, BitEngine::Move move);

//     // 从历史栈恢复状态
//     void popHistory(BitEngine::BitBoard& board);

//     // 清空历史栈
//     void clearHistory();

//     // 历史记录数组
//     static constexpr int MAX_HISTORY = 10000;
//     HistoryState historyStack[MAX_HISTORY];
//     int historyTop;
// };

}  // namespace VanitasBot::IOEngine

#endif
