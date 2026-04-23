// #define MONITOR
// #define MONITOR_LITE

#include "SearchEngine.h"

#ifdef _WIN32
#include <psapi.h>
#include <windows.h>

#else
#include <sys/resource.h>
#endif

#include <algorithm>
#include <cstdio>
#include <iostream>

// // 临时测试探针
// uint64_t stat_nodes_evaluated = 0;
// uint64_t stat_tt_hits = 0;
#ifdef MONITOR

#ifndef MONITOR_LITE
#define MONITOR_LITE
#endif

struct SearchStats {
    uint64_t evals;      // 调用 evaluate 的次数
    uint64_t evalsLite;  // 调用 evaluateLite 的次数
    uint64_t evalsEnd;   // 调用 evaluateEnd 的次数

    uint64_t ttHits;     // TT 成功剪枝次数
    uint64_t nmpTrials;  // 尝试 NMP 的次数
    uint64_t nmpCuts;    // NMP 成功剪枝次数

    uint64_t betaCutoffs;     // 总 Beta 剪枝次数
    uint64_t ttCutoffs;       // 置换表走法引发的剪枝
    uint64_t killer1Cutoffs;  // 杀手1引发的剪枝
    uint64_t killer2Cutoffs;  // 杀手2引发的剪枝
    uint64_t historyCutoffs;  // 历史表走法（普通走法）引发的剪枝

    uint64_t historyRankSum;  // 历史走法剪枝时，它排在第几个？（用于算平均排名）

    uint64_t lmrTrials;  // 尝试 LMR 的次数（满足条件并进入缩减搜索）
    uint64_t lmrCuts;    // LMR 成功剪枝次数（score <= alpha，跳过全量搜索）
    uint64_t lmrMisses;  // LMR 误判次数（score > alpha，仍需全量搜索）
    // 可选：记录 lmr 在各深度区间的触发分布
    // uint64_t lmrByDepth[16];

    uint64_t nBestCuts;  // 实际因超出走法限制而截断的次数

    uint64_t fpTrials;  // 试探fp次数
    uint64_t fpCuts;    // fp剪枝次数

    uint64_t mdpTrials;  // 尝试 MDP 检查的次数
    uint64_t mdpCuts;    // 因 MDP 导致窗口崩溃直接返回的次数

    uint64_t iidTrials;  // 尝试 IID 的次数
    uint64_t iidHits;    // IID 成功找到 ttMove 的次数
} stats;
#endif
#ifdef MONITOR_LITE
struct SearchStatsLite {
    uint64_t nodes;     // 遍历的总节点
    uint64_t maxDepth;  // 实际达到的深度
} statsLite;
#endif

namespace VanitasBot::SearchEngine {
BitEngine::Move KTable[MAX_PLY][KILLER_NUM]{};
MoveWeight HTable[BitEngine::AMAZON_BOARD_SQUARE][BitEngine::AMAZON_BOARD_SQUARE]
                 [BitEngine::AMAZON_BOARD_SQUARE]{};
bool isTimeout_final = false;

// 临时计时器
// auto startTime = std::chrono::steady_clock::now();
// bool isTimeout_final = false;

#pragma region 状态机
// 状态
enum class State : int {
    TT_MOVE,     // 置换表走法
    GEN_ALL,     // 全量生成走法
    KILLER_CHK,  // 杀手合法检查
    KILLER_1,    // 杀手1
    KILLER_2,    // 杀手2
    CAL_WEIGHT,  // 为剩余走法算权重
    SORT,        // 走法排序
    NORMAL,      // 逐取最高分
    NONE         // 状态耗尽
};

// 状态机（实现延迟计算）
// 注意：单个状态机约 45KB
class StateMachine {
   private:
    const BitEngine::BitBoard& board;
    BitEngine::Move ttMove;
    BitEngine::Move killer1;
    BitEngine::Move killer2;

    State currState = State::TT_MOVE;

    BitEngine::MoveList moveList;
    int moveWeight[BitEngine::MAX_AMAZON_MOVE_TYPE];  // 不要再这里写{}防止频繁内存清零
    // int indices[BitEngine::MAX_AMAZON_MOVE_TYPE];  // 索引数组，解决排序问题，22kb，容易爆栈
    int currIndex = 0;

    // TODO: 判断走法是否合法
    bool isValid(BitEngine::Move move) {
        for (int i = 0; i < moveList.count; ++i) {
            if (moveList.moves[i] == move)
                return true;
        }
        return false;
    }

    bool isKiller1OK = false;
    bool isKiller2OK = false;

    inline void toNextState() {
        // if(currState != State::NONE)更安全一点，不过State::NONE里不写toNextState()就好了
        currState = static_cast<State>(static_cast<int>(currState) + 1);
    }

   public:
    // 必须是轻量构造！！！
    StateMachine(const BitEngine::BitBoard& board,
                 BitEngine::Move ttMove,
                 BitEngine::Move killer1,
                 BitEngine::Move killer2)
        : board(board), ttMove(ttMove), killer1(killer1), killer2(killer2) {}

    bool hasNext() {
        return currState != State::NONE;
    }

    BitEngine::Move nextMove() {
        // 使用fallthrough贯通技巧，自动进入下一个状态
        switch (currState) {
            case State::TT_MOVE:
                toNextState();
                // 无条件信任置换表的合法性
                if (ttMove != 0)
                    return ttMove;
                [[fallthrough]];

            case State::GEN_ALL:
                toNextState();
                BitEngine::generateAllMoves(board, moveList);
                [[fallthrough]];

            case State::KILLER_CHK:
                toNextState();
                if ((killer1 != 0 && killer1 != ttMove) || (killer2 != 0 && killer2 != ttMove)) {
                    for (int i = 0; i < moveList.count; ++i) {
                        if (!isKiller1OK) {
                            isKiller1OK = (moveList.moves[i] == killer1);
                        }
                        if (!isKiller2OK) {
                            isKiller2OK = (moveList.moves[i] == killer2);
                        }
                    }
                }
                [[fallthrough]];

            case State::KILLER_1:
                toNextState();
                if (isKiller1OK)
                    return killer1;
                // if (killer1 != 0 && killer1 != ttMove && isValid(killer1)) {
                //     return killer1;
                // }
                [[fallthrough]];

            case State::KILLER_2:
                toNextState();
                if (isKiller2OK)
                    return killer2;
                // if (killer2 != 0 && killer2 != ttMove && killer1 != killer2 && isValid(killer2))
                // {
                //     return killer2;
                // }
                [[fallthrough]];

            case State::CAL_WEIGHT:
                toNextState();
                for (int i = 0; i < moveList.count; ++i) {
                    BitEngine::Move move = moveList.moves[i];
                    if (move == ttMove || move == killer1 || move == killer2) {
                        // 已经验证过了，过滤
                        moveWeight[i] = INVAILD_WEIGHT;
                    } else {
                        // 使用历史表
                        using namespace BitEngine;
                        moveWeight[i] = HTable[getFrom(move)][getTo(move)][getArrow(move)];
                    }
                }
                [[fallthrough]];

            case State::SORT:
                toNextState();
                [[fallthrough]];

            case State::NORMAL: {
                if (currIndex >= moveList.count) {
                    toNextState();
                    return nextMove();
                }

                // 实时选择排序
                int maxIdx = currIndex;
                for (int j = currIndex + 1; j < moveList.count; ++j) {
                    if (moveWeight[j] > moveWeight[maxIdx]) {
                        maxIdx = j;
                    }
                }

                std::swap(moveList.moves[currIndex], moveList.moves[maxIdx]);
                std::swap(moveWeight[currIndex], moveWeight[maxIdx]);

                if (moveWeight[maxIdx] == INVAILD_WEIGHT) {
                    currIndex++;
                    return nextMove();
                }

                return moveList.moves[currIndex++];
            }

                // [备用]
                // case State::SORT:
                //     toNextState();
                //     // 排序
                //     // 两种方案：选择排序、快速排序
                //     // 选择排序的性质好，随用随取，不用一次性全排，易处理平行数组
                //     // 映射数组，“偶尔？”会有运气差的情况
                //     // 快速排序稳定但复杂
                //     // 以下使用 快速排序+索引数组 解决平行数组排序问题
                //     for (int i = 0; i < moveList.count; ++i) {
                //         indices[i] = i;
                //     }
                //     std::sort(indices, indices + moveList.count, [&](int idxA, int idxB) {
                //         return moveWeight[idxA] > moveWeight[idxB];  // 降序
                //     });
                //     [[fallthrough]];

                // case State::NORMAL: {
                //     if (currIndex >= moveList.count) {
                //         toNextState();
                //         return nextMove();  //
                //         //
                //         抽取结束，不过为了接口规范，可以再递归一层，避免未来扩展，现在是会返回0
                //     }
                //     int realIdx = indices[currIndex++];
                //     if (moveWeight[realIdx] == INVAILD_WEIGHT) {
                //         return nextMove();
                //     }
                //     return moveList.moves[realIdx];
                // }

            case State::NONE:
                return 0;

        }  // end switch
        return 0;
    }  // end func
};  // end class
#pragma endregion

BitEngine::Move search(BitEngine::BitBoard& board) {
    // // 临时：每次进入主搜时，强行重置一次计时器起点
    // startTime = std::chrono::steady_clock::now();
    // isTimeout_final = false;

    // 配置计时器
    Utilities::Timer::timeoutConfigs[0].isTimeOut = &isTimeout_final;

    // 重置计时器
    // Utilities::Timer::resetStartTime();

    // TODO: 开局库检查

    // 初始化哈希
    HashEngine::init();
    HashEngine::Key initHash = HashEngine::generateHash(board);

    // IDS迭代加深搜索
    BitEngine::Move globalBestMove = 0;
    TTable::Score alpha = -TTable::SCORE_INFINITY;
    TTable::Score beta = TTable::SCORE_INFINITY;
    TTable::Score lastScore = 0;
    int depth = 1;
    for (/*int depth = 1*/; depth <= MAX_DEPTH; ++depth) {
        if (depth >= ASPIRATION_DEPTH) {
            // 设置渴望窗口
            alpha = std::max(lastScore - ASPIRATION_WINDOW,
                             static_cast<int>(-TTable::SCORE_INFINITY));
            beta = std::min(lastScore + ASPIRATION_WINDOW,
                            static_cast<int>(+TTable::SCORE_INFINITY));
        }

        TTable::Score score;
        while (true) {  // 最多可能搜索两次
            // 搜索
            score = PVS(board, initHash, depth, 0, alpha, beta, true);
            // 下跌
            if (score <= alpha && alpha > -TTable::SCORE_INFINITY) {  // 防止死循环
                alpha = -TTable::SCORE_INFINITY;                      // 开放下界
                continue;
            }
            // 上溢
            else if (score >= beta && beta < TTable::SCORE_INFINITY) {
                beta = TTable::SCORE_INFINITY;  // 开放上界
                continue;
            }
            // 符合预期
            lastScore = score;
            break;
        }

        // 超时，放弃本层结果
        if (isTimeout_final)
            break;

        // [ 或许1s中的搜索可以不用衰减?有点浪费资源了 ]
        // 衰减HTable
        // decayHTable();

        // 读取TTable置换表，更新globalBestMove
        TTable::TTableData ttData;
        if (TTable::read(initHash, ttData))
            globalBestMove = ttData.bestMove;

        // 搜到了必胜解
        if (score >= TTable::SCORE_WIN)
            break;
    }

// // 调试
// printf("搜索报告：\n");
// printf("最大深度: %d\n", depth);
// printf("评估节点: %llu\n", stat_nodes_evaluated);
// printf("TT命中: %llu\n", stat_tt_hits);
// 监测探针
#ifdef MONITOR_LITE
    statsLite.maxDepth = depth;

    printf("搜索报告:\n");
    printf("最大深度:\t %llu\n", statsLite.maxDepth);
    printf("遍历总节点:\t %llu\n", statsLite.nodes);
#endif

#ifdef MONITOR
    // 1. 基础数据计算
    uint64_t totalEvals = stats.evals + stats.evalsLite + stats.evalsEnd;

    // 【修正1】彻底废弃 interiorNodes，统一使用 statsLite.nodes 作为总基数
    uint64_t baseNodes = (statsLite.nodes > 0) ? statsLite.nodes : 1;  // 防除零保护

    printf("评估次数:\t %llu\n", totalEvals);
    printf("   全量eval\t %llu\n", stats.evals);
    printf("   轻量lite\t %llu\n", stats.evalsLite);
    printf("   残局end\t %llu\n", stats.evalsEnd);
    printf("置换表成功:\t %llu\n", stats.ttHits);
    printf("尝试空步:\t %llu\n", stats.nmpTrials);
    printf("空步剪枝:\t %llu\n", stats.nmpCuts);
    printf("总beta剪枝:\t %llu\n", stats.betaCutoffs);
    printf("   置换表:\t %llu\n", stats.ttCutoffs);
    printf("   杀手1:\t %llu\n", stats.killer1Cutoffs);
    printf("   杀手2:\t %llu\n", stats.killer2Cutoffs);
    printf("   历史表:\t %llu\n", stats.historyCutoffs);

    // 安全计算历史表平均排名 (防止除以 0)
    double avgHtRank
        = (stats.historyCutoffs > 0) ? ((double)stats.historyRankSum / stats.historyCutoffs) : 0.0;
    printf("   历史表平均排名: %.2f\n", avgHtRank);

    printf("占比指标: \n");

    // 【修正2】Beta剪枝率：在所有访问过的节点中，有多少比例触发了 Beta 剪枝
    double betaRate = ((double)stats.betaCutoffs * 100.0 / baseNodes);
    printf("   Beta剪枝率\t %.3f%%\n", betaRate);

    // 排序贡献度 (谁促成了剪枝，分母为总剪枝数)
    if (stats.betaCutoffs > 0) {
        printf("   ├─ tt\t %.3f%%\n", ((double)stats.ttCutoffs * 100.0 / stats.betaCutoffs));
        printf("   ├─ killer1\t %.3f%%\n",
               ((double)stats.killer1Cutoffs * 100.0 / stats.betaCutoffs));
        printf("   ├─ killer2\t %.3f%%\n",
               ((double)stats.killer2Cutoffs * 100.0 / stats.betaCutoffs));
        printf("   └─ ht\t %.3f%%\n", ((double)stats.historyCutoffs * 100.0 / stats.betaCutoffs));
    } else {
        printf("   ├─ tt\t 0.000%%\n");
        printf("   ├─ killer1\t 0.000%%\n");
        printf("   ├─ killer2\t 0.000%%\n");
        printf("   └─ ht\t 0.000%%\n");
    }

    // NMP (空步剪枝) 成功率
    double nmpRate
        = (stats.nmpTrials > 0) ? ((double)stats.nmpCuts * 100.0 / stats.nmpTrials) : 0.0;
    printf("   空步剪枝率\t %.3f%%\n", nmpRate);

    // LMR报告
    // ========== LMR 指标 ==========
    printf("LMR 探针报告:\n");
    if (stats.lmrTrials > 0) {
        double lmrSuccessRate = (double)stats.lmrCuts * 100.0 / stats.lmrTrials;
        double lmrMissRate = (double)stats.lmrMisses * 100.0 / stats.lmrTrials;

        printf("   尝试次数:\t\t %llu\n", stats.lmrTrials);
        printf("   成功剪枝:\t\t %llu (%.2f%%)\n", stats.lmrCuts, lmrSuccessRate);
        printf("   误判:\t\t %llu (%.2f%%)\n", stats.lmrMisses, lmrMissRate);

        if (stats.betaCutoffs > 0) {
            double lmrContribution = (double)stats.lmrCuts * 100.0 / stats.betaCutoffs;
            printf("   LMR 贡献剪枝占比:\t %.3f%%\n", lmrContribution);
        }
    } else {
        printf("   未触发 LMR\n");
    }

    // N-Best报告
    // ========== N-Best 指标 ==========
    printf("N-Best 探针报告:\n");
    // 【修正3】利用基础节点基数计算占比
    double nBestCutRate = (double)stats.nBestCuts * 100.0 / baseNodes;
    printf("   截断次数:\t\t %llu \n", stats.nBestCuts);
    printf("   占总节点比例:\t %.3f%%\n", nBestCutRate);

    // FP报告
    // ========== FP 指标 ==========
    printf("FP 探针报告:\n");
    if (stats.fpTrials > 0) {
        double fpSuccessRate = (double)stats.fpCuts * 100.0 / stats.fpTrials;
        // 【修正4】利用基础节点基数计算占比
        double fpCutRate = (double)stats.fpCuts * 100.0 / baseNodes;
        printf("   触发节点数:\t\t %llu\n", stats.fpTrials);
        printf("   截断次数:\t\t %llu (%.2f%%)\n", stats.fpCuts, fpSuccessRate);
        printf("   占总节点比例:\t %.3f%%\n", fpCutRate);
    } else {
        printf("   未触发 FP 剪枝\n");
    }

    // MDP报告
    printf("MDP 探针报告:\n");
    if (stats.mdpTrials > 0) {
        printf("   触发检查数:\t %llu\n", stats.mdpTrials);
        printf("   数学截断数:\t %llu\n", stats.mdpCuts);
    }

    // IID报告
    printf("IID 探针报告:\n");
    if (stats.iidTrials > 0) {
        printf("   IID 尝试数:\t %llu\n", stats.iidTrials);
        printf("   引导成功数:\t %llu (%.2f%%)\n",
               stats.iidHits,
               (double)stats.iidHits * 100.0 / stats.iidTrials);
    }
#endif

    return globalBestMove;
}

TTable::Score PVS(BitEngine::BitBoard& board,
                  HashEngine::Key currHash,
                  TTable::Depth depth,
                  TTable::Ply ply,
                  TTable::Score alpha,
                  TTable::Score beta,
                  bool allowNullMove) {
    // 监测探针
#ifdef MONITOR_LITE
    ++statsLite.nodes;
#endif

    // 毎若干搜索后检查一次超时
    static int nodesCnt = 0;
    if (!(++nodesCnt & CHECK_GAP_MASK) && Utilities::Timer::checkTimeouts())
        return 0;
    if (isTimeout_final)
        return 0;

    // 到达深度限制，自动负数保护
    if (depth <= 0)
        return evaluate(board);

// Mate Distance Pruning 杀棋距离剪枝
// 是否不可能找到比直接将死对手更好的分数了，残局用
#ifdef MONITOR
    ++stats.mdpTrials;
#endif
    TTable::Score mateScore = TTable::SCORE_MATE - ply;
    if (alpha < -mateScore)
        alpha = -mateScore;
    if (beta > mateScore - 1)
        beta = mateScore - 1;
    if (alpha >= beta) {
#ifdef MONITOR
        ++stats.mdpCuts;
#endif
        return alpha;
    }

    // TTable置换表剪枝
    TTable::TTableData ttData;
    BitEngine::Move ttMove = 0;  // 用于记录tt中的最优解，用于排序
    if (TTable::read(currHash, ttData) && ttData.depth >= depth) {
        // // 调试
        // stat_tt_hits++;
        // 监测探针
#ifdef MONITOR
        ++stats.ttHits;
#endif
        // 尝试剪枝
        if (ttData.flag == TTable::NodeFlag::EXACT)
            return ttData.score;
        if (ttData.flag == TTable::NodeFlag::LOWER_BOUND && ttData.score >= beta)
            return beta;
        if (ttData.flag == TTable::NodeFlag::UPPER_BOUND && ttData.score <= alpha)
            return alpha;
        // stat_tt_hits--;
#ifdef MONITOR
        --stats.ttHits;
#endif
        // 剪枝失败，记录bestMove
        ttMove = ttData.bestMove;
    }

    // IID 内部迭代加深
    // 当TTable未命中的时候，进行快速搜索，积累TTable
    // （使用 isPVNode = (beta - alpha == 1) 作为是否为主要变例的判断）
    bool isPVNode = (beta > alpha + 1);  // 下方共用
    if (isPVNode && ttMove == 0 && depth >= ALLOW_IID_DEPTH) {
#ifdef MONITOR
        ++stats.iidTrials;
#endif
        // 快速搜索，仅用于积累TTable的生成
        PVS(board, currHash, depth - 2, ply, alpha, beta, false);
        TTable::TTableData iidData;
        if (TTable::read(currHash, iidData)) {
            ttMove = iidData.bestMove;
#ifdef MONITOR
            if (ttMove != 0)
                ++stats.iidHits;
#endif
        }
    }

    // 空步剪枝
    // 允许空步 && 深度足够 && 不是残局 && 分数不低
    int emptyCell = BitEngine::cntBit(~board.allBlocked());
    bool isEndGame = (emptyCell <= ENDGAME_PIECES);  // 和lmr共用
    if (allowNullMove && depth > ALLOW_NULLMOVE_DEPTH) {
        TTable::Score temScore = evaluateLite(board);
        if (!isEndGame && temScore >= beta) {
            // 监测探针
#ifdef MONITOR
            ++stats.nmpTrials;
#endif
            // 空步模拟
            int nullDepth = std::max(depth - NULLMOVE_R, 0);  // 绝不能小于0！！！
            BitEngine::SwitchPlayer(board);
            HashEngine::Key nexHash = currHash ^ HashEngine::playerBlackKey;
            // 零窗口速测
            TTable::Score nullScore
                = -PVS(board, nexHash, nullDepth, ply + 1, -beta, -beta + 1, false);
            // 空步子树内部禁止空步
            // 恢复棋盘
            BitEngine::SwitchPlayer(board);
            // 超时检测
            if (isTimeout_final)
                return 0;
            // 空步质量检测
            // 默认假设 currScore>=nullScore 必然成立，
            // 所以nullScore>=beta => currScore>=beta
            // 即证明了beta剪枝，返回逻辑和alpha-beta一致
            if (nullScore >= beta) {
                // 监测探针
#ifdef MONITOR
                ++stats.nmpCuts;
#endif
                // 分数没被压下去，合格
                TTable::write({currHash,
                               /*空步*/
                               0,
                               beta,
                               static_cast<TTable::Depth>(depth),
                               TTable::NodeFlag::LOWER_BOUND});
                return beta;
            }
        }  // end if
    }  // end if

    // Futility Pruning边缘剪枝
    // 禁止残局fp，禁止浅层fp，禁止PVNode主要变例fp
    if (!isEndGame && depth <= ALLOW_FP_DEPTH && !isPVNode) {
        // 非杀棋
        if (alpha > -TTable::SCORE_MATE + 1000 && beta < TTable::SCORE_MATE - 1000) {
#ifdef MONITOR
            ++stats.fpTrials;
#endif
            TTable::Score currSocre = evaluateLite(board);  // 当前分数速估
            TTable::Score margin = depth * FP_MARGIN_BASE;  // 理论最大反扑分数
            if (currSocre + margin <= alpha) {
                // 后续不可能超过alpha，剪枝
#ifdef MONITOR
                ++stats.fpCuts;
#endif
                return currSocre;
            }
        }
    }

    // // 生成走法
    // BitEngine::MoveList moveList;
    // BitEngine::generateAllMoves(board, moveList);

    // // 终局判断
    // if (moveList.count == 0) {
    //     return -TTable::SCORE_MATE + depth;  // +depth 争取死慢点/赢快点
    // }

    // DONE: 走法排序，放在状态机里了
    // 权重定义
    // 1. 置换表
    // 2. 杀手表
    // 3. 历史表
    // int moveWeight[BitEngine::MAX_AMAZON_MOVE_TYPE];
    // for (int i = 0; i < moveList.count; ++i) {
    //     BitEngine::Move m = moveList.moves[i];
    //     if (m == ttMove)
    //         moveWeight[i] = TTABLE_WEIGHT;
    //     else if (m == KTable[depth][0] || m == KTable[depth][1])
    //         moveWeight[i] = KTABLE_WEIGHT;
    //     else {
    //         using namespace BitEngine;
    //         moveWeight[i] = HTable[getFrom(m)][getTo(m)][getArrow(m)];
    //     }
    // }

    // [ 已弃用 ]
    // // 首节点，全窗口搜索
    // BitEngine::applyMove(board, bestMove);  // 借用bestMove代表moveList.moves[0]
    // bestScore = -PVS(board, depth - 1, -beta, -alpha);
    // BitEngine::resetMove(board, bestMove);
    // if (bestScore > alpha)
    //     alpha = bestScore;
    // if (!(alpha >= beta))  // beta剪枝失败，进入循环，从 i=1 开始

    // 搜索
    StateMachine stateMachine(board, ttMove, KTable[ply][0], KTable[ply][1]);

    TTable::Score bestScore = -TTable::SCORE_INFINITY;
    BitEngine::Move bestMove = 0;
    TTable::Score oriAlpha = alpha;

    int moveCnt = 0;
    BitEngine::Move move;

    // for (int i = 0; i < moveList.count; ++i) {
    while ((move = stateMachine.nextMove()) != 0) {
        ++moveCnt;
        // // 实时选择排序 -> 性质好，不需要全排一遍
        // int maxInx = i;
        // for (int j = i + 1; j < moveList.count; ++j) {
        //     if (moveWeight[j] > moveWeight[maxInx])
        //         maxInx = j;
        // }
        // std::swap(moveList.moves[maxInx], moveList.moves[i]);
        // std::swap(moveWeight[maxInx], moveWeight[i]);
        // // 取Move，即为权重最高的
        // BitEngine::Move move = moveList.moves[i];

        // N-Best截断
        // 禁止残局N-Best，禁止浅层N-Best，禁止PVNode主要变例N-Best
        if (!isEndGame && depth <= ALLOW_N_BEST && !isPVNode) {
            int maxMoveAllowed = (depth == N_BEST_DEPTH_1)   ? N_BEST_RANK_1
                                 : (depth == N_BEST_DEPTH_2) ? N_BEST_RANK_2
                                                             : N_BEST_RANK_3;
            // 超出预期，直接剪枝
            if (moveCnt > maxMoveAllowed) {
#ifdef MONITOR
                ++stats.nBestCuts;
#endif
                break;
            }
        }

        // 先计算哈希
        // board.player在数值上通过约定保证正确
        HashEngine::Key nexHash
            = HashEngine::updataHash(currHash,
                                     static_cast<HashEngine::Element>(board.player),
                                     BitEngine::getFrom(move),
                                     BitEngine::getTo(move),
                                     BitEngine::getArrow(move));

        // 再落子（避免切换玩家干扰hash）
        BitEngine::applyMove(board, move);

        // 搜索
        TTable::Score score;
        bool fullSearch = true;  // 默认进入全量搜索
        if (moveCnt == 1)        // 首节点全窗口搜索
            score = -PVS(board, nexHash, depth - 1, ply + 1, -beta, -alpha, allowNullMove);
        else if (!isEndGame && depth >= ALLOW_LMR_DEPTH && moveCnt >= ALLOW_LMR_RANK) {
            // lmr晚期移动缩减+零窗口 搜索
            int lmrDepth = depth - 1 - LMR_DEPTH_DECAY;
            // 动态调整
            if (moveCnt >= LMR_RANK_1)
                --lmrDepth;
            if (moveCnt >= LMR_RANK_2)
                --lmrDepth;
            // 监测探针
#ifdef MONITOR
            ++stats.lmrTrials;
#endif
            // lmr禁止空步，减少误判率
            score = -PVS(board, nexHash, lmrDepth, ply + 1, -alpha - 1, -alpha, false);
            if (score <= alpha) {  // 预期判断
                fullSearch = false;
                // 不符合预期，跳过全量搜索
                // 监测探针
#ifdef MONITOR
                ++stats.lmrCuts;
#endif
            }
#ifdef MONITOR
            else {
                ++stats.lmrMisses;
            }
#endif
        }

        // 零窗口 -> 全窗口
        if (fullSearch) {
            score = -PVS(board, nexHash, depth - 1, ply + 1, -alpha - 1, -alpha, allowNullMove);
            if (alpha < score && score < beta)  // 可以省一步更新alpha，使用-score
                // 零窗口尝试失败，重新全窗口
                score = -PVS(board, nexHash, depth - 1, ply + 1, -beta, -score, allowNullMove);
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
            // 监测探针
#ifdef MONITOR
            ++stats.betaCutoffs;
            if (move == ttMove && ttMove != 0)
                ++stats.ttCutoffs;
            else if (move == KTable[ply][0])
                ++stats.killer1Cutoffs;
            else if (move == KTable[ply][1])
                ++stats.killer2Cutoffs;
            else {
                ++stats.historyCutoffs;
                stats.historyRankSum += moveCnt;
            }
#endif

            // 更新杀手
            if (move != KTable[ply][0] && move != KTable[ply][1]) {  // 简易循环数组
                KTable[ply][1] = KTable[ply][0];
                KTable[ply][0] = move;
            }
            // 更新历史
            using namespace BitEngine;
            HTable[getFrom(move)][getTo(move)][getArrow(move)] += (depth * depth * 64);
            // 剪枝
            break;
        }
    }

    // 终局判断
    if (moveCnt == 0 && !stateMachine.hasNext()) {
        return -TTable::SCORE_MATE + depth;  // +depth 争取死慢点/赢快点
    }

    // 写入TTable置换表
    TTable::NodeFlag flag = TTable::NodeFlag::EXACT;
    if (bestScore <= oriAlpha)
        flag = TTable::NodeFlag::UPPER_BOUND;  // Fail-Low
    else if (bestScore >= beta)
        flag = TTable::NodeFlag::LOWER_BOUND;
    TTable::write({currHash, bestMove, bestScore, depth, flag});  // Fail-High

    // 返回最优解
    return bestScore;
}

// TODO: 轻量级评估函数
TTable::Score evaluateLite(const BitEngine::BitBoard& board) {
    // 监测探针
#ifdef MONITOR
    ++stats.evalsLite;
#endif

#ifdef MONITOR
    if (stats.evalsLite % MEM_MONITOR_EVALUATE_LITE_FREQ == 0
        && stats.evalsLite != 0) {  // 每固定次数调用监控一次
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        printf("[Memory Monitor - evaluateLite] RSS: %lu KB\n", pmc.WorkingSetSize / 1024);
#else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        printf("[Memory Monitor - evaluateLite] RSS: %ld KB\n", usage.ru_maxrss);
#endif
    }
#endif

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

    // 计算总分
    return LITE_FACTOR * (myMobility - opMobility);
}

TTable::Score evaluate(const BitEngine::BitBoard& board) {
    // // 调试
    // stat_nodes_evaluated++;
    // 监测探针
#ifdef MONITOR
    ++stats.evals;
#endif

#ifdef MONITOR
    if (stats.evals % MEM_MONITOR_EVALUATE_FREQ == 0
        && stats.evals != 0) {  // 每固定次数调用监控一次
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        printf("[Memory Monitor - evaluate] RSS: %lu KB\n", pmc.WorkingSetSize / 1024);
#else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        printf("[Memory Monitor - evaluate] RSS: %ld KB\n", usage.ru_maxrss);
#endif
    }
#endif

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
    // 监测探针
#ifdef MONITOR
    --stats.evals;
    ++stats.evalsEnd;
#endif

#ifdef MONITOR
    if (stats.evalsEnd % MEM_MONITOR_EVALUATE_ENDGAME_FREQ == 0
        && stats.evalsEnd != 0) {  // 每固定次数调用监控一次
#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        printf("[Memory Monitor - evaluateEndGame] RSS: %lu KB\n", pmc.WorkingSetSize / 1024);
#else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        printf("[Memory Monitor - evaluateEndGame] RSS: %ld KB\n", usage.ru_maxrss);
#endif
    }
#endif

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
