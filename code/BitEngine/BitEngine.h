#pragma once
#ifndef BIT_ENGINE_H
#define BIT_ENGINE_H

// 弃用<bit>，Botzone环境最高C++17，无法使用
// #include <bit>

#include <cstdint>
#include <vector>

// 兼容MSVC和GCC
#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)
#endif

namespace VanitasBot::BitEngine {
// 规范信息类型
using Index = uint32_t;
using Move = uint32_t;
using Bitmap = uint64_t;

// 已知信息
constexpr int AMAZON_BOARD_LENGTH = 8;            // 棋盘长度
constexpr int MAX_AMAZON_MOVE_TYPE = 5112 + 388;  // 5112为理论极限值，388为设计冗余量
// constexpr Bitmap BEGIN_BLACKS_POSITION =
//     makeMask(XYToBit(2,0))
//     | makeMask(XYToBit(5,0))
//     | makeMask(XYToBit(0,2))
//     | makeMask(XYToBit(7,2));
// constexpr Bitmap BEGIN_WHITES_POSITION =
//     makeMask(XYToBit(0,5))
//     | makeMask(XYToBit(7,5))
//     | makeMask(XYToBit(2,7))
//     | makeMask(XYToBit(5,7));

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
#define MOVE_TO_N(pos) (((pos) << static_cast<int>(Offset::toFILE)))
#define MOVE_TO_E(pos) (((pos) << static_cast<int>(Offset::toRANK)) & static_cast<Bitmap>(MapMask::NO_RIGHT))
#define MOVE_TO_NE(pos) (((pos) << static_cast<int>(Offset::toDIAGONAL)) & static_cast<Bitmap>(MapMask::NO_RIGHT))
#define MOVE_TO_NW(pos) (((pos) << static_cast<int>(Offset::toANTI_DIAGONAL)) & static_cast<Bitmap>(MapMask::NO_LEFT))

#define MOVE_TO_S(pos) (((pos) >> static_cast<int>(Offset::toFILE)))
#define MOVE_TO_W(pos) (((pos) >> static_cast<int>(Offset::toRANK)) & static_cast<Bitmap>(MapMask::NO_LEFT))
#define MOVE_TO_SW(pos) (((pos) >> static_cast<int>(Offset::toDIAGONAL)) & static_cast<Bitmap>(MapMask::NO_LEFT))
#define MOVE_TO_SE(pos) (((pos) >> static_cast<int>(Offset::toANTI_DIAGONAL)) & static_cast<Bitmap>(MapMask::NO_RIGHT))

// bit元操作
inline void setBit(Bitmap& bitmap, Bitmap mask) {  // 设置bit
    bitmap |= mask;
}
inline void clsBit(Bitmap& bitmap, Bitmap mask) {  // 清理bit
    bitmap &= ~mask;
}
inline bool chkBit(Bitmap& bitmap, Bitmap mask) {  // 检测bit
    return bitmap & mask;
}
inline Index fnlBit(Bitmap mask) {  // 取最低位 1 的Index
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, mask);
    return static_cast<Index>(index);
#else
    return static_cast<Index>(__builtin_ctzll(mask));
#endif
}
inline void kicBit(Bitmap& bitmap) {  // 剔除最后一位bit
    bitmap &= (bitmap - 1);
}

// 玩家枚举
enum class Player {
    BLACK = -1,
    WHITE = 1,
};

// 全局信息
struct BitBoard {
    Bitmap blacks;  // 黑方棋子
    Bitmap whites;  // 白方棋子
    Bitmap arrows;  // 箭矢
    Player player;  // 玩家颜色

    inline Bitmap allPieces() const {
        return whites | blacks;
    }
    inline Bitmap allBlocked() const {
        return allPieces() | arrows;
    }
};

// 静态方法
// 坐标系转换
inline Index XYToIndex(int x, int y) {
    return y * AMAZON_BOARD_LENGTH + x;
}
inline void indexToXY(Index index, int& out_x, int& out_y) {
    out_x = index % AMAZON_BOARD_LENGTH;
    out_y = index / AMAZON_BOARD_LENGTH;
}

// 根据偏移位生成mask信息
inline Bitmap makeMask(Index index) {
    return 1ULL << index;
}

// 以int信息生成Move
inline Move makeMove(Index from, Index to, Index arrow) {
    return (Move)from | (Move)to << 7 | (Move)arrow << 14;
}
// 从Move取出int信息
inline Index getFrom(Move m) {
    return m & 0x7F;
}
inline Index getTo(Move m) {
    return (m >> 7) & 0x7F;
}
inline Index getArrow(Move m) {
    return (m >> 14) & 0x7F;
}

// 生成皇后八向标记
Bitmap generateQueenMoves(Bitmap from, Bitmap blocked);

// 根据局面生成Move信息
std::vector<Move> generateAllMoves(const BitBoard& board);

// 应用Move，相当于落子
void applyMove(BitBoard& board, Move move);

// 取消Move，相当于悔子
void resetMove(BitBoard& board, Move move);

// 切换玩家
inline void SwitchPlayer(BitBoard& board) {
    board.player = static_cast<Player>(-1 * static_cast<int>(board.player));
}
// 添加障碍
inline void addArrow(BitBoard& board, Bitmap arrow) {
    setBit(board.arrows, arrow);
}
// 删除障碍
inline void delArrow(BitBoard& board, Bitmap arrow) {
    clsBit(board.arrows, arrow);
}
}  // namespace VanitasBot::BitEngine

#endif
