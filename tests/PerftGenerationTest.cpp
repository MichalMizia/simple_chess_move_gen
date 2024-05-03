#include "../include/GameState.h"
#include "../include/ChessConstants.h"
#include "../include/FenParser.h"
#include "gtest/gtest.h"
#include <fstream>
#include <sstream>  

class Perft {
public:
    static uint64_t perft(int depth, const std::string &fen = STARTING_FEN) {
        std::map<std::string, int> all_moves = {};
        if (depth == 0) {
            return 1;
        }

        GameState game = FenParser::parse_fen(fen);

        uint64_t nodes = 0;
        std::vector<Move> moves = game.get_legal_moves();

         for (int i = 0; i < moves.size(); i++) {
                
            game.make_move(moves[i]);
            int curr_moves = _perft(depth - 1, game);
            nodes += curr_moves;
            game.undo_move();

            all_moves.insert({moves[i].perft_str(), curr_moves});
        }

        // write moves to file
        std::ofstream file("perft.txt");
        for (auto &move : all_moves) {
            file << move.first << " " << move.second << std::endl;
        }
        return nodes;
    };

    static uint64_t _perft(int depth, GameState &game) {
        if (depth == 0) {
            return 1;
        }

        uint64_t nodes = 0;
        std::vector<Move> moves = game.get_legal_moves();
        for (int i = 0; i < moves.size(); i++) {
            game.make_move(moves[i]);
            nodes += _perft(depth - 1, game);
            game.undo_move();
        }

        return nodes;
    }
};

// Test case for move generation
TEST(GameStateTest, MoveGeneration) {
    GameState game = FenParser::parse_fen(STARTING_FEN);
    std::vector<Move> moves = game.get_legal_moves();
    ASSERT_EQ(moves.size(), 20);
    ASSERT_TRUE(game.turn == Piece::White);
    ASSERT_EQ(game.castling_rights, WHITE_KING_SIDE | WHITE_QUEEN_SIDE | BLACK_KING_SIDE | BLACK_QUEEN_SIDE);
    ASSERT_EQ(game.en_passant_target, -1);
    ASSERT_EQ(game.halfmove_clock, 0);
    ASSERT_EQ(game.fullmove_counter, 1);
    ASSERT_EQ(game.black_king_square, 4);
    ASSERT_EQ(game.white_king_square, 60);
}

// All these tests pass but they are very slow
// uncomment at your own risk
TEST(PerftTest, Perft5) {
    ASSERT_EQ(Perft::perft(1), 20);
    ASSERT_EQ(Perft::perft(2), 400);
    // ASSERT_EQ(Perft::perft(3), 8902);
    // ASSERT_EQ(Perft::perft(4), 197281);
    // ASSERT_EQ(Perft::perft(5), 4865609);
}

// TEST(PerftTest, PerftSetFen1) {
//     ASSERT_EQ(Perft::perft(1, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"), 44);
//     ASSERT_EQ(Perft::perft(2, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"), 1486);
//     ASSERT_EQ(Perft::perft(3, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"), 62379);
// }

// TEST(PerftTest, PerftSetFen2) {
//     ASSERT_EQ(Perft::perft(1, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), 48);
//     ASSERT_EQ(Perft::perft(2, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), 2039);
//     ASSERT_EQ(Perft::perft(3, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), 97862);
// }

TEST(PerftTest, PerftSetFen3Endgame) {
    ASSERT_EQ(Perft::perft(1, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"), 14);
    ASSERT_EQ(Perft::perft(2, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"), 191);
    ASSERT_EQ(Perft::perft(3, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"), 2812);
    ASSERT_EQ(Perft::perft(4, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"), 43238);
}

TEST(PerftTest, PerftSetFen4Midgame) {
    ASSERT_EQ(Perft::perft(1, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"), 6);
    ASSERT_EQ(Perft::perft(2, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"), 264);
    ASSERT_EQ(Perft::perft(3, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"), 9467);
    ASSERT_EQ(Perft::perft(4, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"), 422333);
}

// TEST(PerftTest, PerftSetFen5Midgame) {
//     ASSERT_EQ(Perft::perft(1, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "), 46);
//     ASSERT_EQ(Perft::perft(2, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "), 2079);
//     ASSERT_EQ(Perft::perft(3, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "), 89890);
//     ASSERT_EQ(Perft::perft(4, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "), 3894594);
// }