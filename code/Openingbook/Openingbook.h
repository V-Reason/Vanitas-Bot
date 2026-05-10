#pragma once
#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

#include "BitEngine/BitEngine.h"
#include "HashEngine/HashEngine.h"

#include <array>

namespace VanitasBot::OpeningBook {

constexpr int MAX_OPENING_MOVES = 10;
constexpr int MAX_OPENING_LINES = 32;

struct OpeningLine {
    std::array<BitEngine::Move, MAX_OPENING_MOVES> moves;
    int moveCount;
};

extern OpeningLine openingLines[MAX_OPENING_LINES];
extern int openingLineCount;

void init();

BitEngine::Move lookup(const BitEngine::BitBoard& board, HashEngine::Key hash);

}  // namespace VanitasBot::OpeningBook

#endif
