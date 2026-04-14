#include "code/BitEngine/BitEngine.h"

namespace VanitasBot::BitEngine {
Bitmap BitBoard::allPieces() const {
    return whites | blacks;
}
Bitmap BitBoard::allBlocked() const {
    return allPieces() | arrows;
}

int XYtoBit(int x, int y) {
    return y * AMAZON_BOARD_LENGTH + x;
}

void bitToXY(int bit, int& out_x, int& out_y) {
    out_x = bit % AMAZON_BOARD_LENGTH;
    out_y = bit / AMAZON_BOARD_LENGTH;
}

Bitmap makeMask(int offset_bit) {
    return 1ULL << offset_bit;
}

inline Move makeMove(int from, int to, int arrow) {
    return (Move)from | (Move)to << 7 | (Move)arrow << 14;
}
int getFrom(Move m) {
    return m & 0x7F;
}
int getTo(Move m) {
    return (m >> 7) & 0x7F;
}
int getArrow(Move m) {
    return (m >> 14) & 0x7F;
}

Bitmap generateQueenMoves(Bitmap from, Bitmap blocked) {
    // 所有可移动到的点
    Bitmap moves = 0;
    for (auto dir : ALL_DIRECTIONS) {
        // 当前所在点
        Bitmap curr = from;
        while (true) {
            // 向所选方向射线“滑行”
            switch (dir) {
                case Dirtion::N:
                    MOVE_TO_N(curr);
                    break;
                case Dirtion::E:
                    MOVE_TO_E(curr);
                    break;
                case Dirtion::NE:
                    MOVE_TO_NE(curr);
                    break;
                case Dirtion::NW:
                    MOVE_TO_NW(curr);
                    break;

                case Dirtion::S:
                    MOVE_TO_S(curr);
                    break;
                case Dirtion::W:
                    MOVE_TO_W(curr);
                    break;
                case Dirtion::SW:
                    MOVE_TO_SW(curr);
                    break;
                case Dirtion::SE:
                    MOVE_TO_SE(curr);
                    break;

                default:
                    break;
            }  // end Switch
            // 超出边界,或者撞到障碍，不记录
            if (curr == 0 || (curr & blocked))
                break;

            moves |= curr;  // 合规，记录
        }  // end while
    }  // end for

    // 返回合成图
    return moves;
}

std::vector<Move> generateAllMoves(const BitBoard& board) {
    // 缓存
    std::vector<Move> moves;
    moves.reserve(MAX_AMAZON_MOVE_TYPE);  // 约21.48KB，0.022MB

    // 确定当前局面
    Bitmap my_amazons = (board.player == Player::BLACK) ? board.blacks : board.whites;
    Bitmap blocked = board.allBlocked();

    // 开始生成
    static_assert(false, "generateAllMoves等待修改");
    for (int from = 0; from < 64; ++from) {
        if (!(my_amazons & makeMask(from)))
            continue;                                        // 找到自己的棋子
        Bitmap to_mask = generateQueenMoves(from, blocked);  // 移动射线

        for (int to = 0; to < 64; ++to) {
            if (!(to_mask & makeMask(to)))
                continue;  // 找到合法落点

            // 模拟：拔除起点障碍，在落点放上障碍
            Bitmap new_blocked = (blocked & ~makeMask(from)) | makeMask(to);
            Bitmap arrow_mask = generateQueenMoves(to, new_blocked);  // 射箭射线

            for (int arrow = 0; arrow < 64; ++arrow) {
                if (arrow_mask & makeMask(arrow)) {
                    moves.push_back(makeMove(from, to, arrow));  // 生成完整动作
                }
            }
        }
    }
    // 返回单图图集
    return moves;
}

void applyMove(BitBoard& board, Move move) {
    // 取出数据并bit化
    Bitmap from = makeMask(getFrom(move));
    Bitmap to = makeMask(getTo(move));
    Bitmap arrow = makeMask(getArrow(move));
    // 移动
    if (board.player == Player::BLACK) {
        clsBit(board.blacks, from);  // 消除原站位障碍判定
        setBit(board.blacks, to);    // 添加新展位障碍判定
    } else {
        clsBit(board.whites, from);  // 同上
        setBit(board.whites, to);
    }
    // 更新箭矢障碍判定
    addArrow(board, arrow);
    // 切换玩家
    SwitchPlayer(board);
}

void resetMove(BitBoard& board, Move move) {
    // 反向取出数据并bit化
    Bitmap from = makeMask(getTo(move));
    Bitmap to = makeMask(getFrom(move));
    Bitmap arrow = makeMask(getArrow(move));
    // 切换玩家
    SwitchPlayer(board);
    // 移动(已经使用了反向的数据)
    if (board.player == Player::BLACK) {
        clsBit(board.blacks, from);  // 消除原站位障碍判定
        setBit(board.blacks, to);    // 添加新展位障碍判定
    } else {
        clsBit(board.whites, from);  // 同上
        setBit(board.whites, to);
    }
    // 删除箭矢障碍判定
    delArrow(board, arrow);
}

void SwitchPlayer(BitBoard& board) {
    board.player = static_cast<Player>(-1 * static_cast<int>(board.player));
}

void addArrow(BitBoard& board, Bitmap arrow) {
    setBit(board.arrows, arrow);
}

void delArrow(BitBoard& board, Bitmap arrow) {
    clsBit(board.arrows, arrow);
}

}  // namespace VanitasBot::BitEngine
