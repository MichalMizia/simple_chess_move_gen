#include "FenParser.h"
#include "GameState.h"
#include "Game.h"

// board is going from 0 in the top left corner where the black pieces are
// to 63 in the bottom right corner where the white pieces are
// 0  1  2  3  4  5  6  7
// 8  9  10 11 12 13 14 15
// 16 17 18 19 20 21 22 23
// 24 25 26 27 28 29 30 31
// 32 33 34 35 36 37 38 39
// 40 41 42 43 44 45 46 47
// 48 49 50 51 52 53 54 55
// 56 57 58 59 60 61 62 63

int main() {
    Game game = Game();
    // Game game2 = Game("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    game.play();

    return 0;
}