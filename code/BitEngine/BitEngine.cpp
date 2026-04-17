#include "BitEngine.h"

namespace VanitasBot::BitEngine {

// [ 已弃用 / 备选 ]
// 射线投射法 Ray-Casting
// Bitmap generateQueenMoves(Bitmap from, Bitmap blocked) {
//     // 所有可移动到的点
//     Bitmap moves = 0;
//     for (auto dir : ALL_DIRECTIONS) {
//         // 当前所在点
//         Bitmap curr = from;
//         while (true) {
//             // 向所选方向射线“滑行”
//             switch (dir) {
//                 case Dirtion::N:
//                     curr = MOVE_TO_N(curr);
//                     break;
//                 case Dirtion::E:
//                     curr = MOVE_TO_E(curr);
//                     break;
//                 case Dirtion::NE:
//                     curr = MOVE_TO_NE(curr);
//                     break;
//                 case Dirtion::NW:
//                     curr = MOVE_TO_NW(curr);
//                     break;

//                 case Dirtion::S:
//                     curr = MOVE_TO_S(curr);
//                     break;
//                 case Dirtion::W:
//                     curr = MOVE_TO_W(curr);
//                     break;
//                 case Dirtion::SW:
//                     curr = MOVE_TO_SW(curr);
//                     break;
//                 case Dirtion::SE:
//                     curr = MOVE_TO_SE(curr);
//                     break;

//                 default:
//                     break;
//             }  // end Switch
//             // 超出边界,或者撞到障碍，不记录
//             if (curr == 0 || (curr & blocked))
//                 break;

//             moves |= curr;  // 合规，记录
//         }  // end while
//     }  // end for

//     // 返回合成图
//     return moves;
// }

// 并行计算法 Kogge-Stone
// 皇后作为进位信号源Gen，空地作为传播信号Pro
Bitmap generateQueenMoves(Bitmap from, Bitmap blocked) {
    Bitmap empty = ~blocked;  // 可通行的空地

    // 八个方向并行计算
    Bitmap moves = koggeStone_N(from, empty) | koggeStone_S(from, empty) | koggeStone_E(from, empty)
                   | koggeStone_W(from, empty) | koggeStone_NE(from, empty) | koggeStone_NW(from, empty)
                   | koggeStone_SE(from, empty) | koggeStone_SW(from, empty);

    return moves & empty;  // 确保只能落子在空地上
}

// std::vector<Move> generateAllMoves(const BitBoard& board) {
void generateAllMoves(const BitBoard& board, MoveList& out_list) {
    // [ 已经改用静态内存MoveList ]
    // // 缓存
    // std::vector<Move> moves;
    // moves.reserve(MAX_AMAZON_MOVE_TYPE);  // 约21.48KB，0.022MB

    out_list.clear();  // 清空重置内存

    // 确定当前局面
    Bitmap my_amazons = (board.player == Player::BLACK) ? board.blacks : board.whites;
    Bitmap blocked = board.allBlocked();

    // 开始生成
    Bitmap pieces = my_amazons;
    while (pieces) {
        // 取出棋子位置
        Index from_idx = fnlBit(pieces);
        Bitmap from = makeMask(from_idx);
        kicBit(pieces);
        // 生成该棋子的Moves
        Bitmap all_to = generateQueenMoves(from, blocked);

        // 遍历合法落点
        Bitmap targets_pieces = all_to;
        while (targets_pieces) {
            // 取出一次to的坐标
            Index to_idx = fnlBit(targets_pieces);
            Bitmap once_to = makeMask(to_idx);
            kicBit(targets_pieces);
            // 移动
            // Bitmap new_blocked = blocked;
            // clsBit(new_blocked, from);
            // setBit(new_blocked, once_to);
            Bitmap new_blocked = (blocked ^ from) | once_to;  // 简化了代码
            // 射箭
            Bitmap all_arrows = generateQueenMoves(once_to, new_blocked);

            // 遍历合法落箭
            Bitmap arrows_pieces = all_arrows;
            while (arrows_pieces) {
                // 取出一次落箭位置
                Index arrow_idx = fnlBit(arrows_pieces);
                kicBit(arrows_pieces);

                // 生成完整动作
                // moves.push_back(makeMove(from_idx, to_idx, arrow_idx));
                out_list.push(makeMove(from_idx, to_idx, arrow_idx));
            }
        }
    }

    // // 返回单图图集
    // return moves;
}

void applyMove(BitBoard& board, Move move) {
    // 取出数据
    Index from_inx = getFrom(move);
    Index to_inx = getTo(move);
    Index arrow_inx = getArrow(move);

    // 更新局面hash值
    // HashEngine::Element player =
    //     (board.player == Player::BLACK) ? HashEngine::Element::PLAYER_BLACK : HashEngine::Element::PLAYER_WHITE;
    board.hash = HashEngine::updataHash(
        board.hash, static_cast<HashEngine::Element>(board.player), from_inx, to_inx, arrow_inx);
    // player在数值上通过约定保证正确

    // 数据bit化
    Bitmap from = makeMask(from_inx);
    Bitmap to = makeMask(to_inx);
    Bitmap arrow = makeMask(arrow_inx);
    // 移动
    if (board.player == Player::BLACK) {
        // clsBit(board.blacks, from);  // 消除原站位障碍判定
        // setBit(board.blacks, to);    // 添加新展位障碍判定
        movBit(board.blacks, from, to);
    } else {
        // clsBit(board.whites, from);  // 同上
        // setBit(board.whites, to);
        movBit(board.whites, from, to);
    }
    // 更新箭矢障碍判定
    addArrow(board, arrow);
    // 切换玩家（必须在updataHash之后，因为要作为参数传入）
    SwitchPlayer(board);
}

void resetMove(BitBoard& board, Move move) {
    // 取出数据
    Index ori_from_inx = getFrom(move);
    Index ori_to_inx = getTo(move);
    Index ori_arrow_inx = getArrow(move);

    // 数据bit化
    Bitmap ori_to = makeMask(ori_to_inx);
    Bitmap ori_from = makeMask(ori_from_inx);
    Bitmap ori_arrow = makeMask(ori_arrow_inx);

    // 切换回上一个玩家（必须在updataHash之前！）
    SwitchPlayer(board);
    // 删除箭矢障碍判定
    delArrow(board, ori_arrow);

    // 移回棋子
    if (board.player == Player::BLACK) {
        // clsBit(board.blacks, ori_to);    // 消除原站位障碍判定
        // setBit(board.blacks, ori_from);  // 添加新站位障碍判定
        movBit(board.blacks, ori_to, ori_from);
    } else {
        // clsBit(board.whites, ori_to);  // 同上
        // setBit(board.whites, ori_from);
        movBit(board.whites, ori_to, ori_from);
    }

    // 更新局面hash值
    // HashEngine::Element player =
    //     (board.player == Player::BLACK) ? HashEngine::Element::PLAYER_BLACK : HashEngine::Element::PLAYER_WHITE;
    board.hash = HashEngine::updataHash(
        board.hash, static_cast<HashEngine::Element>(board.player), ori_from_inx, ori_to_inx, ori_arrow_inx);
    // player在数值上通过约定保证正确
}

}  // namespace VanitasBot::BitEngine
