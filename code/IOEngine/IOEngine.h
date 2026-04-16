#pragma once
#ifndef IO_ENGINE_H
#define IO_ENGINE_H

#include "BitEngine/BitEngine.h"

namespace VanitasBot::IOEngine {

// 初始化IO配置
void initIOEngine();

// 初始化棋盘
void initBoard(BitEngine::BitBoard& board);

// 将int[][]类型的棋盘转换为BitEngine::BitBoard类型
void readInputAndRecover(const int gridInfo[8][8], BitEngine::BitBoard& board);

// 输出答案
void OutputAnswer(BitEngine::Move chosenMove, int& startX, int& startY, int& resultX, int& resultY, int& obstacleX,
                  int& obstacleY);

}  // namespace VanitasBot::IOEngine

#endif
