#pragma once

#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include "BitEngine/BitEngine.h"
#include "HashEngine/HashEngine.h"
#include "TTable/TTable.h"
#include "Utilities/Timer/Timer.h"

#include <algorithm>

// // 临时计时器
// #include <chrono>
// extern auto startTime = std::chrono::steady_clock::now();
// extern bool isTimeOut = false;
// constexpr int TIME_LIMIT_MS = 900;

// inline bool checkTimeout() {
//     auto now = std::chrono::steady_clock::now();
//     auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now -
//     startTime).count(); if (elapsed > TIME_LIMIT_MS) {
//         isTimeOut = true;
//         return true;
//     }
//     return false;
// }

namespace VanitasBot::SearchEngine {
// 空步剪枝
constexpr int ALLOW_NULLMOVE_DEPTH = 3;  // 允许空步剪枝的深度阈值
constexpr int NULLMOVE_R = 3;            // 空步子树深度衰减常量
// 渴望窗口
constexpr int ASPIRATION_DEPTH = 3;     // 开始层数
constexpr int ASPIRATION_WINDOW = 100;  // 宽度
// 超时标志
extern bool isTimeout_final;
// 最大深度
constexpr int MAX_DEPTH = 1000;
// 超时检查间隔
constexpr int CHECK_GAP_MASK = (1 << 13) - 1;  // 取模掩码，毎8192回合检查一次超时，1024为最稳数据
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
constexpr int MELEE_FACTOR = 10;              // 混战

constexpr int W_MOB_A = 6;  // 机动性
constexpr int W_MOB_B = 2;
constexpr int W_TER_A = 4;  // 领地
constexpr int W_TER_B = 8;
constexpr int LITE_FACTOR = (W_MOB_A + W_TER_B + W_TER_A + W_TER_B) / 1.5;  // Lite版本系数

// 内存监控频率
constexpr int MEM_MONITOR_EVALUATE_LITE_FREQ = 10000;  // evaluateLite 每多少次调用打印一次内存使用
constexpr int MEM_MONITOR_EVALUATE_FREQ = 5000;        // evaluate 每多少次调用打印一次内存使用
constexpr int MEM_MONITOR_EVALUATE_ENDGAME_FREQ
    = 2000;  // evaluateEndGame 每多少次调用打印一次内存使用

// 启发数据
using MoveWeight = int;             // 走法权重
constexpr int INVAILD_WEIGHT = -1;  // 无效权重
constexpr int KILLER_NUM = 2;       // 杀手数量（不要轻易改动，有些代码写死了）
// [ 弃用 ]
// constexpr int TTABLE_WEIGHT = 1 << 30;  // 置换表权重
// constexpr int KTABLE_WEIGHT = 1 << 29;  // 杀手权重
// constexpr int HTABLE_WEIGHT = 1 << 28;  // 历史表权重

extern BitEngine::Move KTable[MAX_DEPTH][KILLER_NUM];  // 杀手表
extern MoveWeight HTable[BitEngine::AMAZON_BOARD_SQUARE][BitEngine::AMAZON_BOARD_SQUARE]
                        [BitEngine::AMAZON_BOARD_SQUARE];  // 历史表

// 搜索入口
BitEngine::Move search(BitEngine::BitBoard& board);

// PVS搜索
TTable::Score PVS(BitEngine::BitBoard& board,
                  HashEngine::Key currHash,
                  TTable::Depth depth,
                  TTable::Score alpha,
                  TTable::Score beta,
                  bool allowNullMove);

// 估值函数
TTable::Score evaluateLite(const BitEngine::BitBoard& board);
TTable::Score evaluate(const BitEngine::BitBoard& board);
TTable::Score evaluateEndGame(const BitEngine::BitBoard& board,
                              BitEngine::Bitmap empty,
                              BitEngine::Bitmap myAmazons,
                              BitEngine::Bitmap opAmazons);  // 残局特化

// 全量衰减历史权重
inline void decayHTable() {
    // 行优先
    constexpr int sq = BitEngine::AMAZON_BOARD_SQUARE;
    for (int from = 0; from < sq; ++from) {
        for (int to = 0; to < sq; ++to) {
            for (int arrow = 0; arrow < sq; ++arrow) {
                // 减半
                HTable[from][to][arrow] = std::max(0, HTable[from][to][arrow] >> 1);  // max防止变负
            }
        }
    }
}

}  // namespace VanitasBot::SearchEngine

#endif
