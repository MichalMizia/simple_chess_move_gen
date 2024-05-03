#include "GameState.h"
#include "FenParser.h"
#include "ChessConstants.h"
#include <algorithm>
#include <sstream>

GameState::GameState(const std::array<int, 64> &board, int turn, char castling_rights, char en_passant_square, int halfmove_clock, int fullmove_counter) {
  this->board = board;
  this->turn = turn;
  this->castling_rights = castling_rights;
  this->en_passant_target = en_passant_square;
  this->halfmove_clock = halfmove_clock;
  this->fullmove_counter = fullmove_counter;

  for(int i = 0; i < 64; i++) {
    if(board[i] == (Piece::White | Piece::King)) {
      this->white_king_square = i;
    } else if(board[i] == (Piece::Black | Piece::King)) {
      this->black_king_square = i;
    }
  }

  if(this->white_king_square == -1 || this->black_king_square == -1) {
    throw std::invalid_argument("Invalid board, missing king");
  }

  legal_moves = generate_legal_moves(turn);
}

GameState::GameState() {
  this->board = FenParser::parse_board(STARTING_FEN_BOARD);
  this->turn = Piece::White;
  this->castling_rights = WHITE_KING_SIDE | WHITE_QUEEN_SIDE | BLACK_KING_SIDE | BLACK_QUEEN_SIDE;
  this->en_passant_target = NO_EN_PASSANT;
  this->white_king_square = 60;
  this->black_king_square = 4;
  this->halfmove_clock = 0;
  this->fullmove_counter = 1;

  legal_moves = generate_legal_moves(turn);
}

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

std::vector<Move> GameState::generate_legal_moves(char color) {
  std::vector<Move> legal_moves;
  std::vector<Move> king_moves;
  for (int i = 0; i < 64; i++) {
    if (this->board[i] == 0 || Piece::colour(this->board[i]) != color){
      continue;
    }
    // loop over all squares, if there is our own piece on a square, generate
    // all psuedolegal moves for that piece, then remove illegal ones at the end.

    // generate pawn moves
    if (Piece::piece_type(this->board[i]) == Piece::Pawn) {
      std::vector<Move> temp = generate_pawn_moves(i);
      legal_moves.insert(legal_moves.end(), temp.begin(), temp.end());
    } else if(Piece::piece_type(this->board[i]) == Piece::Knight) {
      // generate knight moves
      std::vector<Move> temp = generate_knight_moves(i);
      legal_moves.insert(legal_moves.end(), temp.begin(), temp.end());
    } else if(Piece::is_rook_or_queen(this->board[i])) {
      // generate straight sliding moves
      std::vector<Move> temp = generate_straight_sliding_moves(i);
      legal_moves.insert(legal_moves.end(), temp.begin(), temp.end());
    } else if(Piece::piece_type(this->board[i]) == Piece::King) {
      // generate king moves
      king_moves = generate_king_moves(i);
     
      king_moves.erase(std::remove_if(king_moves.begin(), king_moves.end(), [this](const Move &move) {
        return !is_king_move_legal(move);
      }), king_moves.end());
    }

    // generate diagonal sliding moves
    // not in if else because the queen works both as rook and a bishop
    if(Piece::is_bishop_or_queen(this->board[i])) {
      std::vector<Move> temp = generate_diagonal_sliding_moves(i);
      legal_moves.insert(legal_moves.end(), temp.begin(), temp.end());
    }

  }

  // erase illegal moves from psuedolegal moves
  legal_moves.erase(std::remove_if(legal_moves.begin(), legal_moves.end(), [this](const Move &move) {
    return !is_figure_move_legal(move);
  }), legal_moves.end());

  legal_moves.insert(legal_moves.end(), king_moves.begin(), king_moves.end());

  return legal_moves;
}

std::vector<Move> GameState::generate_pawn_moves(int i) {
  std::vector<Move> pawn_moves;
  int piece_col = Piece::colourMask & this->board[i];
  int piece = Piece::Pawn | piece_col;
  int opponent_col = piece_col == Piece::White ? Piece::Black : Piece::White;
  int MOVE_DIR = piece_col == Piece::White ? DIR_UP : DIR_DOWN;

  int rank = get_rank(i);  
  int file = get_file(i);

  // Single move forward
  if ((this->board[i + MOVE_DIR] == 0)) {
    // Promotion
    if ((rank == 7 && piece_col == Piece::White) || (rank == 2 && piece_col == Piece::Black)) {
      pawn_moves.push_back(Move(i, i + MOVE_DIR, piece, Move::PROMOTION_BISHOP));
      pawn_moves.push_back(Move(i, i + MOVE_DIR, piece, Move::PROMOTION_KNIGHT));
      pawn_moves.push_back(Move(i, i + MOVE_DIR, piece, Move::PROMOTION_ROOK));
      pawn_moves.push_back(Move(i, i + MOVE_DIR, piece, Move::PROMOTION_QUEEN));
    } else {
      pawn_moves.push_back(Move(i, i + MOVE_DIR, piece, Move::NORMAL));
    }
  }

  // Double move forward
  if ((piece_col == Piece::White && rank == 2) || (piece_col == Piece::Black && rank == 7)) {
    if (this->board[i + MOVE_DIR] == 0 && this->board[i + MOVE_DIR * 2] == 0) {
      pawn_moves.push_back(Move(i, i + MOVE_DIR * 2, piece, Move::DOUBLE_PUSH));
    }
  }

  // Captures
  if (file != 1 && Piece::colour(this->board[i + MOVE_DIR + DIR_LEFT]) == opponent_col) {
    if ((rank == 7 && piece_col == Piece::White) || (rank == 2 && piece_col == Piece::Black)) {
      // Promotion & capture
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_LEFT, piece, Move::PROMOTION_BISHOP | Move::CAPTURE));
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_LEFT, piece, Move::PROMOTION_KNIGHT | Move::CAPTURE));
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_LEFT, piece, Move::PROMOTION_ROOK | Move::CAPTURE));
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_LEFT, piece, Move::PROMOTION_QUEEN | Move::CAPTURE));
    } else {
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_LEFT, piece, Move::CAPTURE));
    }
  }
  if (file != 8 && Piece::colour(this->board[i + MOVE_DIR + DIR_RIGHT]) == opponent_col) {
    if ((rank == 7 && piece_col == Piece::White) || (rank == 2 && piece_col == Piece::Black)) {
      // Promotion & capture
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_RIGHT, piece, Move::PROMOTION_BISHOP | Move::CAPTURE));
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_RIGHT, piece, Move::PROMOTION_KNIGHT | Move::CAPTURE));
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_RIGHT, piece, Move::PROMOTION_ROOK | Move::CAPTURE));
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_RIGHT, piece, Move::PROMOTION_QUEEN | Move::CAPTURE));
    } else {
      pawn_moves.push_back(Move(i, i + MOVE_DIR + DIR_RIGHT, piece, Move::CAPTURE));
    }
  }

  // En passant
  if (this->en_passant_target != -1) {
    if ((file != 1 && i + MOVE_DIR + DIR_LEFT == this->en_passant_target) ||
      (file != 8 && i + MOVE_DIR + DIR_RIGHT == this->en_passant_target)) {
      pawn_moves.push_back(Move(i, this->en_passant_target, piece, Move::EN_PASSANT));
    }
  }

  return pawn_moves;
}

std::vector<Move> GameState::generate_knight_moves(int i, char color) {
  std::vector<Move> knight_moves;
  int piece_col = Piece::colour(this->board[i]);
  if(color != 0) {
    piece_col = color;
  }
  int piece = Piece::Knight | piece_col;

  // Knight move offsets
  int offsets[8] = {-17, -10, 17, 10, -15, -6, 15, 6};
  int rank = get_rank(i);
  int file = get_file(i);
  // conditions for each move to be valid
  bool conditions[8] = {
    file > 1 && rank < 7,
    file > 2 && rank < 8,
    file < 8 && rank > 2,
    file < 7 && rank > 1,
    file < 8 && rank < 7,
    file < 7 && rank < 8,
    file > 1 && rank > 2,
    file > 2 && rank > 1
  };

  for (int j = 0; j < 8; j++) {
    if (conditions[j]) {
      if(Piece::colour(this->board[i + offsets[j]]) == 0) {
        knight_moves.push_back(Move(i, i + offsets[j], piece, Move::NORMAL));
      } else if(Piece::colour(this->board[i + offsets[j]]) != piece_col) {
        knight_moves.push_back(Move(i, i + offsets[j], piece, Move::CAPTURE));
      }
    }
  }

  return knight_moves;
}

std::vector<Move> GameState::generate_straight_sliding_moves(int i, char color) {
  std::vector<Move> sliding_moves;
  int piece = this->board[i];
  int piece_col = Piece::colour(piece);
  if(color != 0) {
    piece_col = color;
  }

  int rank = get_rank(i);

  // going left
  for(int curr_ind = i + DIR_LEFT; curr_ind >= 0; curr_ind += DIR_LEFT) {
    if(get_rank(curr_ind) != rank)
      break;

    if(Piece::colour(board[curr_ind]) == 0) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
    } else if(Piece::colour(board[curr_ind]) != piece_col) {
      // capture the opponent piece
      // and break because we can't go any further
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
      break;
    } else {
      // here we have reached our own piece
      break;
    }
  }

  // going right
  for(int curr_ind = i + DIR_RIGHT; curr_ind < 64; curr_ind += DIR_RIGHT) {
    if(get_rank(curr_ind) != rank)
      break;

    if(Piece::colour(board[curr_ind]) == 0) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
    } else if(Piece::colour(board[curr_ind]) != piece_col) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
      break;
    } else {
      break;
    }
  }

  // going up
  for(int curr_ind = i + DIR_UP; curr_ind >= 0; curr_ind += DIR_UP) {
    if(Piece::colour(board[curr_ind]) == 0) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
    } else if(Piece::colour(board[curr_ind]) != piece_col) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
      break;
    } else {
      break;
    }
  }

  // going down
  for(int curr_ind = i + DIR_DOWN; curr_ind < 64; curr_ind += DIR_DOWN) {
    if(Piece::colour(board[curr_ind]) == 0) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
    } else if(Piece::colour(board[curr_ind]) != piece_col) {
      sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
      break;
    } else {
      break;
    }
  }

  return sliding_moves;
}

std::vector<Move> GameState::generate_diagonal_sliding_moves(int i, char color) {
  std::vector<Move> sliding_moves;
  int piece = this->board[i];
  int piece_col = Piece::colour(piece);
  if(color != 0) {
    piece_col = color;
  }
  int file = get_file(i);
  
  if(file != 1) {
    // going up left
    for(int curr_ind = i + DIR_UP_LEFT; curr_ind >= 0; curr_ind += DIR_UP_LEFT) {
      // stop if we are on the left-most file
      if(Piece::colour(board[curr_ind]) == 0) {
        // if the square is empty, keep going
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
      } else if(Piece::colour(board[curr_ind]) != piece_col) {
        // if there is an enemy piece, capture it and stop
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
        break;
      } else {
        // else it is our own piece so stop
        break;
      }

      if(get_file(curr_ind) == 1)
        break;
    }

    // going down left
    for(int curr_ind = i + DIR_DOWN_LEFT; curr_ind < 64; curr_ind += DIR_DOWN_LEFT) {
      // stop if we are on the left-most file
      if(Piece::colour(board[curr_ind]) == 0) {
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
      } else if(Piece::colour(board[curr_ind]) != piece_col) {
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
        break;
      } else {
        break;
      }

      if(get_file(curr_ind) == 1)
        break;
    }
  }

  if (file != 8) {
    // going up right
    for(int curr_ind = i + DIR_UP_RIGHT; curr_ind >= 0; curr_ind += DIR_UP_RIGHT) {
      // stop if we are on the right-most file
      if(Piece::colour(board[curr_ind]) == 0) {
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
      } else if(Piece::colour(board[curr_ind]) != piece_col) {
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
        break;
      } else {
        break;
      }

      if(get_file(curr_ind) == 8)
        break;
    }


    // going down right
    for(int curr_ind = i + DIR_DOWN_RIGHT; curr_ind < 64; curr_ind += DIR_DOWN_RIGHT) {
      // stop if we are on the right-most file
      if(Piece::colour(board[curr_ind]) == 0) {
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::NORMAL));
      } else if(Piece::colour(board[curr_ind]) != piece_col) {
        sliding_moves.push_back(Move(i, curr_ind, piece, Move::CAPTURE));
        break;
      } else {
        break;
      }

      if(get_file(curr_ind) == 8)
        break;
    }
  }
  return sliding_moves;
}

std::vector<Move> GameState::generate_king_moves(int i, char color) {
  std::vector<Move> king_moves;
  int piece = this->board[i];
  int piece_col = Piece::colour(piece);
  if(color != 0) {
    piece_col = color;
  }

  int rank = get_rank(i);
  int file = get_file(i);
  // King move offsets
  int offsets[8] = {DIR_UP_LEFT, DIR_UP, DIR_UP_RIGHT, DIR_RIGHT, DIR_DOWN_RIGHT, DIR_DOWN, DIR_DOWN_LEFT, DIR_LEFT};
  // conditions for each move to be valid
  bool conditions[8] = {
    rank != 8 && file != 1,
    rank != 8,
    rank != 8 && file != 8,
    file != 8,
    rank != 1 && file != 8,
    rank != 1,
    rank != 1 && file != 1,
    file != 1
  };

  for(int j = 0; j < 8; j++) {
    if(conditions[j]) {
      if (Piece::colour(this->board[i + offsets[j]]) == 0) {
        king_moves.push_back(Move(i, i + offsets[j], piece, Move::NORMAL));
      } else if (Piece::colour(this->board[i + offsets[j]]) != piece_col) {
        king_moves.push_back(Move(i, i + offsets[j], piece, Move::CAPTURE));
      }
    }
  }

  if(piece_col == Piece::White) {
    if((castling_rights & WHITE_KING_SIDE) == WHITE_KING_SIDE && i == 60
      && board[61] == 0 && board[62] == 0) {
      // check if the squares between the king and rook are empty
      // checking if the squares are attacked happens later in the make_move function
      king_moves.push_back(Move(i, i + 2, piece, Move::CASTLE_KINGSIDE));
    }
    if((castling_rights & WHITE_QUEEN_SIDE) == WHITE_QUEEN_SIDE && i == 60
      && board[59] == 0 && board[58] == 0 && board[57] == 0) {
      king_moves.push_back(Move(i, i - 2, piece, Move::CASTLE_QUEENSIDE));
    }
  } else if(piece_col == Piece::Black) {
    if((castling_rights & BLACK_KING_SIDE) == BLACK_KING_SIDE && i == 4
      && board[5] == 0 && board[6] == 0) {
      king_moves.push_back(Move(i, i + 2, piece, Move::CASTLE_KINGSIDE));
    }
    if((castling_rights & BLACK_QUEEN_SIDE) == BLACK_QUEEN_SIDE && i == 4
      && board[3] == 0 && board[2] == 0 && board[1] == 0) {
      king_moves.push_back(Move(i, i - 2, piece, Move::CASTLE_QUEENSIDE));
    }
  }

  return king_moves;
};

void GameState::make_move(const std::string &move) {
  // start square, end square, piece / without color, flags
  std::array<int, 4> move_arr = Move::parse_lan(move);
  // castling returns start and end square as -1
  if(move_arr[0] == -1) {
    // castle
    if(Move::is_castle_kingside(move_arr[3])) {
      if(this->turn == Piece::White) {
        move_arr[0] = 60;
        move_arr[1] = 62;
      } else {
        move_arr[0] = 4;
        move_arr[1] = 6;
      }
    } else if(Move::is_castle_queenside(move_arr[3])) {
      if(this->turn == Piece::White) {
        move_arr[0] = 60;
        move_arr[1] = 58;
      } else {
        move_arr[0] = 4;
        move_arr[1] = 2;
      }
    }
  }

  if(this->board[move_arr[0]] == 0) {
    throw std::invalid_argument("No piece on the start square");
  }
  
  move_arr[2] |= Piece::colour(this->board[move_arr[0]]);
  Move m(move_arr[0], move_arr[1], move_arr[2], move_arr[3]);

  make_move(m);
};

void GameState::make_move(const Move &move) {
  auto it = std::find(legal_moves.begin(), legal_moves.end(), move);
  bool found = it != legal_moves.end();

  int dir = turn == Piece::White ? DIR_UP : DIR_DOWN;
  if(!found) {
    std::stringstream ss;
    ss << move;
    throw std::invalid_argument("Move is not legal " + ss.str());
  }

  // Save the gameData to restore it later
  GameData game_data = {castling_rights, en_passant_target, white_king_square, black_king_square,
    board[move.end], halfmove_clock, fullmove_counter};

  // update the board
  this->board[move.end] = this->board[move.start];
  this->board[move.start] = 0;

  // if it's a promotion, change the piece type
  if(Move::is_promotion_queen(move.flags)) {
    this->board[move.end] = Piece::Queen | Piece::colour(this->board[move.end]);
  } else if(Move::is_promotion_rook(move.flags)) {
    this->board[move.end] = Piece::Rook | Piece::colour(this->board[move.end]);
  } else if(Move::is_promotion_bishop(move.flags)) {
    this->board[move.end] = Piece::Bishop | Piece::colour(this->board[move.end]);
  } else if(Move::is_promotion_knight(move.flags)) {
    this->board[move.end] = Piece::Knight | Piece::colour(this->board[move.end]);
  }

  // if it's a double push, set the en passant target
  if(Move::is_double_push(move.flags)) {
    this->en_passant_target = move.end - dir;
  } else {
    this->en_passant_target = NO_EN_PASSANT;
  }

  // if it's a castle, move the rook
  if(Move::is_castle(move.flags)) {
    if(Move::is_castle_kingside(move.flags)) {
      if(this->turn == Piece::White) {
        std::swap(this->board[63], this->board[61]);
      } else {
        std::swap(this->board[7], this->board[5]);
      }
    } else {
      if(this->turn == Piece::White) {
        std::swap(this->board[56], this->board[59]);
      } else {
        std::swap(this->board[0], this->board[3]);
      }
    }
  }

  // if its an en passant capture, remove the captured pawn
  if(Move::is_en_passant(move.flags)) {
    this->board[move.end - dir] = 0;
  }

  // update castling rights and king square
  if(Piece::piece_type(move.piece) == Piece::King) {
    if(this->turn == Piece::White) {
      this->castling_rights &= ~WHITE_KING_SIDE;
      this->castling_rights &= ~WHITE_QUEEN_SIDE;
      this->white_king_square = move.end;
    } else {
      this->castling_rights &= ~BLACK_KING_SIDE;
      this->castling_rights &= ~BLACK_QUEEN_SIDE;
      this->black_king_square = move.end;
    }
  }

  // subtract castle rights if the rook is captured or moved
  if(move.end == 63 || move.start == 63) {
    this->castling_rights &= ~WHITE_KING_SIDE;
  }
  if(move.end == 56 || move.start == 56) {
    this->castling_rights &= ~WHITE_QUEEN_SIDE;
  }
  if(move.end == 7 || move.start == 7) {
    this->castling_rights &= ~BLACK_KING_SIDE;
  }
  if(move.end == 0 || move.start == 0) {
    this->castling_rights &= ~BLACK_QUEEN_SIDE;
  }

  // update halfmove clock
  if(Move::is_capture(move.flags) || Move::is_en_passant(move.flags)) {
    this->halfmove_clock = 0;
  } else {
    this->halfmove_clock++;
  }

  // update fullmove counter
  if(this->turn == Piece::Black) {
    this->fullmove_counter++;
  }

  // update turn
  this->turn = this->turn == Piece::White ? Piece::Black : Piece::White;

  // update legal moves
  legal_moves = generate_legal_moves(this->turn);
  moves_played.push_back(move);
  game_history.push_back(game_data);
};

void GameState::undo_move() {
  Move move = moves_played.back();
  GameData game_data = game_history.back();
  moves_played.pop_back();
  game_history.pop_back();
  int dir = this->turn == Piece::White ? DIR_DOWN : DIR_UP;

  this->castling_rights = game_data.castling_rights;
  this->en_passant_target = game_data.en_passant_square;
  this->halfmove_clock = game_data.halfmove_clock;
  this->fullmove_counter = game_data.fullmove_counter;
  this->white_king_square = game_data.white_king_square;
  this->black_king_square = game_data.black_king_square;

  if(Move::is_en_passant(move.flags)) {
    // undo en passant
    this->board[move.end - dir] = this->turn | Piece::Pawn;
  }

  if(Move::is_castle(move.flags)) {
    // undo castle by returning the rook
    if(Move::is_castle_kingside(move.flags)) {
      if(this->turn == Piece::Black) {
        this->board[63] = this->board[61];
        this->board[61] = 0;
      } else {
        this->board[7] = this->board[5];
        this->board[5] = 0;
      }
    } else {
      if(this->turn == Piece::Black) {
        this->board[56] = this->board[59];
        this->board[59] = 0;
      } else {
        this->board[0] = this->board[3];
        this->board[3] = 0;
      }
    }
  }


  this->turn = this->turn == Piece::White ? Piece::Black : Piece::White;
  this->board[move.start] = this->board[move.end];
  this->board[move.end] = game_data.captured_piece;
  // undo potential promotion
  if(Move::is_promotion(move.flags)) {
    this->board[move.start] = this->turn | Piece::Pawn;
  }

  legal_moves = generate_legal_moves(this->turn);
}

bool GameState::is_figure_move_legal(const Move &move) {
  int piece_col = Piece::colour(this->board[move.start]);
  int king_square = piece_col == Piece::White ? this->white_king_square : this->black_king_square;
  int dir = piece_col == Piece::White ? DIR_UP : DIR_DOWN;

  // to check if a move is legal, we update the board to the state after the move
  // (including a potential en passant capture, if we did not, there could be an edge case),
  // were a previously pinned pawn, captures another one that has moved 2 ranks via en passant
  // This removes two pawns and could discover an attack on the king, we need to prevent that
  // after playing the move, check if the square of the king is attacked
  int dest_piece = this->board[move.end];
  this->board[move.end] = this->board[move.start];
  this->board[move.start] = 0;
  if(Move::is_en_passant(move.flags)) {
    this->board[move.end - dir] = 0;
  }

  bool is_in_check = is_square_attacked(king_square, piece_col);

  // restore the previous position
  this->board[move.start] = this->board[move.end];
  this->board[move.end] = dest_piece;
  if(Move::is_en_passant(move.flags)) {
    this->board[move.end - dir] = piece_col == Piece::White ? (Piece::Black | Piece::Pawn) : (Piece::White | Piece::Pawn);
  }

  return !is_in_check;
};

bool GameState::is_king_move_legal(const Move &move) {
  int piece_col = Piece::colour(this->board[move.start]);
  int king_square = piece_col == Piece::White ? this->white_king_square : this->black_king_square;

  // if the king is moving into a check, return false
  // We need to update tje board to the state after the move
  // Then check if the king is attacked on the target square
  int previous_piece = this->board[move.end];
  this->board[move.end] = this->board[move.start];
  this->board[move.start] = 0;

  bool is_end_square_attacked = is_square_attacked(move.end, piece_col);

  this->board[move.start] = this->board[move.end];
  this->board[move.end] = previous_piece;

  if(is_end_square_attacked) {
    return false;
  }

  // if a king moves, it is either queenside or kingside castle or regular move
  if(!Move::is_castle(move.flags)) {
    // if it's a regular move, and the king is not attacked on the target square
    return true;
  }

  // here we are castling
  // we cannot castle out of a check
  if(is_square_attacked(king_square, piece_col)) {
    return false;
  }

  // squares that need to be not attacked for a move to be valid
  if(Move::is_castle_queenside(move.flags)) {

    if(piece_col == Piece::White) {
      // check if white can queenside castle
      if((castling_rights & WHITE_QUEEN_SIDE) == 0) {
        return false;
      }
      // then check if none of the squares during castling
      // are attacked by the opponent, if any of them is
      // the castle is illegal
      for(int j = 0; j < 2; j++) {
        if(is_square_attacked(WHITE_QUEENSIDE_SQUARES[j], piece_col)) {
          return false;
        }
      }

      // if none of the squares are attacked, the castle is legal
      return true;
    } else {
      // check if black can queenside castle
      if((castling_rights & BLACK_QUEEN_SIDE) == 0) {
        return false;
      }

      for(int j = 0; j < 2; j++) {
        if(is_square_attacked(BLACK_QUEENSIDE_SQUARES[j], piece_col)) {
          return false;
        }
      }

      return true;
    }
  }

  // here we already know the move is kingside castle
  if(piece_col == Piece::White) {
    // check if white can kingside castle
    if(castling_rights & WHITE_KING_SIDE == 0) {
      return false;
    }

    for(int j = 0; j < 2; j++) {
      if(is_square_attacked(WHITE_KINGSIDE_SQUARES[j], piece_col)) {
        return false;
      }
    }

    return true;
  } else {
    // check if black can kingside castle
    if(castling_rights & BLACK_KING_SIDE == 0) {
      return false;
    }

    for(int j = 0; j < 2; j++) {
      if(is_square_attacked(BLACK_KINGSIDE_SQUARES[j], piece_col)) {
        return false;
      }
    }

    return true;
  }

  return true;
};

bool GameState::is_square_attacked(int square, char color) {
  int file = get_file(square);
  int rank = get_rank(square);
  int opponent_col = color == Piece::White ? Piece::Black : Piece::White;

  // check pawn attacks
  if(color == Piece::White) {
    if(file != 1 && rank != 8 && board[square + DIR_UP_LEFT] == (Piece::Black | Piece::Pawn)) {
      return true;
    }
    if(file != 8 && rank != 8 && board[square + DIR_UP_RIGHT] == (Piece::Black | Piece::Pawn)) {
      return true;
    }
  } else {
    if(file != 1 && rank != 1 && board[square + DIR_DOWN_LEFT] == (Piece::White | Piece::Pawn)) {
      return true;
    }
    if(file != 8 && rank != 1 && board[square + DIR_DOWN_RIGHT] == (Piece::White | Piece::Pawn)) {
      return true;
    }
  }

  // check knight attacks
  for (auto &move: generate_knight_moves(square, color)) {
    // assume we have a knight on this position and are trying to check if it can attack the square
    // it's the same as checking if the square is attacked by a knight
    if(Move::is_capture(move.flags)
      && Piece::colour(this->board[move.end]) == opponent_col
      && Piece::piece_type(this->board[move.end]) == Piece::Knight) {
      return true;
    }
  }

  // check straight sliding attacks
  for(auto &move: generate_straight_sliding_moves(square, color)) {
    if(Move::is_capture(move.flags)
      && Piece::colour(this->board[move.end]) == opponent_col
      && Piece::is_rook_or_queen(this->board[move.end])) {
      return true;
    }
  }

  // check diagonal sliding attacks
  for(auto &move: generate_diagonal_sliding_moves(square, color)) {
    if(Move::is_capture(move.flags)
      && Piece::colour(this->board[move.end]) == opponent_col
      && Piece::is_bishop_or_queen(this->board[move.end])) {
      return true;
    }
  }

  // check king attacks
  for(auto &move: generate_king_moves(square, color)) {
    if(Move::is_capture(move.flags)
      && Piece::colour(this->board[move.end]) == opponent_col
      && Piece::piece_type(this->board[move.end]) == Piece::King) {
      return true;
    }
  }
  
  return false;
};

int GameState::get_rank(int square) const { return 8 - (square / 8); }
int GameState::get_file(int square) const {return (square % 8) + 1;}

void print_in_cyan(const std::string &str) {
  std::cout << "\033[0;96m" << str << "\033[0m";
  // std::cout << "\033[1;36m" << str << "\033[0m";
}

void GameState::print_board() const {
  print_in_cyan("\t +-------------------------+");
  for(int i = 0; i < 64; i++) {
    if(i % 8 == 0) {
      print_in_cyan("\n\t " + std::to_string(9 - get_rank(i)) + "  ");
    }
    char piece = Piece::get_piece_short(this->board[i]);
    if(Piece::colour(this->board[i]) == Piece::Black) {
      piece = tolower(piece);
    }
    if(piece == ' ') {
      std::cout << "·  ";
    } else {
      std::cout << piece << "  ";
    }
  }
  print_in_cyan("\n\t +-------------------------+\n");
  print_in_cyan("\t    a  b  c  d  e  f  g  h\n\n");
}

// -> '   +------------------------+
//      8 | r  n  b  q  k  b  n  r |
//      7 | p  p  p  p  .  p  p  p |
//      6 | .  .  .  .  .  .  .  . |
//      5 | .  .  .  .  p  .  .  . |
//      4 | .  .  .  .  P  P  .  . |
//      3 | .  .  .  .  .  .  .  . |
//      2 | P  P  P  P  .  .  P  P |
//      1 | R  N  B  Q  K  B  N  R |
//        +------------------------+
//          a  b  c  d  e  f  g  h'
std::vector<Move> GameState::get_legal_moves() const {
  return legal_moves;
}

// squares that cannot be attacked in order to castle on a given side
const char GameState::WHITE_KINGSIDE_SQUARES[2] = {61, 62};
const char GameState::WHITE_QUEENSIDE_SQUARES[2] = {59, 58};
const char GameState::BLACK_KINGSIDE_SQUARES[2] = {5, 6};
const char GameState::BLACK_QUEENSIDE_SQUARES[2] = {3, 2};