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
constexpr int ALLOW_FP_DEPTH = 3;               // 允许fp的深度阈值
constexpr int FP_MARGIN_BASE = 8000; /*1250;*/  // 每一步棋的富余基数，即每步最多能拿多少分
// N-Best截断
constexpr int ALLOW_N_BEST = 3;            // 允许N-Best的深度阈值
constexpr int N_BEST_DEPTH_1 = 1;          // N-Best的深度确值
constexpr int N_BEST_DEPTH_2 = 2;          // N-Best的深度确值
constexpr int N_BEST_RANK_1 = 15;          // 动态调整，阈值1
constexpr int N_BEST_RANK_2 = 30;          // 动态调整，阈值2
constexpr int N_BEST_RANK_3 = 45; /*60;*/  // 动态调整，阈值3
// LMR晚期移动缩减
constexpr int ALLOW_LMR_DEPTH = 3;        // 允许LMR的深度阈值
constexpr int ALLOW_LMR_RANK = 8; /*6;*/  // 允许LMR的走法排名
constexpr int LMR_DEPTH_DECAY = 1;        // LMR的深度衰减力度，基础值
constexpr int LMR_RANK_1 = 10;            // 动态调整，阈值1
constexpr int LMR_RANK_2 = 30;            // 动态调整，阈值2
// [ 弃用 ] 空步剪枝（亚马逊棋可能不适合空步）
// 允许深度设置为极大值彻底关闭空步
constexpr int ALLOW_NULLMOVE_DEPTH = 1000; /*3;*/  // 允许空步剪枝的深度阈值
constexpr int NULLMOVE_R = 3;                      // 空步子树深度衰减常量
// 渴望窗口
constexpr int ASPIRATION_DEPTH = 3;       // 开始层数
constexpr int ASPIRATION_WINDOW = 15000;  // 宽度，容忍度
// 超时标志
extern bool isTimeout_final;
// 最大深度
constexpr int MAX_DEPTH = 128;  // 内存池保证不再爆栈，但没必要再往上了
// 最大PLY
constexpr int MAX_PLY = 128;  // 理论上MAX_PLY不应该小于MAX_DEPTH，否则会导致内存池访问越界
// 超时检查间隔
constexpr int CHECK_GAP_MASK = (1 << 13) - 1;  // 取模掩码，毎8192回合检查一次超时，1024为最稳数据
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
                              BitEngine::Bitmap blocked,
                              BitEngine::Bitmap myAmazons,
                              BitEngine::Bitmap opAmazons);  // 残局特化

// Piece-Square Tables 子力位置表
// clang-format off
constexpr int PST[BitEngine::AMAZON_BOARD_SQUARE]={
    -20, -12,  -8,  -6,  -6,  -8, -12, -20,
    -12,  -6,   2,   5,   5,   2,  -6, -12,
     -8,   2,  12,  15,  15,  12,   2,  -8,
     -6,   5,  15,  10,  10,  15,   5,  -6,
     -6,   5,  15,  10,  10,  15,   5,  -6,
     -8,   2,  12,  15,  15,  12,   2,  -8,
    -12,  -6,   2,   5,   5,   2,  -6, -12,
    -20, -12,  -8,  -6,  -6,  -8, -12, -20,
};
// clang-format on

// 计算PST总分
inline int evaluatePST(BitEngine::Bitmap amazons) {
    int reScore = 0;
    while (amazons) {
        reScore += PST[BitEngine::fnlBit(amazons)];
        BitEngine::kicBit(amazons);
    }
    return reScore;
}

// 平滑插值函数（整数版）（无夹挤）
// 系数 t = phase / range
// 线性插值 y=x
inline int linearLerp(int a, int b, int phase, int scale) {
    return a + ((b - a) * phase) / scale;
}
// 缓入插值 y=x^2
inline int easeInLerp(int a, int b, int phase, int scale) {
    int easeInPhase = (phase * phase) / scale;
    return a + ((b - a) * easeInPhase) / scale;
}
// 缓出插值 y=1-(1-x)^x
inline int easeOutLerp(int a, int b, int phase, int scale) {
    int invPhase = scale - phase;
    int easeOutPhase = scale - ((invPhase * invPhase) / scale);
    return a + ((b - a) * easeOutPhase) / scale;
}

// 局面情况边界
constexpr int BEGINGAME_PIECES = 56;
constexpr int MIDDLEGAME_PIECES = 44;
constexpr int MID2ENDGAME_PIECES = 32;
constexpr int ENDGAME_PIECES = 20;
// constexpr int W_CHANGE_PIECES = 40;

// 局面进度标尺
constexpr int PHASE_SCALE = 1 << 8;  // 256好算，和RGB差不多
constexpr int PHASE_SPAN = BEGINGAME_PIECES - ENDGAME_PIECES;

// 加分系数
constexpr int ABSOLUTE_DOMAIN_FACTOR = 2000;              // 绝对领域
constexpr int MELEE_W_TER = ABSOLUTE_DOMAIN_FACTOR / 10;  // 混战_领地

constexpr int TRAPPED_TER_PIECES = 5;  // 皇后限位(领地)阈值
constexpr int TRAPPED_TER_BASE = 100;  // 皇后限位(领地)惩罚
constexpr int TRAPPED_TER_PENALTY[TRAPPED_TER_PIECES] = {
    (TRAPPED_TER_PIECES - 0) * (TRAPPED_TER_PIECES - 0) * TRAPPED_TER_BASE,  // 2500
    (TRAPPED_TER_PIECES - 1) * (TRAPPED_TER_PIECES - 1) * TRAPPED_TER_BASE,  // 1600
    (TRAPPED_TER_PIECES - 2) * (TRAPPED_TER_PIECES - 2) * TRAPPED_TER_BASE,  // 900
    (TRAPPED_TER_PIECES - 3) * (TRAPPED_TER_PIECES - 3) * TRAPPED_TER_BASE,  // 400
    (TRAPPED_TER_PIECES - 4) * (TRAPPED_TER_PIECES - 4) * TRAPPED_TER_BASE,  // 100
};

constexpr int TRAPPED_DIR_PIECES = 3;  // 皇后限位(方向)阈值
constexpr int TRAPPED_DIR_BASE = 400;  // 皇后限位(方向)惩罚
constexpr int TRAPPED_DIR_PENALTY[TRAPPED_DIR_PIECES] = {
    (TRAPPED_DIR_PIECES - 0) * (TRAPPED_DIR_PIECES - 0) * TRAPPED_DIR_BASE,  // 3600
    (TRAPPED_DIR_PIECES - 1) * (TRAPPED_DIR_PIECES - 1) * TRAPPED_DIR_BASE,  // 1600
    (TRAPPED_DIR_PIECES - 2) * (TRAPPED_DIR_PIECES - 2) * TRAPPED_DIR_BASE,  // 400
};

constexpr int W_REDUNDANT_PENALTY = 3000;  // 废皇后惩罚

constexpr int DIST_1_FACTOR = -600;  // dist 1 惩罚
constexpr int DIST_2_FACTOR = -300;  // dist 2 排斥
constexpr int DIST_3_FACTOR = 300;   // dist 3 奖励
// constexpr int DIST_4_FACTOR = 150;   // dist 4 吸引

constexpr int TER_L1 = 3, TER_L2 = 1;  // 分级领地

constexpr int W_MOB_A = 800, W_MOB_B = 300, K_FLX = 5;  // 机动性
constexpr int W_TER_A = 500, W_TER_B = 1000;            // 领地
constexpr int W_PST_A = 30, W_PST_B = 0;                // 位置
constexpr int W_SYN_A = 10, W_SYN_B = 0;                // 协同

}  // namespace VanitasBot::SearchEngine

#endif
