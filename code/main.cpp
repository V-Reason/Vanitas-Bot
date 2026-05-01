#define DEBUG

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,popcnt,lzcnt")

#include "BitEngine/BitEngine.h"
#include "HashEngine/HashEngine.h"
#include "IOEngine/IOEngine.h"
#include "SearchEngine/SearchEngine.h"
#include "TTable/TTable.h"
#include "Utilities/Logger/Logger.h"

#include <cassert>
#include <iostream>

using namespace VanitasBot;
using namespace VanitasBot::Utilities;

#ifdef DEBUG
void switchToBoard(int which, BitEngine::BitBoard& board);
#endif

int main() {
#ifdef DEBUG
    assert(false);  // release模式检测
    freopen("test_cases/test_beg.txt", "r", stdin);
#endif
    Timer::resetStartTime();
    IOEngine::initIOEngine();

    BitEngine::BitBoard board;
    IOEngine::initBoard(board);
    IOEngine::readInputAndRecover(board);

#ifdef DEBUG
    switchToBoard(3, board);
#endif
#ifdef LOGGER_ON
    Logger::showBitboard(board);
#endif

    BitEngine::Move chosenMove;
    chosenMove = SearchEngine::search(board);

    IOEngine::outputAnswer(chosenMove);

    BitEngine::applyMove(board, chosenMove);
#ifdef LOGGER_ON
    Logger::showBitboard(board);
    printf("PassedTime: %d", Timer::getPassedTime());
#endif
    return 0;
}

#ifdef DEBUG
void switchToBoard(int which, BitEngine::BitBoard& board) {
    switch (which) {
        case 0:
            // 开局
            IOEngine::initBoard(board);
            break;
        case 1:
            // 均势开局
            board.blacks = 0x0000240000240000ULL;  // (2,2), (5,2), (2,5), (5,5) - 略微内聚
            board.whites = 0x8100000000000081ULL;  // (0,0), (7,0), (0,7), (7,7) - 极度分散
            board.arrows = 0x0000001818000000ULL;  // 中心 4 格有箭
            board.player = BitEngine::Player::BLACK;
            break;

        case 2:
            // 白方角落惨案 (Trapped Amazon)
            board.blacks = 0x00000000000000A5ULL;  // 黑方在底线散开
            board.whites = 0x8400110000000000ULL;  // 白方 H8 皇后被困
            board.arrows = 0x40C0000000000000ULL;  // 箭矢封死了 H8 的所有出路
            board.player = BitEngine::Player::BLACK;
            break;

        case 3:
            // 中局：双方各射出约12-14支箭
            board.blacks = 0x0400040000080400ULL;
            board.whites = 0x0020200000202000ULL;
            board.arrows = 0x8244910248240091ULL;
            board.player = BitEngine::Player::BLACK;
            break;

        case 4:
            // 中后局：第 30 步左右
            board.blacks = 0x0000000000040100ULL;  // 黑皇后
            board.whites = 0x0008100000000000ULL;  // 白皇后
            board.arrows = 0x0761284A1248021FULL;  // 散落的箭矢
            board.player = BitEngine::Player::BLACK;
            break;

        case 5:
            // 黑方空间压制
            board.blacks = 0x0000003C00000000ULL;  // 黑方四个皇后围住中心
            board.whites = 0x4200000000000042ULL;  // 白方皇后在死角 (1,0), (6,0), (1,7), (6,7)
            board.arrows = 0x1824420000422418ULL;  // 箭矢形成了一道弧形防线
            board.player = BitEngine::Player::BLACK;
            break;

        case 6:
            // 白方局部封锁成功
            board.blacks = 0x0101010100000000ULL;  // 黑方全被挤在 A 列
            board.whites = 0x0000000010101010ULL;  // 白方占据 E 列
            board.arrows = 0x0202020202020202ULL;  // B 列被箭封死，形成隔离墙
            board.player = BitEngine::Player::WHITE;
            break;

        case 7:
            // 中局向残局过渡（空格约 24 个）
            board.blacks = 0x0000000001000200ULL;
            board.whites = 0x0000008000400000ULL;
            board.arrows = 0xFEFDF87F3F1F0F07ULL;  // 大量空格被填满
            board.player = BitEngine::Player::WHITE;
            break;

        case 8:
            // 中心堡垒 vs 边缘包围 (Center Control & Sum Mobility)
            board.blacks = 0x0000001818000000ULL;  // 黑皇后抱团中心 (D4,E4,D5,E5)
            board.whites = 0x0000810000810000ULL;  // 白皇后散落两翼
            board.arrows = 0x4224004224004224ULL;  // 散落的障碍物切断了白方的横向联系
            board.player = BitEngine::Player::BLACK;
            break;

        case 9:
            // 对角线柏林墙 (Transition Scale Alignment)
            board.blacks = 0x0000000000000303ULL;  // 黑方在左下安全区
            board.whites = 0xC0C0000000000000ULL;  // 白方在右上狭窄区
            board.arrows = 0x3F0F0300C0F0FCFCULL;  // 一道贯穿棋盘的对角线箭墙
            board.player = BitEngine::Player::BLACK;
            break;

        case 10:
            // 被分割的牢笼 (Absolute Domain & Melee)
            board.blacks = 0x0000000000000107ULL;  // 黑方主力和混战代表
            board.whites = 0xE080000000000000ULL;  // 白方主力和混战代表
            board.arrows = 0x1F7FFF0808FFFEF8ULL;  // 将棋盘彻底焊死的物理隔离墙
            board.player = BitEngine::Player::BLACK;
            break;
        case 11:
            // 残局：空格剩余 12 个，棋盘高度破碎
            board.blacks = (1ULL << 0) | (1ULL << 8) | (1ULL << 55) | (1ULL << 63);
            board.whites = (1ULL << 27) | (1ULL << 28) | (1ULL << 35) | (1ULL << 36);
            board.arrows = 0x7E7EBDDBDBBD7E7EULL;
            board.player = BitEngine::Player::BLACK;
            break;

        case 12:
            // 黑方残局胜势
            board.blacks = 0x0000000000000003ULL;  // 黑后在 (0,0), (1,0)
            board.whites = 0x8000000000000000ULL;  // 白后在 (7,7)
            // 构造：左上角有一片 6 格的空地只属于黑方，白方被完全封死
            board.arrows = 0xFFFFFCFCF0F0F0F0ULL;
            board.player = BitEngine::Player::BLACK;
            break;
    }
}
#endif
