#pragma once
#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "BitEngine/BitEngine.h"
#include "HashEngine/HashEngine.h"
#include "TTable/TTable.h"

namespace VanitasBot::SearchEngine {
// 配置数据
constexpr int MAX_DEPTH = 20;                // 最大搜索深度
constexpr int CHECK_GAP_MASK = 1 << 10 - 1;  // 取模掩码，毎1024回合检查一次超时
// 局面情况分类
constexpr int MIDDLEGAME_PIECES = 44;
constexpr int ENDGAME_PIECES = 16;
constexpr int W_CHANGE_PIECES = 40;
// 中心掩码
constexpr BitEngine::Bitmap CENTER_MASK
    = (1ULL << 27) | (1ULL << 28) | (1ULL << 35) | (1ULL << 36);  // 中心区域(3,3),(3,4),(4,3),(4,4)
// 加分系数
constexpr int CENTER_FACTOR = 20;             // 中心点
constexpr int ABSOLUTE_DOMAIN_FACTOR = 1000;  // 绝对领域

constexpr int W_MOB_A = 6;  // 机动性
constexpr int W_MOB_B = 2;
constexpr int W_TER_A = 4;  // 领地
constexpr int W_TER_B = 8;

// 搜索入口
BitEngine::Move search(BitEngine::BitBoard& board);

// PVS搜索
TTable::Score PVS(BitEngine::BitBoard& board,
                  TTable::Depth depth,
                  TTable::Score alpha,
                  TTable::Score beta);

// 估值函数
TTable::Score evaluate(const BitEngine::BitBoard& board);
TTable::Score evaluateEndGame(const BitEngine::BitBoard& board,
                              BitEngine::Bitmap empty,
                              BitEngine::Bitmap myAmazons,
                              BitEngine::Bitmap opAmazons);  // 残局特化

}  // namespace VanitasBot::SearchEngine

#endif
