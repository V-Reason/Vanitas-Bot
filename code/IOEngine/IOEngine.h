#pragma once
#ifndef IO_ENGINE_H
#define IO_ENGINE_H

#include "BitEngine/BitEngine.h"

namespace VanitasBot::IOEngine {
// 初始化IO配置
void initIOEngine();

// 初始化棋盘
void initBoard(BitEngine::BitBoard& board);

// 历史状态恢复
void rollBackHistory(BitEngine::BitBoard& board);

// 输出答案
void OutputAnswer(BitEngine::Move chosenMove);
}  // namespace VanitasBot::IOEngine

#endif
