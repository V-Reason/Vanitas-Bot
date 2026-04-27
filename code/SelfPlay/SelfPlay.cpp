#define LOGGER_ON
#define SELF_PLAY_MAIN
#define MONITOR_LITE
#include "../BitEngine/BitEngine.h"
#include "../IOEngine/IOEngine.h"
#include "../SearchEngine/SearchEngine.h"
#include "../Utilities/Logger/Logger.h"
#include "../Utilities/Timer/Timer.h"

#include <chrono>
#include <climits>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace VanitasBot::SearchEngine {
extern BitEngine::Move KTable[MAX_PLY][KILLER_NUM];
extern MoveWeight HTable[BitEngine::AMAZON_BOARD_SQUARE][BitEngine::AMAZON_BOARD_SQUARE]
                        [BitEngine::AMAZON_BOARD_SQUARE];
extern BitEngine::MoveList moveListPool[MAX_PLY];
extern MoveWeight moveWeightPool[MAX_PLY][BitEngine::MAX_AMAZON_MOVE_TYPE];
;
// 将Move转换为字符串表示
std::string moveToString(BitEngine::Move move) {
    BitEngine::Index from = BitEngine::getFrom(move);
    BitEngine::Index to = BitEngine::getTo(move);
    BitEngine::Index arrow = BitEngine::getArrow(move);

    int from_x = from % BitEngine::AMAZON_BOARD_LENGTH;
    int from_y = from / BitEngine::AMAZON_BOARD_LENGTH;
    int to_x = to % BitEngine::AMAZON_BOARD_LENGTH;
    int to_y = to / BitEngine::AMAZON_BOARD_LENGTH;
    int arrow_x = arrow % BitEngine::AMAZON_BOARD_LENGTH;
    int arrow_y = arrow / BitEngine::AMAZON_BOARD_LENGTH;

    // 使用双数字格式 (行-列格式)
    int from_row = from_y;
    int from_col = from_x;
    int to_row = to_y;
    int to_col = to_x;
    int arrow_row = arrow_y;
    int arrow_col = arrow_x;

    std::ostringstream oss;
    oss << from_row << from_col << '-' << to_row << to_col << '-' << arrow_row << arrow_col;
    return oss.str();
}

// 解析字符串为Move
BitEngine::Move stringToMove(const std::string& moveStr) {
    // 解析格式: 00-11-22 (双数字格式，如00代表第0行第0列)
    if (moveStr.length() != 8)  // 格式必须是 "xy-xy-xy"
        return 0;               // 格式错误

    // 检查分隔符位置
    if (moveStr[2] != '-' || moveStr[5] != '-')
        return 0;  // 分隔符位置错误

    try {
        // 解析双数字坐标 (第一位是行，第二位是列)
        int from_row = moveStr[0] - '0';
        int from_col = moveStr[1] - '0';
        int to_row = moveStr[3] - '0';
        int to_col = moveStr[4] - '0';
        int arrow_row = moveStr[6] - '0';
        int arrow_col = moveStr[7] - '0';

        // 检查坐标范围 (0-7 对应8x8棋盘)
        if (from_row >= 0 && from_row < BitEngine::AMAZON_BOARD_LENGTH && from_col >= 0
            && from_col < BitEngine::AMAZON_BOARD_LENGTH && to_row >= 0
            && to_row < BitEngine::AMAZON_BOARD_LENGTH && to_col >= 0
            && to_col < BitEngine::AMAZON_BOARD_LENGTH && arrow_row >= 0
            && arrow_row < BitEngine::AMAZON_BOARD_LENGTH && arrow_col >= 0
            && arrow_col < BitEngine::AMAZON_BOARD_LENGTH) {
            // 转换为线性索引
            BitEngine::Index from = from_row * BitEngine::AMAZON_BOARD_LENGTH + from_col;
            BitEngine::Index to = to_row * BitEngine::AMAZON_BOARD_LENGTH + to_col;
            BitEngine::Index arrow = arrow_row * BitEngine::AMAZON_BOARD_LENGTH + arrow_col;

            return BitEngine::makeMove(from, to, arrow);
        }
    } catch (...) {
        // 转换失败，返回无效走法
        return 0;
    }

    return 0;  // 无效坐标
}

// 自博弈配置参数
struct SelfPlayConfig {
    int game_count = 100;                            // 对弈局数
    int time_per_move_ms = 1000;                     // 每步思考时间(毫秒)
    bool save_games = true;                          // 是否保存对弈记录
    std::string output_file = "selfplay_games.txt";  // 输出文件名
    bool enable_random_opening = true;               // 是否随机开局
    int random_opening_moves = 4;                    // 随机开局步数
    std::string input_file = "";                     // 输入局面文件
    bool use_custom_board = false;                   // 是否使用自定义局面
    BitEngine::BitBoard custom_board;                // 自定义局面
    bool step_by_step = false;                       // 是否逐步控制
};

class SelfPlayManager {
   private:
    SelfPlayConfig config_;
    std::mt19937 rng_;
    std::uniform_int_distribution<> move_picker_;

   public:
    explicit SelfPlayManager(const SelfPlayConfig& config = SelfPlayConfig{})
        : config_(config), rng_(std::time(nullptr)), move_picker_(0, 100) {}

    // 从文件读取特定局面
    bool loadCustomBoard(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "错误：无法打开局面文件 " << filename << std::endl;
            return false;
        }

        std::string line;
        BitEngine::BitBoard board = {};

        while (std::getline(file, line)) {
            if (line.substr(0, 6) == "BLACK:") {
                // 解析黑方棋子位置（双数字格式，如00 11 22）
                std::istringstream iss(line.substr(6));
                std::string pos;
                while (iss >> pos) {
                    try {
                        if (pos.length() == 2) {
                            // 解析双数字格式 (行号-列号)
                            int row = pos[0] - '0';
                            int col = pos[1] - '0';

                            if (row >= 0 && row < BitEngine::AMAZON_BOARD_LENGTH && col >= 0
                                && col < BitEngine::AMAZON_BOARD_LENGTH) {
                                BitEngine::Index index = row * BitEngine::AMAZON_BOARD_LENGTH + col;
                                BitEngine::setBit(board.blacks, BitEngine::makeMask(index));
                            }
                        }
                    } catch (...) {
                        // 双数字解析失败，跳过该位置
                        std::cerr << "警告：无效的双数字位置 " << pos << std::endl;
                    }
                }
            } else if (line.substr(0, 6) == "WHITE:") {
                // 解析白方棋子位置（双数字格式，如00 11 22）
                std::istringstream iss(line.substr(6));
                std::string pos;
                while (iss >> pos) {
                    try {
                        if (pos.length() == 2) {
                            // 解析双数字格式 (行号-列号)
                            int row = pos[0] - '0';
                            int col = pos[1] - '0';

                            if (row >= 0 && row < BitEngine::AMAZON_BOARD_LENGTH && col >= 0
                                && col < BitEngine::AMAZON_BOARD_LENGTH) {
                                BitEngine::Index index = row * BitEngine::AMAZON_BOARD_LENGTH + col;
                                BitEngine::setBit(board.whites, BitEngine::makeMask(index));
                            }
                        }
                    } catch (...) {
                        // 双数字解析失败，跳过该位置
                        std::cerr << "警告：无效的双数字位置 " << pos << std::endl;
                    }
                }
            } else if (line.substr(0, 6) == "ARROW:") {
                // 解析箭矢位置（双数字格式，如00 11 22）
                std::istringstream iss(line.substr(6));
                std::string pos;
                while (iss >> pos) {
                    try {
                        if (pos.length() == 2) {
                            // 解析双数字格式 (行号-列号)
                            int row = pos[0] - '0';
                            int col = pos[1] - '0';

                            if (row >= 0 && row < BitEngine::AMAZON_BOARD_LENGTH && col >= 0
                                && col < BitEngine::AMAZON_BOARD_LENGTH) {
                                BitEngine::Index index = row * BitEngine::AMAZON_BOARD_LENGTH + col;
                                BitEngine::setBit(board.arrows, BitEngine::makeMask(index));
                            }
                        }
                    } catch (...) {
                        // 双数字解析失败，跳过该位置
                        std::cerr << "警告：无效的双数字位置 " << pos << std::endl;
                    }
                }
            } else if (line.substr(0, 7) == "PLAYER:") {
                // 解析当前玩家
                std::string player = line.substr(7);
                if (player.find("BLACK") != std::string::npos) {
                    board.player = BitEngine::Player::BLACK;
                } else {
                    board.player = BitEngine::Player::WHITE;
                }
            }
        }

        config_.custom_board = board;
        config_.use_custom_board = true;
        file.close();
        return true;
    }

    /*显示当前棋盘状态
    void displayBoard(const BitEngine::BitBoard& board) {
        std::cout << "\n当前棋盘状态:\n";
        std::cout << "  ";
        for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; x++) {
            std::cout << (char)('A' + x) << " ";
        }
        std::cout << "\n";

        for (int y = 0; y < BitEngine::AMAZON_BOARD_LENGTH; y++) {
            std::cout << (y + 1) << " ";
            for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; x++) {
                BitEngine::Bitmap pos_mask
                    = BitEngine::makeMask(y * BitEngine::AMAZON_BOARD_LENGTH + x);

                if (board.blacks & pos_mask) {
                    std::cout << "B ";  // 黑方棋子
                } else if (board.whites & pos_mask) {
                    std::cout << "W ";  // 白方棋子
                } else if (board.arrows & pos_mask) {
                    std::cout << "o ";  // 箭矢
                } else {
                    std::cout << ". ";  // 空位
                }
            }
            std::cout << (y + 1) << "\n";
        }

        std::cout << "  ";
        for (int x = 0; x < BitEngine::AMAZON_BOARD_LENGTH; x++) {
            std::cout << (char)('A' + x) << " ";
        }
        std::cout << "\n";
        std::cout << "当前玩家: " << (board.player == BitEngine::Player::BLACK ? "BLACK" : "WHITE")
                  << "\n";
    }*/

    // 获取用户输入的走法
    BitEngine::Move getUserMove(const BitEngine::BitBoard& board) {
        std::string input;
        std::cout << "请输入走法 (格式: 00-11-22) 或 'ai' 让AI自动走: ";
        std::cin >> input;

        if (input == "ai" || input == "AI") {
            // 让AI搜索走法
            std::cout << "AI正在思考..." << std::endl;
            BitEngine::BitBoard temp_board_for_search = board;
            return VanitasBot::SearchEngine::search(temp_board_for_search);
        }

        // 解析用户输入的走法
        BitEngine::Move userMove = stringToMove(input);

        // 验证走法是否合法
        BitEngine::MoveList move_list;
        BitEngine::generateAllMoves(board, move_list);

        for (int i = 0; i < move_list.count; i++) {
            if (move_list.moves[i] == userMove) {
                return userMove;  // 合法走法
            }
        }

        std::cout << "非法走法！请重新输入。" << std::endl;
        return getUserMove(board);  // 递归重新输入
    }

    // 生成初始棋盘
    BitEngine::BitBoard generateInitialBoard() {
        if (config_.use_custom_board && !config_.input_file.empty()) {
            // 如果配置了自定义局面文件，则加载该局面
            BitEngine::BitBoard loaded_board = config_.custom_board;
            return loaded_board;
        }

        BitEngine::BitBoard board;
        IOEngine::initBoard(board);
        return board;
    }

    // 随机选择开局走法
    void applyRandomOpening(BitEngine::BitBoard& board, int num_moves) {
        if (!config_.enable_random_opening || num_moves <= 0)
            return;

        for (int i = 0; i < num_moves; ++i) {
            BitEngine::MoveList move_list;
            BitEngine::generateAllMoves(board, move_list);

            // 将MoveList转换为vector方便处理
            std::vector<BitEngine::Move> moves;
            for (int j = 0; j < move_list.count; ++j) {
                moves.push_back(move_list.moves[j]);
            }

            if (moves.empty())
                break;

            // 随机选择一个走法
            int selected_idx = move_picker_(rng_) % moves.size();
            BitEngine::Move selected_move = moves[selected_idx];

            // 应用走法
            BitEngine::applyMove(board, selected_move);

            // 切换玩家
            // board.player = (board.player == BitEngine::Player::BLACK) ? BitEngine::Player::WHITE
            //                                                          : BitEngine::Player::BLACK;
        }
    }

    // 交互式单局游戏（逐步控制）
    void interactiveGame() {
        // 必要的初始化步骤
        Utilities::Timer::resetStartTime();
        IOEngine::initIOEngine();
        BitEngine::BitBoard board = generateInitialBoard();

        // 如果不是自定义局面，则应用随机开局
        if (!config_.use_custom_board) {
            applyRandomOpening(board, config_.random_opening_moves);
        }

        std::cout << "=== 逐步控制模式开始 ===" << std::endl;
        VanitasBot::Utilities::Logger::showBitboard(board, "当前棋盘状态");

        int step_count = 0;

        while (true) {
            // 检查是否超时
            if (Utilities::Timer::checkTimeouts()) {
                std::cout << "超时！游戏结束。" << std::endl;
                break;
            }

            // 生成所有合法走法
            BitEngine::MoveList move_list;
            BitEngine::generateAllMoves(board, move_list);

            // 将MoveList转换为vector方便处理
            std::vector<BitEngine::Move> legal_moves;
            for (int i = 0; i < move_list.count; ++i) {
                legal_moves.push_back(move_list.moves[i]);
            }

            // 检查游戏结束条件
            if (legal_moves.empty()) {
                std::cout << "游戏结束！" << std::endl;
                std::cout << "获胜方: "
                          << ((board.player == BitEngine::Player::BLACK) ? "WHITE" : "BLACK")
                          << std::endl;
                break;
            }

            std::cout << "\n--- 第 " << ++step_count << " 步 ---" << std::endl;
            std::cout << "轮到 " << (board.player == BitEngine::Player::BLACK ? "BLACK" : "WHITE")
                      << " 走棋" << std::endl;

            if (config_.step_by_step) {
                // 逐步控制模式
                BitEngine::Move chosen_move = getUserMove(board);

                if (chosen_move == 0) {
                    std::cout << "无法获取有效走法，退出游戏。" << std::endl;
                    break;
                }

                // 应用走法
                BitEngine::applyMove(board, chosen_move);

                std::cout << "走法: " << moveToString(chosen_move) << std::endl;
            } else {
                // AI自动模式
                // 设置时间限制
                Utilities::Timer::timeoutConfigs[0].timeoutMs = config_.time_per_move_ms;

                // 使用搜索函数获取最佳走法
                BitEngine::BitBoard temp_board = board;
                BitEngine::Move best_move = VanitasBot::SearchEngine::search(temp_board);

                // 如果搜索失败或超时，随机选择一个走法
                if (best_move == 0 && !legal_moves.empty()) {
                    best_move = legal_moves[0];  // 选择第一个走法作为备选
                }

                // 应用走法
                BitEngine::applyMove(board, best_move);

                std::cout << "AI走法: " << moveToString(best_move) << std::endl;
            }

            // 切换玩家
            // board.player = (board.player == BitEngine::Player::BLACK) ? BitEngine::Player::WHITE
            //                                                         : BitEngine::Player::BLACK;

            // 显示更新后的棋盘
            VanitasBot::Utilities::Logger::showBitboard(board, "当前棋盘状态");

            // 检查是否达到最大步数（平局条件）
            if (step_count >= 500) {
                std::cout << "达到最大步数，平局！" << std::endl;
                break;
            }

            // 等待用户确认下一步
            if (config_.step_by_step) {
                std::string cont;
                std::cout << "按回车继续...";
                std::cin.ignore();
                std::getline(std::cin, cont);
            }
        }
    }

    // 执行单局游戏（非交互模式）
    struct GameResult {
        std::vector<BitEngine::Move> moves;
        BitEngine::Player winner;
        bool game_interrupted;  // 表示游戏是否因超时等中断
        int game_length;
        bool is_draw;
    };

    GameResult playSingleGame() {
        // 必要的初始化步骤
        Utilities::Timer::resetStartTime();
        // 清理全局状态，确保每局开始时是干净的
        VanitasBot::SearchEngine::isTimeout_final = false;
        memset(VanitasBot::TTable::TTable, 0, sizeof(VanitasBot::TTable::TTable));
        memset(VanitasBot::SearchEngine::KTable, 0, sizeof(VanitasBot::SearchEngine::KTable));
        memset(VanitasBot::SearchEngine::HTable, 0, sizeof(VanitasBot::SearchEngine::HTable));
        // 清空走法列表池
        memset(VanitasBot::SearchEngine::moveListPool,
               0,
               sizeof(VanitasBot::SearchEngine::moveListPool));

        // 清空走法权重池
        memset(VanitasBot::SearchEngine::moveWeightPool,
               0,
               sizeof(VanitasBot::SearchEngine::moveWeightPool));
        HashEngine::init();
        BitEngine::BitBoard board = generateInitialBoard();

        // 如果不是自定义局面，则应用随机开局
        if (!config_.use_custom_board) {
            applyRandomOpening(board, config_.random_opening_moves);
        }

        GameResult result;
        result.winner = BitEngine::Player::BLACK;  // 默认设为BLACK，稍后根据情况更新
        result.game_interrupted = true;            // 默认设为中断状态，稍后更新
        result.game_length = 0;
        result.is_draw = false;

        // 游戏主循环
        while (true) {
            // 生成所有合法走法
            BitEngine::MoveList move_list;
            BitEngine::generateAllMoves(board, move_list);

            // 将MoveList转换为vector方便处理
            std::vector<BitEngine::Move> legal_moves;
            for (int i = 0; i < move_list.count; ++i) {
                legal_moves.push_back(move_list.moves[i]);
            }

            // 检查游戏结束条件
            if (legal_moves.empty()) {
                // 当前玩家无法移动，对手获胜
                result.winner = (board.player == BitEngine::Player::BLACK)
                                    ? BitEngine::Player::WHITE
                                    : BitEngine::Player::BLACK;
                result.game_interrupted = false;  // 游戏正常结束
                break;
            }

            // 设置时间限制
            Utilities::Timer::timeoutConfigs[0].timeoutMs = config_.time_per_move_ms;

            // 使用搜索函数获取最佳走法
            BitEngine::BitBoard temp_board_for_search = board;
            BitEngine::Move best_move = VanitasBot::SearchEngine::search(temp_board_for_search);

            // 如果搜索失败或超时，随机选择一个走法
            if (best_move == 0 && !legal_moves.empty()) {
                // 尝试评估每个走法并选择最好的
                BitEngine::Move best_alternative = legal_moves[0];
                int best_score = INT_MIN;

                for (const auto& move : legal_moves) {
                    // 创建临时棋盘以评估走法
                    BitEngine::BitBoard test_board = board;
                    BitEngine::applyMove(test_board, move);

                    // 使用评估函数来评分
                    int score = VanitasBot::SearchEngine::evaluate(test_board);

                    if (score > best_score) {
                        best_score = score;
                        best_alternative = move;
                    }
                }
                best_move = best_alternative;
            }

            // 记录走法
            result.moves.push_back(best_move);
            result.game_length++;

            // 应用走法
            BitEngine::applyMove(board, best_move);

            // 切换玩家
            // board.player = (board.player == BitEngine::Player::BLACK) ? BitEngine::Player::WHITE
            //                                                         : BitEngine::Player::BLACK;

            // 检查是否达到最大步数（平局条件）
            if (result.game_length >= 500) {  // 设定最大步数限制
                result.is_draw = true;
                result.game_interrupted = false;  // 游戏正常结束，只是平局
                break;
            }
        }

        // 清理全局状态，避免影响下一局游戏
        // 1. 清理TTable（置换表）
        memset(VanitasBot::TTable::TTable, 0, sizeof(VanitasBot::TTable::TTable));

        // 清空杀手表
        memset(VanitasBot::SearchEngine::KTable, 0, sizeof(VanitasBot::SearchEngine::KTable));

        // 清空历史表
        memset(VanitasBot::SearchEngine::HTable, 0, sizeof(VanitasBot::SearchEngine::HTable));

        // 清空走法列表池
        memset(VanitasBot::SearchEngine::moveListPool,
               0,
               sizeof(VanitasBot::SearchEngine::moveListPool));

        // 清空走法权重池
        memset(VanitasBot::SearchEngine::moveWeightPool,
               0,
               sizeof(VanitasBot::SearchEngine::moveWeightPool));

        // 2. 重置HashEngine
        HashEngine::init();

        // 3. 重置其他全局状态
        Utilities::Timer::resetStartTime();
        VanitasBot::SearchEngine::isTimeout_final = false;
        return result;
    }

    // 运行多局自博弈
    void runSelfPlay() {
        // 必要的初始化步骤
        Utilities::Timer::resetStartTime();
        IOEngine::initIOEngine();
        // 如果指定了输入文件，尝试加载
        if (!config_.input_file.empty()) {
            loadCustomBoard(config_.input_file);
        }

        if (config_.step_by_step) {
            // 逐步控制模式
            interactiveGame();
            return;
        }

        std::cout << "开始自博弈，共 " << config_.game_count << " 局..." << std::endl;
        if (config_.use_custom_board) {
            std::cout << "使用自定义局面: " << config_.input_file << std::endl;
        } else {
            std::cout << "使用标准开局" << std::endl;
        }

        int black_wins = 0;
        int white_wins = 0;
        int draws = 0;
        int interrupted = 0;

        std::ofstream output_file;
        if (config_.save_games) {
            output_file.open(config_.output_file);
            if (!output_file.is_open()) {
                std::cerr << "警告：无法打开输出文件 " << config_.output_file << std::endl;
            }
        }

        for (int game_num = 0; game_num < config_.game_count; ++game_num) {
            std::cout << "正在执行第 " << (game_num + 1) << " 局..." << std::flush;

            auto start_time = std::chrono::high_resolution_clock::now();

            GameResult result = playSingleGame();

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration
                = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            // 统计结果
            if (result.game_interrupted) {
                interrupted++;
                std::cout << " 中断 (步数: " << result.game_length << ")" << std::endl;
            } else if (result.is_draw) {
                draws++;
                std::cout << " 平局 (步数: " << result.game_length << ", 耗时: " << duration.count()
                          << "ms)" << std::endl;
            } else if (result.winner == BitEngine::Player::BLACK) {
                black_wins++;
                std::cout << " 黑胜 (步数: " << result.game_length << ", 耗时: " << duration.count()
                          << "ms)" << std::endl;
            } else if (result.winner == BitEngine::Player::WHITE) {
                white_wins++;
                std::cout << " 白胜 (步数: " << result.game_length << ", 耗时: " << duration.count()
                          << "ms)" << std::endl;
            } else {
                std::cout << " 未知结果 (步数: " << result.game_length << ")" << std::endl;
            }

            // 保存游戏记录
            if (config_.save_games && output_file.is_open()) {
                output_file << "Game " << (game_num + 1) << ":\n";
                if (result.game_interrupted) {
                    output_file << "Winner: Interrupted\n";
                } else {
                    output_file << "Winner: "
                                << (result.is_draw
                                        ? "Draw"
                                        : (result.winner == BitEngine::Player::BLACK ? "Black"
                                                                                     : "White"))
                                << "\n";
                }
                output_file << "Length: " << result.game_length << "\n";

                output_file << "Moves: ";
                for (size_t i = 0; i < result.moves.size(); ++i) {
                    if (i > 0)
                        output_file << " ";
                    // 将走法转换为可读格式
                    output_file << moveToString(result.moves[i]);
                }
                output_file << "\n\n";
            }
        }

        // 输出统计结果
        std::cout << "\n=== 自博弈统计结果 ===" << std::endl;
        std::cout << "总局数: " << config_.game_count << std::endl;
        std::cout << "黑方获胜: " << black_wins << " ("
                  << (config_.game_count > 0 ? (black_wins * 100.0 / config_.game_count) : 0)
                  << "%)" << std::endl;
        std::cout << "白方获胜: " << white_wins << " ("
                  << (config_.game_count > 0 ? (white_wins * 100.0 / config_.game_count) : 0)
                  << "%)" << std::endl;
        std::cout << "平局: " << draws << " ("
                  << (config_.game_count > 0 ? (draws * 100.0 / config_.game_count) : 0) << "%)"
                  << std::endl;
        std::cout << "中断: " << interrupted << std::endl;

        if (config_.save_games) {
            output_file.close();
            std::cout << "游戏记录已保存至: " << config_.output_file << std::endl;
        }
    }

    // 获取当前配置
    const SelfPlayConfig& getConfig() const {
        return config_;
    }

    // 设置配置
    void setConfig(const SelfPlayConfig& config) {
        config_ = config;
    }
};

// 便捷函数：运行自博弈
void runSelfPlayGames(int game_count = 100, int time_per_move_ms = 1000) {
    SelfPlayConfig config;
    config.game_count = game_count;
    config.time_per_move_ms = time_per_move_ms;

    SelfPlayManager manager(config);
    manager.runSelfPlay();
}

// 主函数示例
}  // namespace VanitasBot::SearchEngine

#ifdef SELF_PLAY_MAIN
int main() {
    std::cout << "VanitasBot 自博弈系统" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "请选择测试模式：" << std::endl;
    std::cout << "1. 快速测试 (2局, 1000ms/步, 0步随机开局)" << std::endl;
    std::cout << "2. 标准测试 (5局, 1000ms/步, 1步随机开局)" << std::endl;
    std::cout << "3. 深度测试 (3局, 3000ms/步, 2步随机开局)" << std::endl;
    std::cout << "4. 无随机开局测试 (5局, 1000ms/步, 0步随机开局)" << std::endl;
    std::cout << "5. 逐步控制模式 (交互式)" << std::endl;
    std::cout << "6. 大规模测试 (20局, 1000ms/步, 1步随机开局)" << std::endl;
    std::cout << "7. 从文件输入自定义局面" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "请输入选项 (1-7): ";

    int choice;
    std::cin >> choice;

    VanitasBot::SearchEngine::SelfPlayConfig config;

    switch (choice) {
        case 1:
            config.game_count = 2;
            config.time_per_move_ms = 1000;
            config.enable_random_opening = false;
            config.random_opening_moves = 0;
            config.step_by_step = false;
            config.output_file = "test_quick.txt";
            std::cout << "\n模式: 快速测试" << std::endl;
            break;
        case 2:
            config.game_count = 5;
            config.time_per_move_ms = 1000;
            config.enable_random_opening = true;
            config.random_opening_moves = 1;
            config.step_by_step = false;
            config.output_file = "test_standard.txt";
            std::cout << "\n模式: 标准测试" << std::endl;
            break;
        case 3:
            config.game_count = 3;
            config.time_per_move_ms = 3000;
            config.enable_random_opening = true;
            config.random_opening_moves = 2;
            config.step_by_step = false;
            config.output_file = "test_deep.txt";
            std::cout << "\n模式: 深度测试" << std::endl;
            break;
        case 4:
            config.game_count = 2;
            config.time_per_move_ms = 1000;
            config.enable_random_opening = false;
            config.random_opening_moves = 0;
            config.step_by_step = false;
            config.output_file = "test_no_random.txt";
            std::cout << "\n模式: 无随机开局测试" << std::endl;
            break;
        case 5:
            config.game_count = 1;
            config.time_per_move_ms = 2000;
            config.enable_random_opening = true;
            config.random_opening_moves = 1;
            config.step_by_step = true;
            config.save_games = false;
            std::cout << "\n模式: 逐步控制模式" << std::endl;
            break;
        case 6:
            config.game_count = 20;
            config.time_per_move_ms = 1000;
            config.enable_random_opening = true;
            config.random_opening_moves = 1;
            config.step_by_step = false;
            config.output_file = "test_large.txt";
            std::cout << "\n模式: 大规模测试" << std::endl;
            break;
        case 7:
            std::cout << "\n模式: 从文件输入自定义局面" << std::endl;
            std::cout << "请输入局面文件路径: ";
            std::cin >> config.input_file;
            config.use_custom_board = true;
            config.game_count = 1;
            config.time_per_move_ms = 1000;
            config.enable_random_opening = false;
            config.random_opening_moves = 0;
            config.step_by_step = true;
            config.save_games = false;
            config.output_file = "custom_game.txt";
            break;
        default:
            std::cout << "\n无效选项，使用默认配置" << std::endl;
            config.game_count = 5;
            config.time_per_move_ms = 1000;
            config.enable_random_opening = true;
            config.random_opening_moves = 1;
            config.step_by_step = false;
            config.output_file = "test_default.txt";
            break;
    }

    config.save_games = true;

    std::cout << "配置: " << config.game_count << "局, " << config.time_per_move_ms << "ms/步, "
              << config.random_opening_moves << "步随机开局" << std::endl;
    std::cout << "输出文件: " << config.output_file << std::endl;

    // 创建自博弈管理器并运行
    VanitasBot::SearchEngine::SelfPlayManager manager(config);
    manager.runSelfPlay();

    std::cout << "\n自博弈完成！" << std::endl;

    return 0;
}
#endif
