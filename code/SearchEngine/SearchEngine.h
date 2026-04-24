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
// IID 内部迭代加深
constexpr int ALLOW_IID_DEPTH = 4;  // 允许IID的深度阈值
constexpr int IID_DEPTH_DECAY = 2;  // IID的深度裁切力度
// Futility Pruning 边缘剪枝/静态空步剪枝
constexpr int ALLOW_FP_DEPTH = 3;    // 允许fp的深度阈值
constexpr int FP_MARGIN_BASE = 150;  // 每一步棋的富余基数，即每步最多能拿多少分
// N-Best截断
constexpr int ALLOW_N_BEST = 3;    // 允许N-Best的深度阈值
constexpr int N_BEST_DEPTH_1 = 1;  // N-Best的深度确值
constexpr int N_BEST_DEPTH_2 = 2;  // N-Best的深度确值
constexpr int N_BEST_RANK_1 = 15;  // 动态调整，阈值1
constexpr int N_BEST_RANK_2 = 30;  // 动态调整，阈值2
constexpr int N_BEST_RANK_3 = 60;  // 动态调整，阈值3
// LMR晚期移动缩减
constexpr int ALLOW_LMR_DEPTH = 3;  // 允许LMR的深度阈值
constexpr int ALLOW_LMR_RANK = 6;   // 允许LMR的走法排名
constexpr int LMR_DEPTH_DECAY = 1;  // LMR的深度衰减力度，基础值
constexpr int LMR_RANK_1 = 10;      // 动态调整，阈值1
constexpr int LMR_RANK_2 = 30;      // 动态调整，阈值2
// 空步剪枝
constexpr int ALLOW_NULLMOVE_DEPTH = 3;  // 允许空步剪枝的深度阈值
constexpr int NULLMOVE_R = 3;            // 空步子树深度衰减常量
// 渴望窗口
constexpr int ASPIRATION_DEPTH = 3;     // 开始层数
constexpr int ASPIRATION_WINDOW = 100;  // 宽度
// 超时标志
extern bool isTimeout_final;
// 最大深度
constexpr int MAX_DEPTH = 100;  // 内存池保证不再爆栈，但没必要再往上了
// 最大PLY
constexpr int MAX_PLY = 128;  // 理论上MAX_PLY不应该小于MAX_DEPTH，否则会导致内存池访问越界
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
// 启发数据
using MoveWeight = int;             // 走法权重
constexpr int INVAILD_WEIGHT = -1;  // 无效权重
constexpr int KILLER_NUM = 2;       // 杀手数量（不要轻易改动，有些代码写死了）
// [ 弃用 ]
// constexpr int TTABLE_WEIGHT = 1 << 30;  // 置换表权重
// constexpr int KTABLE_WEIGHT = 1 << 29;  // 杀手权重
// constexpr int HTABLE_WEIGHT = 1 << 28;  // 历史表权重

// [ 关闭对外暴漏 ]
// extern BitEngine::Move KTable[MAX_PLY][KILLER_NUM];  // 杀手表
// extern MoveWeight HTable[BitEngine::AMAZON_BOARD_SQUARE][BitEngine::AMAZON_BOARD_SQUARE]
//                         [BitEngine::AMAZON_BOARD_SQUARE];  // 历史表

// 搜索入口
BitEngine::Move search(BitEngine::BitBoard& board);

// PVS搜索
TTable::Score PVS(BitEngine::BitBoard& board,
                  HashEngine::Key currHash,
                  TTable::Depth depth,
                  TTable::Ply ply,
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

// // 全量衰减历史权重
// inline void decayHTable() {
//     // 行优先
//     constexpr int sq = BitEngine::AMAZON_BOARD_SQUARE;
//     for (int from = 0; from < sq; ++from) {
//         for (int to = 0; to < sq; ++to) {
//             for (int arrow = 0; arrow < sq; ++arrow) {
//                 // 减半
//                 HTable[from][to][arrow] = std::max(0, HTable[from][to][arrow] >> 1);  //
//                 max防止变负
//             }
//         }
//     }
// }

}  // namespace VanitasBot::SearchEngine

#endif
