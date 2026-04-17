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

// 搜索入口
BitEngine::Move search(BitEngine::BitBoard& board);

// PVS搜索
TTable::Score PVS(BitEngine::BitBoard& board, TTable::Depth depth, TTable::Score alpha, TTable::Score beta);

// 估值函数
TTable::Score evaluate(const BitEngine::BitBoard& board);
}  // namespace VanitasBot::SearchEngine

#endif
