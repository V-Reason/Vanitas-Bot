#pragma once

#ifndef SELF_PLAY_H
#define SELF_PLAY_H

#include "../BitEngine/BitEngine.h"

#include <string>
#include <vector>

namespace VanitasBot::SearchEngine {

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
};

class SelfPlayManager {
   public:
    struct GameResult {
        std::vector<BitEngine::Move> moves;
        BitEngine::Player winner;
        bool game_interrupted;  // 表示游戏是否因超时等中断
        int game_length;
        bool is_draw;
    };

   private:
    SelfPlayConfig config_;

   public:
    explicit SelfPlayManager(const SelfPlayConfig& config = SelfPlayConfig{});

    // 从文件读取特定局面
    bool loadCustomBoard(const std::string& filename);

    // 生成初始棋盘
    BitEngine::BitBoard generateInitialBoard();

    // 随机选择开局走法
    void applyRandomOpening(BitEngine::BitBoard& board, int num_moves);

    // 执行单局游戏
    GameResult playSingleGame();

    // 运行多局自博弈
    void runSelfPlay();

    // 获取当前配置
    const SelfPlayConfig& getConfig() const;

    // 设置配置
    void setConfig(const SelfPlayConfig& config);
};

// 便捷函数：运行自博弈
void runSelfPlayGames(int game_count = 100, int time_per_move_ms = 1000);

}  // namespace VanitasBot::SearchEngine

#endif
