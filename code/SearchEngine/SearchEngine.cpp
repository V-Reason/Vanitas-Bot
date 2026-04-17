#include "SearchEngine.h"

namespace VanitasBot::SearchEngine {
BitEngine::Move search(BitEngine::BitBoard& board) {
    // TODO: 开局库检查

    // 初始化哈希
    HashEngine::init();
    board.hash = HashEngine::generateHash(board);

    // IDS迭代加深搜索
    BitEngine::Move globalBestMove = 0;
    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        TTable::Score score = PVS(board, depth, -TTable::SCORE_INFINITY, TTable::SCORE_INFINITY);

        // TODO: isTimeOut检测

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

TTable::Score PVS(BitEngine::BitBoard& board, TTable::Depth depth, TTable::Score alpha, TTable::Score beta) {
    // 毎若干搜索后检查一次超时
    static int nodesCnt = 0;
    // if (!(++nodesCnt & CHECK_GAP_MASK) && checkTimeout()) return 0;
    // if (isTimeOut) return 0;

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

    // 搜索
    TTable::Score bestScore = -TTable::SCORE_INFINITY;
    BitEngine::Move bestMove = moveList.moves[0];
    TTable::Score ori_Alpha = alpha;

    // 首节点，全窗口搜索
    BitEngine::applyMove(board, bestMove);  // 借用bestMove代表moveList.moves[0]
    bestScore = -PVS(board, depth - 1, -beta, -alpha);
    BitEngine::resetMove(board, bestMove);
    if (bestScore > alpha)
        alpha = bestScore;
    if (!(alpha >= beta))  // beta剪枝失败，进入循环，从 i=1 开始
        for (int i = 1; i < moveList.count; ++i) {
            // 取Move
            BitEngine::Move move = moveList.moves[i];
            // 落子（内部自动更新hash）
            BitEngine::applyMove(board, move);

            // 零窗口搜索
            TTable::Score score;
            score = -PVS(board, depth - 1, -alpha - 1, -alpha);
            if (alpha < score && score < beta)                  // 可以省一步更新alpha，使用-score
                score = -PVS(board, depth - 1, -beta, -score);  // 零窗口尝试失败，重新搜索

            // 悔棋
            BitEngine::resetMove(board, move);

            // TODO:超时处理
            // if(isTimeOut) return 0;

            // 更新结果
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            // 收缩窗口
            if (score > alpha)
                alpha = score;
            // beta剪枝
            if (alpha >= beta)
                break;
        }

    // 写入TTable置换表
    TTable::NodeFlag flag = TTable::NodeFlag::EXACT;
    if (bestScore <= ori_Alpha)
        flag = TTable::NodeFlag::UPPER_BOUND;  // Fail-Low
    else if (bestScore > beta)
        flag = TTable::NodeFlag::LOWER_BOUND;
    TTable::write({board.hash, bestMove, bestScore, depth, flag});  // Fail-High

    // 返回最优解
    return bestScore;
}
}  // namespace VanitasBot::SearchEngine
