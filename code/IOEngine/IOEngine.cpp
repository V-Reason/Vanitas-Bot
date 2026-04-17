#include "IOEngine.h"

#include <cstdio>

namespace VanitasBot::IOEngine {

IOEngine::IOEngine(): historyTop(0) {
    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);
}

void IOEngine::initBoard(BitEngine::BitBoard& board) {
    board.blacks = 0;
    board.whites = 0;
    board.arrows = 0;
    board.player = BitEngine::Player::WHITE;

    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(2, 0)));
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(5, 0)));
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(0, 2)));
    BitEngine::setBit(board.blacks, BitEngine::makeMask(BitEngine::XYToIndex(7, 2)));

    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(0, 5)));
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(7, 5)));
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(2, 7)));
    BitEngine::setBit(board.whites, BitEngine::makeMask(BitEngine::XYToIndex(5, 7)));
}

void IOEngine::readInputAndRecover(BitEngine::BitBoard& board) {
    int turnID;
    scanf("%d", &turnID);

    int x0, y0, x1, y1, x2, y2;
    int loopCnt = 2 * turnID - 1;

    for (int i = 0; i < loopCnt; i++) {
        scanf("%d %d %d %d %d %d", &x0, &y0, &x1, &y1, &x2, &y2);
        if (x0 == -1) {
            board.player = BitEngine::Player::BLACK;
        } else {
            BitEngine::Move move = BitEngine::makeMove(BitEngine::XYToIndex(x0, y0), BitEngine::XYToIndex(x1, y1),
                                                       BitEngine::XYToIndex(x2, y2));
            pushHistory(board, move);
            BitEngine::applyMove(board, move);
        }
    }
}

void IOEngine::outputAnswer(BitEngine::Move chosenMove) {
    int fromX, fromY, toX, toY, arrowX, arrowY;

    BitEngine::indexToXY(BitEngine::getFrom(chosenMove), fromX, fromY);
    BitEngine::indexToXY(BitEngine::getTo(chosenMove), toX, toY);
    BitEngine::indexToXY(BitEngine::getArrow(chosenMove), arrowX, arrowY);

    printf("%d %d %d %d %d %d\n", fromX, fromY, toX, toY, arrowX, arrowY);
}

void IOEngine::pushHistory(const BitEngine::BitBoard& board, BitEngine::Move move) {
    if (historyTop < MAX_HISTORY) {
        historyStack[historyTop].board = board;
        historyStack[historyTop].move = move;
        historyTop++;
    }
}

void IOEngine::popHistory(BitEngine::BitBoard& board) {
    if (historyTop > 0) {
        historyTop--;
        board = historyStack[historyTop].board;
    }
}

void IOEngine::rollbackToTurn(BitEngine::BitBoard& board, int turn) {
    if (turn < 0 || turn >= historyTop) {
        return;
    }

    historyTop = turn;
    board = historyStack[historyTop].board;
}

void IOEngine::clearHistory() {
    historyTop = 0;
}

int IOEngine::getHistoryDepth() const {
    return historyTop;
}

}  // namespace VanitasBot::IOEngine
