#pragma once
#ifndef IO_ENGINE_H
#define IO_ENGINE_H

#include "BitEngine/BitEngine.h"

namespace VanitasBot::IOEngine {

// 历史记录结构
struct HistoryState {
    BitEngine::BitBoard board;
    BitEngine::Move move;
};

// 初始化IO配置
void initIOEngine();

// 初始化棋盘
void initBoard(BitEngine::BitBoard& board);

// 读取输入并应用最新走法到棋盘，保存到历史记录
void readInputAndRecover(BitEngine::BitBoard& board);

// 输出答案，直接输出Move的六个坐标
void OutputAnswer(BitEngine::Move chosenMove);

// 保存当前状态到历史栈
void pushHistory(const BitEngine::BitBoard& board, BitEngine::Move move);

// 从历史栈恢复状态
void popHistory(BitEngine::BitBoard& board);

// 根据回合数回溯历史
void rollbackToTurn(BitEngine::BitBoard& board, int turn);

// 清空历史栈
void clearHistory();

// 获取历史栈深度
int getHistoryDepth();

}  // namespace VanitasBot::IOEngine

#endif
