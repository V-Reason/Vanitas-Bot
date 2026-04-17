#pragma once
#ifndef HASH_ENGINE_H
#define HASH_ENGINE_H

#include <cassert>
#include <cstdint>

// 向前声明
namespace VanitasBot::BitEngine {
using Index = uint32_t;
struct BitBoard;
}  // namespace VanitasBot::BitEngine

namespace VanitasBot::HashEngine {
using Key = uint64_t;

enum class Element : int {
    PLAYER_BLACK,  // 和BitEngine中保持一致！！！
    PLAYER_WHITE,
    // EMPTY,   // 省一次异或
    ARROW,
    ELEMENT_COUNT,  // 用于标记枚举数量
};

// constexpr Element ALL_ELEMENTS[] = {Element::EMPTY, Element::ARROW, Element::PLAYER_BLACK,
// Element::PLAYER_WHITE};  // 用于遍历，不要写ELEMENT_COUNT在里面！！！
constexpr Element ALL_ELEMENTS[] = {
    Element::PLAYER_BLACK,
    Element::PLAYER_WHITE,
    Element::ARROW,
};  // 用于遍历，不要写ELEMENT_COUNT在里面！！！

// 对外暴漏的密码本
constexpr int AMAZON_BOARD_SQUARE = 64;  //  没法使用BitEngine里的常量，这里手动定义
extern Key elementHash[static_cast<int>(Element::ELEMENT_COUNT)]
                      [static_cast<int>(AMAZON_BOARD_SQUARE)];
// 黑方回合需要额外异或的密匙
extern Key playerBlackKey;

// 初始化密码本（全局只调用一次！）
void init();

// 全量计算：针对当前BitBoard生成Hash值
Key generateHash(const BitEngine::BitBoard& board);

// 增量计算：针对一次Move更新Hash值
// 调用时机——先updataHash，再applyMove！！！
// 因为applyMove中会SwitchPlayer，updataHash的player参数会直接拿board里的参数强转
inline Key updataHash(Key ori_key,
                      Element player,
                      BitEngine::Index from,
                      BitEngine::Index to,
                      BitEngine::Index arrow) {
    // 调试
    assert((player == Element::PLAYER_BLACK) || (player == Element::PLAYER_WHITE));
    // 全部哈希特性都更新一次，次序不限
    ori_key ^= elementHash[static_cast<int>(player)][from];
    ori_key ^= elementHash[static_cast<int>(player)][to];
    // ori_key ^= elementHash[static_cast<int>(Element::EMPTY)][arrow];
    ori_key ^= elementHash[static_cast<int>(Element::ARROW)][arrow];
    ori_key ^= playerBlackKey;  // 要切换玩家了，所以也更新

    return ori_key;
}

}  // namespace VanitasBot::HashEngine

#endif
