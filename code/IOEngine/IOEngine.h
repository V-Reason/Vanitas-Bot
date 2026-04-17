#pragma once
#ifndef IO_ENGINE_H
#define IO_ENGINE_H

#include "BitEngine/BitEngine.h"

namespace VanitasBot::IOEngine {

class IOEngine {
   public:
    IOEngine();

    // 初始化棋盘
    void initBoard(BitEngine::BitBoard& board);

    // 读取输入并恢复状态
    void readInputAndRecover(BitEngine::BitBoard& board);

    // 输出答案
    void outputAnswer(BitEngine::Move chosenMove);

    // 根据回合数回溯历史
    void rollbackToTurn(BitEngine::BitBoard& board, int turn);

    // 获取历史栈深度
    int getHistoryDepth() const;

   private:
    // 历史记录结构
    struct HistoryState {
        BitEngine::BitBoard board;
        BitEngine::Move move;
    };

    // 保存当前状态到历史栈
    void pushHistory(const BitEngine::BitBoard& board, BitEngine::Move move);

    // 从历史栈恢复状态
    void popHistory(BitEngine::BitBoard& board);

    // 清空历史栈
    void clearHistory();

    // 历史记录数组
    static constexpr int MAX_HISTORY = 10000;
    HistoryState historyStack[MAX_HISTORY];
    int historyTop;
};

}  // namespace VanitasBot::IOEngine

#endif
