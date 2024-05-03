#include "Move.h"
#include <cmath>

Move::Move(int start_, int end_, int piece_, int flags_) {
    if(start_ < 0 || start_ > 63 || end_ < 0 || end_ > 63){
        throw std::invalid_argument("Invalid start or end square: " + std::to_string(start_) + "->" + std::to_string(end_));
    }
    start = start_;
    end = end_;
    piece = piece_;
    flags = flags_;
}

std::string Move::lan_str() const {
  if(Move::is_castle_kingside(flags)){
    return "O-O";
  } else if(Move::is_castle_queenside(flags)){
    return "O-O-O";
  }

  std::string lan_str = "";
  // Convert the start and end squares to algebraic notation
  std::string start_square = std::string(1, 'a' + (start % 8)) + std::to_string(8 - start / 8);
  std::string end_square = std::string(1, 'a' + (end % 8)) + std::to_string(8 - end / 8);

  lan_str += start_square;
  if (Move::is_capture(flags) || Move::is_en_passant(flags)) {
    lan_str += 'x';
  } else {
    lan_str += '-'; 
  }
  lan_str += end_square;

  if(Piece::piece_type(piece) != Piece::Pawn){
    lan_str = Piece::get_piece_short(piece) + lan_str;
  } else if(Move::is_promotion_queen(flags)){
    lan_str += "=Q";
  } else if(Move::is_promotion_rook(flags)){
    lan_str += "=R";
  } else if(Move::is_promotion_bishop(flags)){
    lan_str += "=B";
  } else if(Move::is_promotion_knight(flags)){
    lan_str += "=N";
  } else if(Move::is_en_passant(flags)){
    lan_str += " e.p";
  }

  return lan_str;
}

std::array<int, 4> Move::parse_lan(const std::string &lan_str) {
  if(lan_str == "O-O"){
    // start square, end square, piece, flags
    return {-1, -1, Piece::King, Move::CASTLE_KINGSIDE};
  }

  if(lan_str == "O-O-O"){
    return {-1, -1, Piece::King, Move::CASTLE_QUEENSIDE};
  }

  if(lan_str.length() < 5){
    throw std::invalid_argument("Invalid move: " + lan_str);
  }

  int start_ind = 1;
  int piece = Piece::NonePiece;
  int flags = 0;

  if(toupper(lan_str[0]) != lan_str[0]){
    // here we have a pawn
    start_ind = 0;
    piece = Piece::Pawn;
    // check for promotion
    if(lan_str.length() > 6 && lan_str[5] == '='){
      if(lan_str[6] == 'Q'){
        flags = Move::PROMOTION_QUEEN;
      } else if(lan_str[6] == 'R'){
        flags = Move::PROMOTION_ROOK;
      } else if(lan_str[6] == 'B'){
        flags = Move::PROMOTION_BISHOP;
      } else if(lan_str[6] == 'N'){
        flags = Move::PROMOTION_KNIGHT;
      }
    }

    if(std::abs(lan_str[4] - lan_str[1]) == 2 ){
      flags = Move::DOUBLE_PUSH;
    }

    if(lan_str.find("e.p") != std::string::npos){
      flags = Move::EN_PASSANT;
    }

  } else {
    if(toupper(lan_str[0]) == 'N'){
      piece = Piece::Knight;
    } else if(toupper(lan_str[0]) == 'B'){
      piece = Piece::Bishop;
    } else if(toupper(lan_str[0]) == 'R'){
      piece = Piece::Rook;
    } else if(toupper(lan_str[0]) == 'Q'){
      piece = Piece::Queen;
    } else if(toupper(lan_str[0]) == 'K'){
      piece = Piece::King;
    } else {
      throw std::invalid_argument("Invalid piece: " + lan_str[0]);
    }
  }

  if(lan_str[start_ind + 1] < '1' || lan_str[start_ind + 1] > '8' ||
     lan_str[start_ind + 4] < '1' || lan_str[start_ind + 4] > '8' ||
     lan_str[start_ind] < 'a' || lan_str[start_ind] > 'h' ||
     lan_str[start_ind + 3] < 'a' || lan_str[start_ind + 3] > 'h') {
    throw std::invalid_argument("Invalid move: " + lan_str);
  }

  int start_square = (8 - (lan_str[start_ind + 1] - '0')) * 8 + (lan_str[start_ind] - 'a');
  int end_square = (8 - (lan_str[start_ind + 4] - '0')) * 8 + (lan_str[start_ind + 3] - 'a');

  if(lan_str[start_ind + 2] == 'x' && Move::is_en_passant(flags) == false) {
    flags |= Move::CAPTURE;
  }
  
  if(flags == 0)
    flags = Move::NORMAL;

  return {start_square, end_square, piece, flags};
}

// string used for perft testing
// can be found in tests/PerftGenerationTest.cpp
std::string Move::perft_str() const {
  std::string perft_str = std::string(1, 'a' + (start % 8)) + std::to_string(8 - start / 8);
  perft_str += std::string(1, 'a' + (end % 8)) + std::to_string(8 - end / 8);
  
  if (Move::is_promotion_queen(flags)) {
    perft_str += '=Q';
  } else if (Move::is_promotion_rook(flags)) {
    perft_str += '=R';
  } else if (Move::is_promotion_bishop(flags)) {
    perft_str += '=B';
  } else if (Move::is_promotion_knight(flags)) {
    perft_str += '=N';
  }

  return perft_str;
}

bool Move::is_normal(int flags) {
  return (flags & NORMAL) == NORMAL;
}
bool Move::is_capture(int flags) {
  return (flags & CAPTURE) == CAPTURE;
}
bool Move::is_en_passant(int flags) {
  return (flags & EN_PASSANT) == EN_PASSANT;
}
bool Move::is_double_push(int flags) {
  return (flags & DOUBLE_PUSH) == DOUBLE_PUSH;
}
bool Move::is_castle_kingside(int flags) {
  return (flags & CASTLE_KINGSIDE) == CASTLE_KINGSIDE;
}
bool Move::is_castle_queenside(int flags) {
  return (flags & CASTLE_QUEENSIDE) == CASTLE_QUEENSIDE;
}
bool Move::is_castle(int flags) {
  return is_castle_kingside(flags) || is_castle_queenside(flags);
}
bool Move::is_promotion_queen(int flags) {
  return (flags & PROMOTION_QUEEN) == PROMOTION_QUEEN;
}
bool Move::is_promotion_rook(int flags) {
  return (flags & PROMOTION_ROOK) == PROMOTION_ROOK;
}
bool Move::is_promotion_bishop(int flags) {
  return (flags & PROMOTION_BISHOP) == PROMOTION_BISHOP;
}
bool Move::is_promotion_knight(int flags) {
  return (flags & PROMOTION_KNIGHT) == PROMOTION_KNIGHT;
}
bool Move::is_promotion(int flags) {
  return is_promotion_queen(flags) || is_promotion_rook(flags) || is_promotion_bishop(flags) || is_promotion_knight(flags);
}

// overloaded operators
std::ostream& operator<<(std::ostream &os, const Move &move) {
  if(Piece::colour(move.piece) == Piece::White){
    os << "W: ";
  } else {
    os << "B: ";
  };
  os << move.lan_str();
  return os;
}

bool Move::operator==(const Move &other) const {
  return start == other.start && end == other.end && piece == other.piece && flags == other.flags;
}
bool Move::operator!=(const Move &other) const {
  return !(*this == other);
}
bool Move::operator<(const Move &other) const {
  if (start < other.start) {
    return true;
  } else if (start == other.start) {
    if (end < other.end) {
      return true;
    } else if (end == other.end) {
      if (piece < other.piece) {
        return true;
      } else if (piece == other.piece) {
        return flags < other.flags;
      }
    }
  }
  return false;
}
bool Move::operator>(const Move &other) const {
  return !(*this < other || *this == other);
}