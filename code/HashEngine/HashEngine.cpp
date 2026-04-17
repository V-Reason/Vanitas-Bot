#include "HashEngine/HashEngine.h"

#include "BitEngine/BitEngine.h"

#include <random>

namespace VanitasBot::HashEngine {

Key elementHash[static_cast<int>(Element::ELEMENT_COUNT)]
               [static_cast<int>(BitEngine::AMAZON_BOARD_LENGTH * BitEngine::AMAZON_BOARD_LENGTH)];
Key playerBlackKey;

void init() {
    // 全局随机种子（必须写死，不要使用Time来写，保证哈希稳定）
    std::mt19937_64 rng(0x192837465A6B7C8DULL);

    // 布满随机数
    for (auto ele : ALL_ELEMENTS) {
        for (auto sqr = 0; sqr < BitEngine::AMAZON_BOARD_SQUARE; ++sqr) {
            elementHash[static_cast<int>(ele)][static_cast<int>(sqr)] = rng();
        }
    }
    playerBlackKey = rng();
}

Key generateHash(const BitEngine::BitBoard& board) {
    // 设置初始数
    Key hash = 0;

    // 空白EMPTY不用管，已经删掉了

    // 箭矢ARROW
    BitEngine::Bitmap arrows = board.arrows;
    while (arrows) {
        BitEngine::Index sqr = BitEngine::fnlBit(arrows);
        hash ^= elementHash[static_cast<int>(Element::ARROW)][sqr];
        BitEngine::kicBit(arrows);
    }

    // 黑棋PLAYER_BLACK
    BitEngine::Bitmap blacks = board.blacks;
    while (blacks) {
        BitEngine::Index sqr = BitEngine::fnlBit(blacks);
        hash ^= elementHash[static_cast<int>(Element::PLAYER_BLACK)][sqr];
        BitEngine::kicBit(blacks);
    }

    // 白棋PLAYER_WHITE
    BitEngine::Bitmap whites = board.whites;
    while (whites) {
        BitEngine::Index sqr = BitEngine::fnlBit(whites);
        hash ^= elementHash[static_cast<int>(Element::PLAYER_WHITE)][sqr];
        BitEngine::kicBit(whites);
    }

    // 黑方额外异或
    if (board.player == BitEngine::Player::BLACK)
        hash ^= playerBlackKey;

    // 返回key值
    return hash;
}

}  // namespace VanitasBot::HashEngine
