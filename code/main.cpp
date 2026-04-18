// 文件：test_movegen_correctness.cpp
#include "BitEngine/BitEngine.h"
#include "Utilities/Logger/Logger.h"

#include <cassert>
#include <cstdio>
#include <vector>

using namespace VanitasBot::Utilities;
using namespace VanitasBot::BitEngine;
using namespace VanitasBot::HashEngine;

// 简单参考实现：逐格射线步进
Bitmap ref_queen_moves(Bitmap from, Bitmap blocked) {
    Bitmap moves = 0;
    int idx = fnlBit(from);
    int x = idx % 8, y = idx / 8;
    const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    for (int dir = 0; dir < 8; ++dir) {
        int nx = x + dx[dir], ny = y + dy[dir];
        while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            Bitmap target = makeMask(XYToIndex(nx, ny));
            if (blocked & target)
                break;
            setBit(moves, target);
            nx += dx[dir];
            ny += dy[dir];
        }
    }
    return moves;
}

void test_queen_moves_random() {
    printf("Testing Queen moves vs reference (random positions)...\n");
    // 生成随机局面10000次
    for (int seed = 0; seed < 100000000; ++seed) {
        srand(seed);
        Bitmap from = makeMask(rand() % 64);
        Bitmap blocked = 0;
        for (int i = 0; i < 10; ++i) {
            setBit(blocked, makeMask(rand() % 64));
        }
        blocked &= ~from;  // 起点不能阻塞

        Bitmap slow = ref_queen_moves(from, blocked);
        Bitmap fast = generateQueenMoves(from, blocked);
        if (fast != slow) {
            using namespace VanitasBot::BitEngine;
            using namespace VanitasBot::Utilities;
            int x, y;
            indexToXY(fnlBit(from), x, y);
            printf("From: %d (%d,%d)\n", fnlBit(from), x, y);
            printf("blocked:");
            Logger::showBitmap(blocked);
            printf("slow:");
            Logger::showBitmap(slow);
            printf("fast:");
            Logger::showBitmap(fast);
            printf("-------------------------------------------------------------------\n");
            assert(false);
        }
    }
    printf("  PASS\n");
}

void test_ref_simple() {
    using namespace VanitasBot::BitEngine;
    Bitmap from = makeMask(XYToIndex(0, 0));  // 角落 A1
    Bitmap blocked = 0;
    Bitmap moves = ref_queen_moves(from, blocked);
    // 预期：同一行 (1,0)~(7,0)，同一列 (0,1)~(0,7)，对角线 (1,1)~(7,7)
    // 总共 7+7+7 = 21 个位
    assert(cntBit(moves) == 21);
    // 检查不应包含自身 (0,0)
    assert(!(moves & from));
    // 检查四个角是否在正确位置
    assert(moves & makeMask(XYToIndex(7, 0)));  // 最右列
    assert(moves & makeMask(XYToIndex(0, 7)));  // 最下行
    assert(moves & makeMask(XYToIndex(7, 7)));  // 右下角
    printf("Reference implementation simple test PASS.\n");
}

int main() {
    test_ref_simple();
    test_queen_moves_random();

    // Logger::showBitmap(static_cast<Bitmap>(MapMask::ALL));
    // Logger::showBitmap(static_cast<Bitmap>(MapMask::NONE));
    // Logger::showBitmap(static_cast<Bitmap>(MapMask::NO_LEFT));
    // Logger::showBitmap(static_cast<Bitmap>(MapMask::NO_RIGHT));

    return 0;
}
