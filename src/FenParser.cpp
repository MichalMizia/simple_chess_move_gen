#include "FenParser.h"
#include "ChessConstants.h"
#include "GameState.h"
#include <string>
#include <vector>
#include <sstream>

std::array<int, 64> FenParser::parse_board(const std::string &board_string) {
  std::array<int, 64> board;
  int index = 0;

  board.fill(0);

  for(char c : board_string) {
    if(c == '/') {
      continue;
    }

    if(c >= '1' && c <= '8') {
      index += c - '0';
    } else {
      int piece = Piece::NonePiece;

      switch(c) {
        case 'p':
          piece = Piece::Black | Piece::Pawn;
          break;
        case 'n':
          piece = Piece::Black | Piece::Knight;
          break;
        case 'b':
          piece = Piece::Black | Piece::Bishop;
          break;
        case 'r':
          piece = Piece::Black | Piece::Rook;
          break;
        case 'q':
          piece = Piece::Black | Piece::Queen;
          break;
        case 'k':
          piece = Piece::Black | Piece::King;
          break;
        case 'P':
          piece = Piece::White | Piece::Pawn;
          break;
        case 'N':
          piece = Piece::White | Piece::Knight;
          break;
        case 'B':
          piece = Piece::White | Piece::Bishop;
          break;
        case 'R':
          piece = Piece::White | Piece::Rook;
          break;
        case 'Q':
          piece = Piece::White | Piece::Queen;
          break;
        case 'K':
          piece = Piece::White | Piece::King;
          break;
        default:
          throw std::invalid_argument("Invalid FEN string, unknown piece");
      }

      board[index] = piece;
      index++;
    }
  }

  return board;
};

GameState FenParser::parse_fen(const std::string &fen_string) {
  // split the fen into tokens
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream fen_stream(fen_string);
  
  while(std::getline(fen_stream, token, ' ')) {
    tokens.push_back(token);
  }

  if(tokens.size() != 6) {
    throw std::invalid_argument("Invalid FEN string, expected 6 space separated tokens");
  }

  std::array<int, 64> board = FenParser::parse_board(tokens[0]);

  int color = tokens[1] == "w" ? Piece::White : Piece::Black;

  char castling_rights = 0;
  for(char c : tokens[2]) {
    switch(c) {
      case 'K':
        castling_rights |= WHITE_KING_SIDE;
        break;
      case 'Q':
        castling_rights |= WHITE_QUEEN_SIDE;
        break;
      case 'k':
        castling_rights |= BLACK_KING_SIDE;
        break;
      case 'q':
        castling_rights |= BLACK_QUEEN_SIDE;
        break;
      case '-':
        break;
      default:
        throw std::invalid_argument("Invalid FEN string, unknown castling rights");
    }
  }

  char en_passant_square = tokens[3] == "-" ? -1 : (tokens[3][0] - 'a') + (8 - (tokens[3][1] - '0')) * 8;

  int halfmove_clock = std::stoi(tokens[4]);

  int fullmove_counter = std::stoi(tokens[5]);
  
  return GameState(board, color, castling_rights, en_passant_square, halfmove_clock, fullmove_counter);
}
