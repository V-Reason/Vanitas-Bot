#pragma once
#ifndef TT_H
#define TT_H

#include "BitEngine/BitEngine.h"
#include "HashEngine/HashEngine.h"

#include <cstdint>
#include <cstring>

namespace VanitasBot::TTable {
using Score = int32_t;  // NegaMax必须可用正负数
using Depth = int8_t;   // 允许负数
using Ply = uint8_t;    // ply

// 分数上下限
// int16_t的取值范围是-32768~32767
// int32_t的取值范围是-2147483648~2147483647
constexpr Score SCORE_INFINITY = 1000000;            //  手动制定，防溢出
constexpr Score SCORE_MATE = SCORE_INFINITY - 1000;  //  用于杀棋的深度惩罚，比如MATA+-(depth * K)
constexpr Score SCORE_WIN = SCORE_MATE - 1000;       //  必胜解的最低分数

// 节点评估标志
enum class NodeFlag : uint8_t {
    EXACT,        // 精确值
    LOWER_BOUND,  // 下界（Fail-High触发）
    UPPER_BOUND,  // 上界（Fail-Low触发）
};

// 置换表数据 142bit -> 144bit 18B -> 24B
struct TTableData {
    HashEngine::Key hash;      // 64bit 局面哈希值
    BitEngine::Move bestMove;  // 32bit 局面最优解
    Score score;               // 32bit 局面评分
    Depth depth;               // 8bit  该节点所在搜索深度
    NodeFlag flag;             // 8bit  节点评估标记
};

// 配置数据
constexpr size_t MAX_TTABLE_SIZE = 1 << 22;          // 约400万条，96MB
constexpr size_t TTABLE_MASK = MAX_TTABLE_SIZE - 1;  // 用于取模运算

// 全局静态数组表
extern TTableData TTable[MAX_TTABLE_SIZE];  // 约400万条，64MB

// // 初始化放在.cpp里自动完成
// inline void init() {
//     std::memset(TTable, 0, sizeof(TTable));
// }

// 取下标
inline size_t getIndex(HashEngine::Key hash) {
    return hash & TTABLE_MASK;  // 取模
}

// 写入TTable
inline void write(const TTableData& tableData) {
    auto index = getIndex(tableData.hash);
    // 旧数据为空 || 新数据更深
    if (TTable[index].hash == 0 || tableData.depth >= TTable[index].depth) {
        // 全量覆盖
        TTable[index] = tableData;
    }
}

// 读取TTable
inline bool read(HashEngine::Key in_hash, TTableData& out_tableData) {
    auto index = getIndex(in_hash);
    if (TTable[index].hash == in_hash) {  // 先验，防止哈希碰撞
        out_tableData = TTable[index];
        return true;
    }
    return false;
}

}  // namespace VanitasBot::TTable

#endif
