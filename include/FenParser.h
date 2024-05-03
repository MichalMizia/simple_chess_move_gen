class GameState;  // Forward declaration

#include "../src/Piece.cpp"
#include <array>

#pragma once

class FenParser {
public:
  static std::array<int, 64> parse_board(const std::string &fen_string);
  static GameState parse_fen(const std::string &fen_string);
};