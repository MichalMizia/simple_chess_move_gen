#include "FenParser.h"
#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "ChessConstants.h"

typedef unsigned long long int u_long64_t;

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
std::string int_to_bin_string(u_long64_t n) {
    std::string result = "";
    while (n > 0) {
        result = (n % 2 == 0 ? "0" : "1") + result;
        n /= 2;
    }
    if(result.size() < 64){
        result = std::string(64 - result.size(), '0') + result;
    }
    return "0b" + result;
}

void append_to_file(std::vector<u_long64_t> &lookup_table, std::string name) {
    std::ofstream file("../lookup.txt", std::ios::app);
    file << "const u_long64_t " << name << "[64] = {" << std::endl;
    for (auto &entry : lookup_table) {
        file << int_to_bin_string(entry) << "," << std::endl;
    }
    file << "};\n" << std::endl;
    file.close();
}

int main() {
    std::ofstream file("../lookup.txt");
    file.clear();
    file.close();
    GameState game = FenParser::parse_fen(STARTING_FEN);
    game.board.fill(0);
    std::string input;
    // for horsies
    std::vector<u_long64_t> knight_lookup(64, 0);
    for(int i = 0; i < 64; i++){
        game.board[i] = Piece::Knight | Piece::White;
        std::vector<Move> moves = game.generate_knight_moves(i);
        for(auto &move : moves){
            knight_lookup[i] |= (1ULL << move.end);
        }
        game.board[i] = 0;
    }
    append_to_file(knight_lookup, "knight_lookup");

    // for bishops
    std::vector<u_long64_t> bishop_lookup(64, 0);
    for(int i = 0; i < 64; i++){
        game.board[i] = Piece::Bishop | Piece::White;
        std::vector<Move> moves = game.generate_diagonal_sliding_moves(i);
        for(auto &move : moves){
            bishop_lookup[i] |= (1ULL << move.end);
        }
        game.board[i] = 0;
    }
    append_to_file(bishop_lookup, "bishop_lookup");

    // for rooks
    std::vector<u_long64_t> rook_lookup(64, 0);
    for(int i = 0; i < 64; i++){
        game.board[i] = Piece::Rook | Piece::White;
        std::vector<Move> moves = game.generate_straight_sliding_moves(i);
        for(auto &move : moves){
            rook_lookup[i] |= (1ULL << move.end);
        }
        game.board[i] = 0;
    }
    append_to_file(rook_lookup, "rook_lookup");

    return 0;
}