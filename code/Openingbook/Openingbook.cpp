#include "Openingbook.h"

#include "BitEngine/BitEngine.h"
#include "IOEngine/IOEngine.h"

namespace VanitasBot::OpeningBook {

OpeningLine openingLines[MAX_OPENING_LINES];
int openingLineCount = 0;

inline void addLine(const std::array<BitEngine::Move, MAX_OPENING_MOVES>& moves, int count) {
    if (openingLineCount < MAX_OPENING_LINES) {
        openingLines[openingLineCount].moves = moves;
        openingLines[openingLineCount].moveCount = count;
        ++openingLineCount;
    }
}

void init() {
    using namespace BitEngine;
    openingLineCount = 0;
    // 1. 黑(2,0)→(2,3) 射(5,3)
    // 2. 白(2,7)→(6,3) 射(4,5)
    // 3. 黑(5,0)→(1,0) 射(1,5)
    // 4. 白(0,5)→(1,4) 射(2,4)
    // 5. 黑(1,0)→(6,5) 射(4,7)
    // 6. 白(6,3)→(3,0) 射(0,0)
    // 7. 黑(2,3)→(3,2) 射(1,0)
    // 8. 白(1,4)→(1,2) 射(0,3)
    // 9. 黑(6,5)→(6,6) 射(6,7)
    // 10. 白(5,7)→(3,5) 射(3,6)
    addLine({makeMove(XYToIndex(2, 0), XYToIndex(2, 3), XYToIndex(5, 3)),
             makeMove(XYToIndex(2, 7), XYToIndex(6, 3), XYToIndex(4, 5)),
             makeMove(XYToIndex(5, 0), XYToIndex(1, 0), XYToIndex(1, 5)),
             makeMove(XYToIndex(0, 5), XYToIndex(1, 4), XYToIndex(2, 4)),
             makeMove(XYToIndex(1, 0), XYToIndex(6, 5), XYToIndex(4, 7)),
             makeMove(XYToIndex(6, 3), XYToIndex(3, 0), XYToIndex(0, 0)),
             makeMove(XYToIndex(2, 3), XYToIndex(3, 2), XYToIndex(1, 0)),
             makeMove(XYToIndex(1, 4), XYToIndex(1, 2), XYToIndex(0, 3)),
             makeMove(XYToIndex(6, 5), XYToIndex(6, 6), XYToIndex(6, 7)),
             makeMove(XYToIndex(5, 7), XYToIndex(3, 5), XYToIndex(3, 6))},
            10);
    // 1. 黑(2,0)→(2,6) 射(5,3)
    // 2. 白(0,5)→(3,2) 射(3,6)
    // 3. 黑(5,0)→(4,1) 射(4,6)
    // 4. 白(7,5)→(6,4) 射(7,4)
    // 5. 黑(7,2)→(5,4) 射(5,6)
    // 6. 白(6,4)→(6,0) 射(2,4)
    // 7. 黑(0,2)→(2,2) 射(6,6)
    // 8. 白(2,7)→(0,5) 射(2,3)
    // 9. 黑(2,6)→(1,5) 射(0,4)
    // 10. 白(6,0)→(6,1) 射(6,5)
    addLine({makeMove(XYToIndex(2, 0), XYToIndex(2, 6), XYToIndex(5, 3)),
             makeMove(XYToIndex(0, 5), XYToIndex(3, 2), XYToIndex(3, 6)),
             makeMove(XYToIndex(5, 0), XYToIndex(4, 1), XYToIndex(4, 6)),
             makeMove(XYToIndex(7, 5), XYToIndex(6, 4), XYToIndex(7, 4)),
             makeMove(XYToIndex(7, 2), XYToIndex(5, 4), XYToIndex(5, 6)),
             makeMove(XYToIndex(6, 4), XYToIndex(6, 0), XYToIndex(2, 4)),
             makeMove(XYToIndex(0, 2), XYToIndex(2, 2), XYToIndex(6, 6)),
             makeMove(XYToIndex(2, 7), XYToIndex(0, 5), XYToIndex(2, 3)),
             makeMove(XYToIndex(2, 6), XYToIndex(1, 5), XYToIndex(0, 4)),
             makeMove(XYToIndex(6, 0), XYToIndex(6, 1), XYToIndex(6, 5))},
            10);
    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(0,5)→(3,2) 射(2,1)
    // 3. 黑(2,0)→(1,0) 射(1,7)
    // 4. 白(2,7)→(1,6) 射(1,1)
    // 5. 黑(1,0)→(0,1) 射(3,4)
    // 6. 白(7,5)→(7,3) 射(4,0)
    // 7. 黑(7,2)→(6,3) 射(7,2)
    // 8. 白(7,3)→(6,2) 射(6,0)
    // 9. 黑(5,0)→(5,1) 射(1,5)
    // 10. 白(1,6)→(2,5) 射(3,6)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(0, 5), XYToIndex(3, 2), XYToIndex(2, 1)),
             makeMove(XYToIndex(2, 0), XYToIndex(1, 0), XYToIndex(1, 7)),
             makeMove(XYToIndex(2, 7), XYToIndex(1, 6), XYToIndex(1, 1)),
             makeMove(XYToIndex(1, 0), XYToIndex(0, 1), XYToIndex(3, 4)),
             makeMove(XYToIndex(7, 5), XYToIndex(7, 3), XYToIndex(4, 0)),
             makeMove(XYToIndex(7, 2), XYToIndex(6, 3), XYToIndex(7, 2)),
             makeMove(XYToIndex(7, 3), XYToIndex(6, 2), XYToIndex(6, 0)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 1), XYToIndex(1, 5)),
             makeMove(XYToIndex(1, 6), XYToIndex(2, 5), XYToIndex(3, 6))},
            10);
    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(0,5)→(2,3) 射(5,6)
    // 3. 黑(2,0)→(1,1) 射(3,3)
    // 4. 白(7,5)→(5,3) 射(7,3)
    // 5. 黑(1,1)→(1,4) 射(4,7)
    // 6. 白(2,7)→(1,6) 射(3,6)
    // 7. 黑(5,0)→(6,1) 射(6,7)
    // 8. 白(5,3)→(5,2) 射(6,3)
    // 9. 黑(6,1)→(0,1) 射(1,2)
    // 10. 白(5,2)→(6,2) 射(4,4)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(0, 5), XYToIndex(2, 3), XYToIndex(5, 6)),
             makeMove(XYToIndex(2, 0), XYToIndex(1, 1), XYToIndex(3, 3)),
             makeMove(XYToIndex(7, 5), XYToIndex(5, 3), XYToIndex(7, 3)),
             makeMove(XYToIndex(1, 1), XYToIndex(1, 4), XYToIndex(4, 7)),
             makeMove(XYToIndex(2, 7), XYToIndex(1, 6), XYToIndex(3, 6)),
             makeMove(XYToIndex(5, 0), XYToIndex(6, 1), XYToIndex(6, 7)),
             makeMove(XYToIndex(5, 3), XYToIndex(5, 2), XYToIndex(6, 3)),
             makeMove(XYToIndex(6, 1), XYToIndex(0, 1), XYToIndex(1, 2)),
             makeMove(XYToIndex(5, 2), XYToIndex(6, 2), XYToIndex(4, 4))},
            10);
    // 1. 黑(2,0)→(2,5) 射(1,4)
    // 2. 白(0,5)→(0,3) 射(6,3)
    // 3. 黑(5,0)→(5,5) 射(6,4)
    // 4. 白(5,7)→(2,4) 射(4,4)
    // 5. 黑(0,2)→(2,2) 射(3,3)
    // 6. 白(0,3)→(3,0) 射(7,0)
    // 7. 黑(2,2)→(2,1) 射(4,1)
    // 8. 白(3,0)→(5,0) 射(5,4)
    // 9. 黑(2,1)→(2,2) 射(4,0)
    // 10. 白(5,0)→(6,0) 射(7,1)
    addLine({makeMove(XYToIndex(2, 0), XYToIndex(2, 5), XYToIndex(1, 4)),
             makeMove(XYToIndex(0, 5), XYToIndex(0, 3), XYToIndex(6, 3)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 5), XYToIndex(6, 4)),
             makeMove(XYToIndex(5, 7), XYToIndex(2, 4), XYToIndex(4, 4)),
             makeMove(XYToIndex(0, 2), XYToIndex(2, 2), XYToIndex(3, 3)),
             makeMove(XYToIndex(0, 3), XYToIndex(3, 0), XYToIndex(7, 0)),
             makeMove(XYToIndex(2, 2), XYToIndex(2, 1), XYToIndex(4, 1)),
             makeMove(XYToIndex(3, 0), XYToIndex(5, 0), XYToIndex(5, 4)),
             makeMove(XYToIndex(2, 1), XYToIndex(2, 2), XYToIndex(4, 0)),
             makeMove(XYToIndex(5, 0), XYToIndex(6, 0), XYToIndex(7, 1))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,6)
    // 2. 白(0,5)→(3,2) 射(2,1)
    // 3. 黑(4,6)→(2,4) 射(4,2)
    // 4. 白(3,2)→(1,4) 射(1,5)
    // 5. 黑(5,0)→(5,5) 射(6,4)
    // 6. 白(5,7)→(3,5) 射(7,1)
    // 7. 黑(2,4)→(2,3) 射(3,4)
    // 8. 白(1,4)→(1,3) 射(1,4)
    // 9. 黑(5,5)→(6,6) 射(4,4)
    // 10. 白(3,5)→(2,4) 射(2,5)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 6)),
             makeMove(XYToIndex(0, 5), XYToIndex(3, 2), XYToIndex(2, 1)),
             makeMove(XYToIndex(4, 6), XYToIndex(2, 4), XYToIndex(4, 2)),
             makeMove(XYToIndex(3, 2), XYToIndex(1, 4), XYToIndex(1, 5)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 5), XYToIndex(6, 4)),
             makeMove(XYToIndex(5, 7), XYToIndex(3, 5), XYToIndex(7, 1)),
             makeMove(XYToIndex(2, 4), XYToIndex(2, 3), XYToIndex(3, 4)),
             makeMove(XYToIndex(1, 4), XYToIndex(1, 3), XYToIndex(1, 4)),
             makeMove(XYToIndex(5, 5), XYToIndex(6, 6), XYToIndex(4, 4)),
             makeMove(XYToIndex(3, 5), XYToIndex(2, 4), XYToIndex(2, 5))},
            10);
    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(0,5)→(3,2) 射(6,2)
    // 3. 黑(2,0)→(2,2) 射(3,3)
    // 4. 白(2,7)→(2,5) 射(4,5)
    // 5. 黑(7,2)→(5,4) 射(6,4)
    // 6. 白(7,5)→(7,1) 射(1,1)
    // 7. 黑(5,0)→(6,1) 射(5,2)
    // 8. 白(2,5)→(2,4) 射(2,6)
    // 9. 黑(5,4)→(4,3) 射(3,4)
    // 10. 白(7,1)→(7,2) 射(5,4)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(0, 5), XYToIndex(3, 2), XYToIndex(6, 2)),
             makeMove(XYToIndex(2, 0), XYToIndex(2, 2), XYToIndex(3, 3)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 5), XYToIndex(4, 5)),
             makeMove(XYToIndex(7, 2), XYToIndex(5, 4), XYToIndex(6, 4)),
             makeMove(XYToIndex(7, 5), XYToIndex(7, 1), XYToIndex(1, 1)),
             makeMove(XYToIndex(5, 0), XYToIndex(6, 1), XYToIndex(5, 2)),
             makeMove(XYToIndex(2, 5), XYToIndex(2, 4), XYToIndex(2, 6)),
             makeMove(XYToIndex(5, 4), XYToIndex(4, 3), XYToIndex(3, 4)),
             makeMove(XYToIndex(7, 1), XYToIndex(7, 2), XYToIndex(5, 4))},
            10);
    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(7,5)→(4,2) 射(4,1)
    // 3. 黑(5,0)→(5,3) 射(0,3)
    // 4. 白(0,5)→(2,3) 射(2,1)
    // 5. 黑(2,0)→(3,1) 射(3,6)
    // 6. 白(4,2)→(6,2) 射(7,3)
    // 7. 黑(5,3)→(6,4) 射(6,7)
    // 8. 白(2,7)→(2,6) 射(4,4)
    // 9. 黑(7,2)→(5,0) 射(5,1)
    // 10. 白(5,7)→(5,6) 射(7,4)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(7, 5), XYToIndex(4, 2), XYToIndex(4, 1)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 3), XYToIndex(0, 3)),
             makeMove(XYToIndex(0, 5), XYToIndex(2, 3), XYToIndex(2, 1)),
             makeMove(XYToIndex(2, 0), XYToIndex(3, 1), XYToIndex(3, 6)),
             makeMove(XYToIndex(4, 2), XYToIndex(6, 2), XYToIndex(7, 3)),
             makeMove(XYToIndex(5, 3), XYToIndex(6, 4), XYToIndex(6, 7)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 6), XYToIndex(4, 4)),
             makeMove(XYToIndex(7, 2), XYToIndex(5, 0), XYToIndex(5, 1)),
             makeMove(XYToIndex(5, 7), XYToIndex(5, 6), XYToIndex(7, 4))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(2,7)→(2,1) 射(4,1)
    // 3. 黑(5,0)→(5,3) 射(3,1)
    // 4. 白(7,5)→(6,4) 射(6,2)
    // 5. 黑(2,0)→(1,1) 射(1,5)
    // 6. 白(0,5)→(1,6) 射(6,1)
    // 7. 黑(1,1)→(0,1) 射(6,7)
    // 8. 白(1,6)→(2,5) 射(5,2)
    // 9. 黑(7,2)→(7,4) 射(4,7)
    // 10. 白(5,7)→(5,6) 射(1,2)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 1), XYToIndex(4, 1)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 3), XYToIndex(3, 1)),
             makeMove(XYToIndex(7, 5), XYToIndex(6, 4), XYToIndex(6, 2)),
             makeMove(XYToIndex(2, 0), XYToIndex(1, 1), XYToIndex(1, 5)),
             makeMove(XYToIndex(0, 5), XYToIndex(1, 6), XYToIndex(6, 1)),
             makeMove(XYToIndex(1, 1), XYToIndex(0, 1), XYToIndex(6, 7)),
             makeMove(XYToIndex(1, 6), XYToIndex(2, 5), XYToIndex(5, 2)),
             makeMove(XYToIndex(7, 2), XYToIndex(7, 4), XYToIndex(4, 7)),
             makeMove(XYToIndex(5, 7), XYToIndex(5, 6), XYToIndex(1, 2))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(2,7)→(2,1) 射(5,1)
    // 3. 黑(5,0)→(1,4) 射(6,4)
    // 4. 白(7,5)→(7,3) 射(6,2)
    // 5. 黑(7,2)→(6,3) 射(7,2)
    // 6. 白(0,5)→(2,5) 射(5,2)
    // 7. 黑(6,3)→(7,4) 射(6,3)
    // 8. 白(5,7)→(5,6) 射(2,3)
    // 9. 黑(7,4)→(6,5) 射(7,4)
    // 10. 白(2,1)→(3,1) 射(3,3)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 1), XYToIndex(5, 1)),
             makeMove(XYToIndex(5, 0), XYToIndex(1, 4), XYToIndex(6, 4)),
             makeMove(XYToIndex(7, 5), XYToIndex(7, 3), XYToIndex(6, 2)),
             makeMove(XYToIndex(7, 2), XYToIndex(6, 3), XYToIndex(7, 2)),
             makeMove(XYToIndex(0, 5), XYToIndex(2, 5), XYToIndex(5, 2)),
             makeMove(XYToIndex(6, 3), XYToIndex(7, 4), XYToIndex(6, 3)),
             makeMove(XYToIndex(5, 7), XYToIndex(5, 6), XYToIndex(2, 3)),
             makeMove(XYToIndex(7, 4), XYToIndex(6, 5), XYToIndex(7, 4)),
             makeMove(XYToIndex(2, 1), XYToIndex(3, 1), XYToIndex(3, 3))},
            10);

    // 1. 黑(2,0)→(2,5) 射(4,7)
    // 2. 白(0,5)→(4,1) 射(5,1)
    // 3. 黑(0,2)→(3,2) 射(7,6)
    // 4. 白(5,7)→(2,4) 射(2,2)
    // 5. 黑(7,2)→(5,4) 射(6,5)
    // 6. 白(2,7)→(1,6) 射(1,1)
    // 7. 黑(5,4)→(5,5) 射(6,6)
    // 8. 白(7,5)→(4,2) 射(4,5)
    // 9. 黑(5,5)→(5,3) 射(5,2)
    // 10. 白(2,4)→(6,4) 射(6,0)
    addLine({makeMove(XYToIndex(2, 0), XYToIndex(2, 5), XYToIndex(4, 7)),
             makeMove(XYToIndex(0, 5), XYToIndex(4, 1), XYToIndex(5, 1)),
             makeMove(XYToIndex(0, 2), XYToIndex(3, 2), XYToIndex(7, 6)),
             makeMove(XYToIndex(5, 7), XYToIndex(2, 4), XYToIndex(2, 2)),
             makeMove(XYToIndex(7, 2), XYToIndex(5, 4), XYToIndex(6, 5)),
             makeMove(XYToIndex(2, 7), XYToIndex(1, 6), XYToIndex(1, 1)),
             makeMove(XYToIndex(5, 4), XYToIndex(5, 5), XYToIndex(6, 6)),
             makeMove(XYToIndex(7, 5), XYToIndex(4, 2), XYToIndex(4, 5)),
             makeMove(XYToIndex(5, 5), XYToIndex(5, 3), XYToIndex(5, 2)),
             makeMove(XYToIndex(2, 4), XYToIndex(6, 4), XYToIndex(6, 0))},
            10);

    // 1. 黑(5,0)→(5,5) 射(2,5)
    // 2. 白(7,5)→(5,3) 射(4,4)
    // 3. 黑(2,0)→(2,4) 射(1,4)
    // 4. 白(5,3)→(2,0) 射(7,0)
    // 5. 黑(7,2)→(6,3) 射(3,6)
    // 6. 白(5,7)→(7,5) 射(6,6)
    // 7. 黑(2,4)→(1,5) 射(3,7)
    // 8. 白(0,5)→(0,3) 射(1,2)
    // 9. 黑(6,3)→(4,3) 射(1,3)
    // 10. 白(2,0)→(1,1) 射(0,1)
    addLine({makeMove(XYToIndex(5, 0), XYToIndex(5, 5), XYToIndex(2, 5)),
             makeMove(XYToIndex(7, 5), XYToIndex(5, 3), XYToIndex(4, 4)),
             makeMove(XYToIndex(2, 0), XYToIndex(2, 4), XYToIndex(1, 4)),
             makeMove(XYToIndex(5, 3), XYToIndex(2, 0), XYToIndex(7, 0)),
             makeMove(XYToIndex(7, 2), XYToIndex(6, 3), XYToIndex(3, 6)),
             makeMove(XYToIndex(5, 7), XYToIndex(7, 5), XYToIndex(6, 6)),
             makeMove(XYToIndex(2, 4), XYToIndex(1, 5), XYToIndex(3, 7)),
             makeMove(XYToIndex(0, 5), XYToIndex(0, 3), XYToIndex(1, 2)),
             makeMove(XYToIndex(6, 3), XYToIndex(4, 3), XYToIndex(1, 3)),
             makeMove(XYToIndex(2, 0), XYToIndex(1, 1), XYToIndex(0, 1))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(0,5)→(2,3) 射(2,1)
    // 3. 黑(2,0)→(4,2) 射(2,2)
    // 4. 白(7,5)→(5,3) 射(3,5)
    // 5. 黑(7,2)→(7,4) 射(4,7)
    // 6. 白(2,7)→(2,6) 射(2,4)
    // 7. 黑(5,0)→(1,0) 射(1,5)
    // 8. 白(2,3)→(4,1) 射(2,3)
    // 9. 黑(1,0)→(1,4) 射(1,0)
    // 10. 白(4,1)→(3,1) 射(5,1)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(0, 5), XYToIndex(2, 3), XYToIndex(2, 1)),
             makeMove(XYToIndex(2, 0), XYToIndex(4, 2), XYToIndex(2, 2)),
             makeMove(XYToIndex(7, 5), XYToIndex(5, 3), XYToIndex(3, 5)),
             makeMove(XYToIndex(7, 2), XYToIndex(7, 4), XYToIndex(4, 7)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 6), XYToIndex(2, 4)),
             makeMove(XYToIndex(5, 0), XYToIndex(1, 0), XYToIndex(1, 5)),
             makeMove(XYToIndex(2, 3), XYToIndex(4, 1), XYToIndex(2, 3)),
             makeMove(XYToIndex(1, 0), XYToIndex(1, 4), XYToIndex(1, 0)),
             makeMove(XYToIndex(4, 1), XYToIndex(3, 1), XYToIndex(5, 1))},
            10);

    // 1. 黑(5,0)→(5,6) 射(1,6)
    // 2. 白(5,7)→(1,3) 射(0,3)
    // 3. 黑(2,0)→(2,3) 射(2,2)
    // 4. 白(0,5)→(3,5) 射(6,2)
    // 5. 黑(2,3)→(2,4) 射(2,6)
    // 6. 白(3,5)→(3,0) 射(3,4)
    // 7. 黑(7,2)→(6,3) 射(4,1)
    // 8. 白(3,0)→(1,0) 射(1,1)
    // 9. 黑(0,2)→(1,2) 射(3,0)
    // 10. 白(1,0)→(2,0) 射(2,1)
    addLine({makeMove(XYToIndex(5, 0), XYToIndex(5, 6), XYToIndex(1, 6)),
             makeMove(XYToIndex(5, 7), XYToIndex(1, 3), XYToIndex(0, 3)),
             makeMove(XYToIndex(2, 0), XYToIndex(2, 3), XYToIndex(2, 2)),
             makeMove(XYToIndex(0, 5), XYToIndex(3, 5), XYToIndex(6, 2)),
             makeMove(XYToIndex(2, 3), XYToIndex(2, 4), XYToIndex(2, 6)),
             makeMove(XYToIndex(3, 5), XYToIndex(3, 0), XYToIndex(3, 4)),
             makeMove(XYToIndex(7, 2), XYToIndex(6, 3), XYToIndex(4, 1)),
             makeMove(XYToIndex(3, 0), XYToIndex(1, 0), XYToIndex(1, 1)),
             makeMove(XYToIndex(0, 2), XYToIndex(1, 2), XYToIndex(3, 0)),
             makeMove(XYToIndex(1, 0), XYToIndex(2, 0), XYToIndex(2, 1))},
            10);
    // 1. 黑(5,0)→(5,6) 射(2,3)
    // 2. 白(7,5)→(3,1) 射(4,2)
    // 3. 黑(0,2)→(2,4) 射(0,4)
    // 4. 白(2,7)→(5,4) 射(3,4)
    // 5. 黑(2,0)→(2,1) 射(2,2)
    // 6. 白(0,5)→(1,5) 射(2,6)
    // 7. 黑(2,4)→(2,5) 射(1,4)
    // 8. 白(3,1)→(4,1) 射(6,1)
    // 9. 黑(5,6)→(6,6) 射(4,6)
    // 10. 白(4,1)→(4,0) 射(6,2)
    addLine({makeMove(XYToIndex(5, 0), XYToIndex(5, 6), XYToIndex(2, 3)),
             makeMove(XYToIndex(7, 5), XYToIndex(3, 1), XYToIndex(4, 2)),
             makeMove(XYToIndex(0, 2), XYToIndex(2, 4), XYToIndex(0, 4)),
             makeMove(XYToIndex(2, 7), XYToIndex(5, 4), XYToIndex(3, 4)),
             makeMove(XYToIndex(2, 0), XYToIndex(2, 1), XYToIndex(2, 2)),
             makeMove(XYToIndex(0, 5), XYToIndex(1, 5), XYToIndex(2, 6)),
             makeMove(XYToIndex(2, 4), XYToIndex(2, 5), XYToIndex(1, 4)),
             makeMove(XYToIndex(3, 1), XYToIndex(4, 1), XYToIndex(6, 1)),
             makeMove(XYToIndex(5, 6), XYToIndex(6, 6), XYToIndex(4, 6)),
             makeMove(XYToIndex(4, 1), XYToIndex(4, 0), XYToIndex(6, 2))},
            10);

    // 1. 黑(2,0)→(2,6) 射(0,4)
    // 2. 白(0,5)→(2,3) 射(0,1)
    // 3. 黑(7,2)→(5,4) 射(5,6)
    // 4. 白(5,7)→(2,4) 射(4,4)
    // 5. 黑(0,2)→(2,2) 射(3,3)
    // 6. 白(2,3)→(4,1) 射(1,1)
    // 7. 黑(5,0)→(5,2) 射(5,0)
    // 8. 白(4,1)→(3,0) 射(1,2)
    // 9. 黑(2,2)→(3,1) 射(4,1)
    // 10. 白(7,5)→(6,4) 射(6,1)
    addLine({makeMove(XYToIndex(2, 0), XYToIndex(2, 6), XYToIndex(0, 4)),
             makeMove(XYToIndex(0, 5), XYToIndex(2, 3), XYToIndex(0, 1)),
             makeMove(XYToIndex(7, 2), XYToIndex(5, 4), XYToIndex(5, 6)),
             makeMove(XYToIndex(5, 7), XYToIndex(2, 4), XYToIndex(4, 4)),
             makeMove(XYToIndex(0, 2), XYToIndex(2, 2), XYToIndex(3, 3)),
             makeMove(XYToIndex(2, 3), XYToIndex(4, 1), XYToIndex(1, 1)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 2), XYToIndex(5, 0)),
             makeMove(XYToIndex(4, 1), XYToIndex(3, 0), XYToIndex(1, 2)),
             makeMove(XYToIndex(2, 2), XYToIndex(3, 1), XYToIndex(4, 1)),
             makeMove(XYToIndex(7, 5), XYToIndex(6, 4), XYToIndex(6, 1))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(2,7)→(2,1) 射(4,1)
    // 3. 黑(5,0)→(5,3) 射(3,1)
    // 4. 白(7,5)→(6,4) 射(6,1)
    // 5. 黑(2,0)→(1,1) 射(0,2)
    // 6. 白(6,4)→(6,2) 射(6,4)
    // 7. 黑(1,1)→(1,0) 射(1,4)
    // 8. 白(0,5)→(3,5) 射(3,6)
    // 9. 黑(5,3)→(5,1) 射(4,0)
    // 10. 白(2,1)→(1,2) 射(4,2)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 1), XYToIndex(4, 1)),
             makeMove(XYToIndex(5, 0), XYToIndex(5, 3), XYToIndex(3, 1)),
             makeMove(XYToIndex(7, 5), XYToIndex(6, 4), XYToIndex(6, 1)),
             makeMove(XYToIndex(2, 0), XYToIndex(1, 1), XYToIndex(0, 2)),
             makeMove(XYToIndex(6, 4), XYToIndex(6, 2), XYToIndex(6, 4)),
             makeMove(XYToIndex(1, 1), XYToIndex(1, 0), XYToIndex(1, 4)),
             makeMove(XYToIndex(0, 5), XYToIndex(3, 5), XYToIndex(3, 6)),
             makeMove(XYToIndex(5, 3), XYToIndex(5, 1), XYToIndex(4, 0)),
             makeMove(XYToIndex(2, 1), XYToIndex(1, 2), XYToIndex(4, 2))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(7,5)→(3,1) 射(6,1)
    // 3. 黑(2,0)→(2,3) 射(2,5)
    // 4. 白(2,7)→(6,3) 射(3,6)
    // 5. 黑(7,2)→(7,4) 射(4,7)
    // 6. 白(5,7)→(6,6) 射(6,4)
    // 7. 黑(4,6)→(4,4) 射(0,4)
    // 8. 白(3,1)→(3,2) 射(3,5)
    // 9. 黑(5,0)→(6,0) 射(1,5)
    // 10. 白(6,6)→(5,6) 射(4,5)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(7, 5), XYToIndex(3, 1), XYToIndex(6, 1)),
             makeMove(XYToIndex(2, 0), XYToIndex(2, 3), XYToIndex(2, 5)),
             makeMove(XYToIndex(2, 7), XYToIndex(6, 3), XYToIndex(3, 6)),
             makeMove(XYToIndex(7, 2), XYToIndex(7, 4), XYToIndex(4, 7)),
             makeMove(XYToIndex(5, 7), XYToIndex(6, 6), XYToIndex(6, 4)),
             makeMove(XYToIndex(4, 6), XYToIndex(4, 4), XYToIndex(0, 4)),
             makeMove(XYToIndex(3, 1), XYToIndex(3, 2), XYToIndex(3, 5)),
             makeMove(XYToIndex(5, 0), XYToIndex(6, 0), XYToIndex(1, 5)),
             makeMove(XYToIndex(6, 6), XYToIndex(5, 6), XYToIndex(4, 5))},
            10);

    // 1. 黑(0,2)→(4,6) 射(5,5)
    // 2. 白(2,7)→(2,3) 射(6,3)
    // 3. 黑(2,0)→(1,1) 射(1,5)
    // 4. 白(7,5)→(5,3) 射(6,2)
    // 5. 黑(7,2)→(7,4) 射(4,7)
    // 6. 白(5,7)→(7,5) 射(6,4)
    // 7. 黑(1,1)→(2,1) 射(2,0)
    // 8. 白(5,3)→(5,1) 射(4,0)
    // 9. 黑(2,1)→(0,1) 射(0,4)
    // 10. 白(0,5)→(2,7) 射(3,6)
    addLine({makeMove(XYToIndex(0, 2), XYToIndex(4, 6), XYToIndex(5, 5)),
             makeMove(XYToIndex(2, 7), XYToIndex(2, 3), XYToIndex(6, 3)),
             makeMove(XYToIndex(2, 0), XYToIndex(1, 1), XYToIndex(1, 5)),
             makeMove(XYToIndex(7, 5), XYToIndex(5, 3), XYToIndex(6, 2)),
             makeMove(XYToIndex(7, 2), XYToIndex(7, 4), XYToIndex(4, 7)),
             makeMove(XYToIndex(5, 7), XYToIndex(7, 5), XYToIndex(6, 4)),
             makeMove(XYToIndex(1, 1), XYToIndex(2, 1), XYToIndex(2, 0)),
             makeMove(XYToIndex(5, 3), XYToIndex(5, 1), XYToIndex(4, 0)),
             makeMove(XYToIndex(2, 1), XYToIndex(0, 1), XYToIndex(0, 4)),
             makeMove(XYToIndex(0, 5), XYToIndex(2, 7), XYToIndex(3, 6))},
            10);
}

BitEngine::Move lookup(const BitEngine::BitBoard& board, HashEngine::Key hash) {
    int halfMove = (board.player == BitEngine::Player::BLACK) ? 0 : 1;

    for (int i = 0; i < openingLineCount; ++i) {
        const auto& line = openingLines[i];
        BitEngine::BitBoard simBoard;
        IOEngine::initBoard(simBoard);
        HashEngine::Key simHash = HashEngine::generateHash(simBoard);

        for (int step = 0; step < line.moveCount; ++step) {
            int idx = step * 2 + halfMove;
            if (idx >= line.moveCount || line.moves[idx] == 0)
                break;

            if (simHash == hash) {
                return line.moves[idx];
            }

            BitEngine::Move m = line.moves[idx];
            simHash = HashEngine::updataHash(simHash,
                                             (halfMove == 0) ? HashEngine::Element::PLAYER_BLACK
                                                             : HashEngine::Element::PLAYER_WHITE,
                                             BitEngine::getFrom(m),
                                             BitEngine::getTo(m),
                                             BitEngine::getArrow(m));
            BitEngine::applyMove(simBoard, m);
        }
    }

    return 0;
}

}  // namespace VanitasBot::OpeningBook
