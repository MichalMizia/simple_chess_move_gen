#include "../include/GameState.h"
#include "../include/ChessConstants.h"
#include "../include/FenParser.h"
#include "gtest/gtest.h"
#include <fstream>
#include <sstream>  

std::vector<Move> moves_from_ulong(int square, u_long64_t bitmask, int piece){
    std::vector<Move> moves;
    for(int i = 0; i < 64; i++){
        if(bitmask & (1ULL << i)){
            moves.push_back(Move(square, i, piece, Move::NORMAL));
        }
    }
    return moves;
}

TEST(NewGenTest, BishopTest) {
    GameState game = FenParser::parse_fen(STARTING_FEN);
    game.board.fill(0);
    std::string input;
    // for bishops
    for(int i = 0; i < 64; i++){
        game.board[i] = Piece::Bishop | Piece::White;
        std::vector<Move> moves = game.generate_diagonal_sliding_moves(i);
        std::for_each(moves.begin(), moves.end(), [](Move &move){
            move.flags = Move::NORMAL;
        });

        std::vector<Move> moves2 = moves_from_ulong(i, ChessConstants::bishop_lookup[i], Piece::Bishop | Piece::White);
        
        std::sort(moves.begin(), moves.end());
        std::sort(moves2.begin(), moves2.end());
        
        ASSERT_EQ(moves, moves2);
        
        game.board[i] = 0;
    }
}
TEST(NewGenTest, KnightTest) {
    GameState game = FenParser::parse_fen(STARTING_FEN);
    game.board.fill(0);
    std::string input;
    // for knights
    for(int i = 0; i < 64; i++){
        game.board[i] = Piece::Knight | Piece::White;
        std::vector<Move> moves = game.generate_knight_moves(i);
        std::for_each(moves.begin(), moves.end(), [](Move &move){
            move.flags = Move::NORMAL;
        });

        std::vector<Move> moves2 = moves_from_ulong(i, ChessConstants::knight_lookup[i], Piece::Knight | Piece::White);

        std::sort(moves.begin(), moves.end());
        std::sort(moves2.begin(), moves2.end());
        
        ASSERT_EQ(moves, moves2);
        
        game.board[i] = 0;
    }
}

TEST(NewGenTest, RookTest) {
    GameState game = FenParser::parse_fen(STARTING_FEN);
    game.board.fill(0);
    std::string input;
    // for rooks
    for(int i = 0; i < 64; i++){
        game.board[i] = Piece::Rook | Piece::White;
        std::vector<Move> moves = game.generate_straight_sliding_moves(i);
        std::for_each(moves.begin(), moves.end(), [](Move &move){
            move.flags = Move::NORMAL;
        });

        std::vector<Move> moves2 = moves_from_ulong(i, ChessConstants::rook_lookup[i], Piece::Rook | Piece::White);

        std::sort(moves.begin(), moves.end());
        std::sort(moves2.begin(), moves2.end());
        
        ASSERT_EQ(moves, moves2);
        
        game.board[i] = 0;
    }
};