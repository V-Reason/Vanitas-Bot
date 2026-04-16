#pragma once
#ifndef IO_ENGINE_H
#define IO_ENGINE_H

#include "BitEngine/BitEngine.h"

namespace VanitasBot::IOEngine {

// 初始化IO配置
void initIOEngine();

// 初始化棋盘
void initBoard(BitEngine::BitBoard& board);

// 读取输入并恢复棋盘状态
void readInputAndRecover(BitEngine::BitBoard& board);

// 输出答案
void OutputAnswer(BitEngine::Move chosenMove, int& startX, int& startY, int& resultX, int& resultY, int& obstacleX,
                  int& obstacleY);

}  // namespace VanitasBot::IOEngine

#endif
