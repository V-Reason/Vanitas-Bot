#pragma once
#ifndef BIT_ENGINE_H
#define BIT_ENGINE_H

#include <cstdint>
#include <vector>

namespace VanitasBot::BitEngine {
// 规范单次移动信息和地图信息
using Move = uint32_t;
using Bitmap = uint64_t;

// 玩家定义
constexpr int PLAYER_BLACK = 1;
constexpr int PLAYER_WHITE = -1;

// 边界掩码
enum class MapMask : Bitmap {
    ALL = 0xFFFFFFFFFFFFFFFFULL,
    NONE = 0x0000000000000000ULL,
    NO_LEFT = 0x7F7F7F7F7F7F7F7FULL,
    NO_RIGHT = 0xFEFEFEFEFEFEFEFEULL
};

// 方向枚举
enum class Dirtion : int {
    SW = -9,  // 西南
    S = -8,   // 南
    SE = -7,  // 东南
    W = -1,   // 西
    E = 1,    // 东
    NW = 7,   // 西北
    N = 8,    // 北
    NE = 9    // 东北
};
constexpr Dirtion ALL_DIRECTIONS[] = {
    Dirtion::SW, Dirtion::S, Dirtion::SE, Dirtion::W, Dirtion::E, Dirtion::NW, Dirtion::N, Dirtion::NE,
};  // 用于遍历

// 斜向枚举
enum class Offset : int {
    toRANK = 1,           // 用于水平
    toANTI_DIAGONAL = 7,  // 用于反对角线
    toFILE = 8,           // 用于竖直
    toDIAGONAL = 9        // 用于对角线
};

// 方向走步_宏函数
#define MOVE_TO_S(pos) (((pos) >> Offset::toFILE))
#define MOVE_TO_W(pos) (((pos) >> Offset::toRANK) & MapMask::NO_LEFT)
#define MOVE_TO_SW(pos) (((pos) >> Offset::toDIAGONAL) & MapMask::NO_LEFT)
#define MOVE_TO_SE(pos) (((pos) >> Offset::toANTI_DIAGONAL) & MapMask::NO_RIGHT)

#define MOVE_TO_N(pos) (((pos) << Offset::toFILE))
#define MOVE_TO_E(pos) (((pos) << Offset::toRANK) & MapMask::NO_RIGHT)
#define MOVE_TO_NE(pos) (((pos) << Offset::toDIAGONAL) & MapMask::NO_RIGHT)
#define MOVE_TO_NW(pos) (((pos) << Offset::toANTI_DIAGONAL) & MapMask::NO_LEFT)

// 全局信息
struct BitBoard {
    Bitmap blacks;
    Bitmap whites;
    Bitmap arrows;
    int player;

    inline Bitmap allPieces() const;
    inline Bitmap allBlocked() const;
};

// 静态方法
// 坐标系转换
inline int XYToBit(int x, int y);
inline void bitToXY(int bit, int& out_x, int& out_y);

// 根据偏移位生成mask信息
inline Bitmap makeMask(int offset_bit);

// 生成皇后八向标记
Bitmap generateQueenMoves(Bitmap from, Bitmap blocked);

// 根据局面生成Move信息
std::vector<Move> generateAllMoves(const BitBoard& board);

// 应用Move，相当于落子
void applyMove(BitBoard& board, Move move);

// 取消Move，相当于悔子
void resetMove(BitBoard& board, Move move);

}  // namespace VanitasBot::BitEngine

#endif
