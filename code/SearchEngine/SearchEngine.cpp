#define DEBUG

#include "SearchEngine.h"

#include <algorithm>
#include <cstdio>

namespace VanitasBot::SearchEngine {
BitEngine::Move KTable[MAX_DEPTH][KILLER_NUM]{};
MoveWeight HTable[BitEngine::AMAZON_BOARD_SQUARE][BitEngine::AMAZON_BOARD_SQUARE]
                 [BitEngine::AMAZON_BOARD_SQUARE]{};
bool isTimeout_final = false;

// 临时计时器
// auto startTime = std::chrono::steady_clock::now();
// bool isTimeout_final = false;
BitEngine::Move search(BitEngine::BitBoard& board) {
    // // 临时：每次进入主搜时，强行重置一次计时器起点
    // startTime = std::chrono::steady_clock::now();
    // isTimeout_final = false;

    // 配置计时器
    Utilities::Timer::timeoutConfigs[0].isTimeOut = &isTimeout_final;

    // 重置计时器
    Utilities::Timer::resetStartTime();

    // TODO: 开局库检查

    // 初始化哈希
    HashEngine::init();
    board.hash = HashEngine::generateHash(board);

    // IDS迭代加深搜索
    BitEngine::Move globalBestMove = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        TTable::Score score = PVS(board, depth, -TTable::SCORE_INFINITY, TTable::SCORE_INFINITY);

#ifdef DEBUG
        printf("depth: %d\n", depth);
        printf("PassedTime: %d\n", Utilities::Timer::getPassedTime());
#endif

        // TODO: isTimeout_final检测
        if (isTimeout_final)
            break;

        // 衰减HTable
        decayHTable();

        // 读取TTable置换表，更新globalBestMove
        TTable::TTableData ttData;
        if (TTable::read(board.hash, ttData))
            globalBestMove = ttData.bestMove;

        // 搜到了必胜解
        if (score >= TTable::SCORE_WIN)
            break;
    }

    return globalBestMove;
}

TTable::Score PVS(BitEngine::BitBoard& board,
                  TTable::Depth depth,
                  TTable::Score alpha,
                  TTable::Score beta) {
    // 毎若干搜索后检查一次超时
    static int nodesCnt = 0;
    if (!(++nodesCnt & CHECK_GAP_MASK) && Utilities::Timer::checkTimeouts())
        return 0;
    if (isTimeout_final)
        return 0;

    // TTable置换表剪枝
    TTable::TTableData ttData;
    BitEngine::Move ttMove = 0;  // 用于记录tt中的最优解，用于排序
    if (TTable::read(board.hash, ttData) && ttData.depth >= depth) {
        // 尝试剪枝
        if (ttData.flag == TTable::NodeFlag::EXACT)
            return ttData.score;
        if (ttData.flag == TTable::NodeFlag::LOWER_BOUND && ttData.score >= beta)
            return beta;
        if (ttData.flag == TTable::NodeFlag::UPPER_BOUND && ttData.score <= alpha)
            return alpha;
        // 剪枝失败，记录bestMove
        ttMove = ttData.bestMove;
    }

    // 到达深度限制
    if (depth == 0)
        return evaluate(board);

    // 生成走法
    BitEngine::MoveList moveList;
    BitEngine::generateAllMoves(board, moveList);

    // 终局判断
    if (moveList.count == 0) {
        return -TTable::SCORE_MATE + depth;  // +depth 争取死慢点/赢快点
    }

    // TODO: 走法排序
    // 权重定义
    // 1. 置换表
    // 2. 杀手表
    // 3. 历史表
    int moveWeight[BitEngine::MAX_AMAZON_MOVE_TYPE];
    for (int i = 0; i < moveList.count; ++i) {
        BitEngine::Move m = moveList.moves[i];
        if (m == ttMove)
            moveWeight[i] = TTABLE_WEIGHT;
        else if (m == KTable[depth][0] || m == KTable[depth][1])
            moveWeight[i] = KTABLE_WEIGHT;
        else {
            using namespace BitEngine;
            moveWeight[i] = HTable[getFrom(m)][getTo(m)][getArrow(m)];
        }
    }

    // [ 已弃用 ]
    // // 首节点，全窗口搜索
    // BitEngine::applyMove(board, bestMove);  // 借用bestMove代表moveList.moves[0]
    // bestScore = -PVS(board, depth - 1, -beta, -alpha);
    // BitEngine::resetMove(board, bestMove);
    // if (bestScore > alpha)
    //     alpha = bestScore;
    // if (!(alpha >= beta))  // beta剪枝失败，进入循环，从 i=1 开始

    // 搜索
    TTable::Score bestScore = -TTable::SCORE_INFINITY;
    BitEngine::Move bestMove = moveList.moves[0];
    TTable::Score ori_Alpha = alpha;

    for (int i = 0; i < moveList.count; ++i) {
        // 实时选择排序 -> 性质好，不需要全排一遍
        int maxInx = i;
        for (int j = i + 1; j < moveList.count; ++j) {
            if (moveWeight[j] > moveWeight[maxInx])
                maxInx = j;
        }
        std::swap(moveList.moves[maxInx], moveList.moves[i]);
        std::swap(moveWeight[maxInx], moveWeight[i]);

        // 取Move，即为权重最高的
        BitEngine::Move move = moveList.moves[i];

        // 落子（内部自动更新hash）
        BitEngine::applyMove(board, move);

        // 搜索
        TTable::Score score;
        if (i == 0)  // 首节点全窗口搜索
            score = -PVS(board, depth - 1, -beta, -alpha);
        else {  // 零窗口搜索
            score = -PVS(board, depth - 1, -alpha - 1, -alpha);
            if (alpha < score && score < beta)                  // 可以省一步更新alpha，使用-score
                score = -PVS(board, depth - 1, -beta, -score);  // 零窗口尝试失败，重新全窗口搜索
        }

        // 悔棋
        BitEngine::resetMove(board, move);

        // TODO:超时处理
        if (isTimeout_final)
            return 0;

        // 更新结果
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        // 收缩窗口
        if (score > alpha)
            alpha = score;
        // beta剪枝
        if (alpha >= beta) {
            // 更新杀手
            if (move != KTable[depth][0]) {  // 简易循环数组
                KTable[depth][1] = KTable[depth][0];
                KTable[depth][0] = move;
            }
            // 更新历史
            using namespace BitEngine;
            HTable[getFrom(move)][getTo(move)][getArrow(move)] += depth * depth;
            // 剪枝
            break;
        }
    }

    // 写入TTable置换表
    TTable::NodeFlag flag = TTable::NodeFlag::EXACT;
    if (bestScore <= ori_Alpha)
        flag = TTable::NodeFlag::UPPER_BOUND;  // Fail-Low
    else if (bestScore >= beta)
        flag = TTable::NodeFlag::LOWER_BOUND;
    TTable::write({board.hash, bestMove, bestScore, depth, flag});  // Fail-High

    // 返回最优解
    return bestScore;
}

TTable::Score evaluate(const BitEngine::BitBoard& board) {
    using namespace BitEngine;
    // 分辨敌我
    Bitmap myAmazons, opAmazons;
    if (board.player == Player::BLACK) {
        myAmazons = board.blacks;
        opAmazons = board.whites;
    } else {
        myAmazons = board.whites;
        opAmazons = board.blacks;
    }

    // 障碍与空地缓存
    Bitmap allBlocked = board.allBlocked();
    Bitmap empty = ~allBlocked;
    int emptyCnt = cntBit(empty);

    // 残局特化
    if (emptyCnt <= ENDGAME_PIECES)
        return evaluateEndGame(board, empty, myAmazons, opAmazons);

    // 普通局面

    // 机动性评估：
    // 汇总一步能到达的所有格子
    Bitmap myReach = 0;
    Bitmap me = myAmazons;
    while (me) {
        Index idx = fnlBit(me);
        myReach |= generateQueenMoves(makeMask(idx), allBlocked);
        kicBit(me);
    }
    Bitmap opReach = 0;
    Bitmap op = opAmazons;
    while (op) {
        Index idx = fnlBit(op);
        opReach |= generateQueenMoves(makeMask(idx), allBlocked);
        kicBit(op);
    }
    // 量化
    int myMobility = cntBit(myReach);
    int opMobility = cntBit(opReach);

    // 领地评估：
    // 汇总能领先到达的所有格子
    int myExclusive = cntBit(myReach & ~opReach);
    int opExclusive = cntBit(~myReach & opReach);

    // 中局特化
    // 中心区域加分
    int myCenter = 0;
    int opCenter = 0;
    if (emptyCnt > MIDDLEGAME_PIECES) {
        myCenter = cntBit(myReach & CENTER_MASK);
        opCenter = cntBit(opReach & CENTER_MASK);
    }

    // 动态权重
    // 机动性逐渐贬值，领地逐渐升值
    int w_mob, w_ter;
    if (emptyCnt > 40) {
        w_mob = W_MOB_A;
        w_ter = W_TER_A;
    } else {
        w_mob = W_MOB_B;
        w_ter = W_TER_B;
    }

    // 计算总分
    return w_mob * (myMobility - opMobility) + w_ter * (myExclusive - opExclusive)
           + CENTER_FACTOR * (myCenter - opCenter);
}

TTable::Score evaluateEndGame(const BitEngine::BitBoard& board,
                              BitEngine::Bitmap empty,
                              BitEngine::Bitmap myAmazons,
                              BitEngine::Bitmap opAmazons) {
    using namespace BitEngine;
    // 缓存
    TTable::Score score = 0;
    Bitmap visited = 0;
    Bitmap unvisited = empty;

    // 遍历所有空地，寻找独立连通区域
    while (unvisited) {
        Bitmap region = makeMask(fnlBit(unvisited));

        // 水漫法
        Bitmap lastRegion = 0;
        while (region != lastRegion) {
            lastRegion = region;
            region |= (getKingMoves(region) & empty);
        }

        // 更新访问标志
        setBit(visited, region);
        clsBit(unvisited, region);

        // 判定区域归属（边缘是否碰到棋子）
        Bitmap regionBorder = getKingMoves(region);
        bool myRegion = (regionBorder & myAmazons);
        bool opRegion = (regionBorder & opAmazons);

        // 结算
        int regionSize = cntBit(region);
        if (myRegion && !opRegion) {  // 我的绝对领域
            score += ABSOLUTE_DOMAIN_FACTOR * regionSize;
        } else if (!myRegion && opRegion) {  // 她的绝对领域
            score -= ABSOLUTE_DOMAIN_FACTOR * regionSize;
        } else if (myRegion && opRegion) {  // 混战
            // TODO：没想好怎么设计
            // 机动性给简单的分
            int myMob = cntBit(getKingMoves(myAmazons) & region);
            int opMob = cntBit(getKingMoves(opAmazons) & region);
            score += MELEE_FACTOR * (myMob - opMob);
        }
    }  // end while

    return score;
}

}  // namespace VanitasBot::SearchEngine
