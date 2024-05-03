#include "Move.h"
#include "../src/Piece.cpp"
#include <array>
#include <vector>

#pragma once

/**
 * @struct GameData
 * @brief A struct to hold the state of a chess game.
 *
 * This struct holds the current state of a chess game, including the castling rights,
 * the en passant square, the squares of the white and black kings, the last piece
 * that was captured, the halfmove clock for the fifty-move rule, and the fullmove counter.
 *
 * It is used to undo moves, each move, a new GameData struct is stored in game_history vector.
 */
struct GameData {
  char castling_rights;
  char en_passant_square;
  char white_king_square;
  char black_king_square;
  int captured_piece;
  int halfmove_clock;
  int fullmove_counter;

  GameData(char castling_rights, char en_passant_square, char white_king_square,
    char black_king_square, int captured_piece, int halfmove_clock, int fullmove_counter)
    : castling_rights(castling_rights), en_passant_square(en_passant_square),
      white_king_square(white_king_square), black_king_square(black_king_square),
      captured_piece(captured_piece), halfmove_clock(halfmove_clock),
      fullmove_counter(fullmove_counter) {};
};

class GameState {
public:
  /**
   * @brief This constructor is used by FenParser to create a new game from fen
   *
   * \b Example: GameState game = FenParser::parse_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
   *
   * @param board An array representing the current state of the chess board. Each element of the array represents a square on the board, and the value of the element represents the piece on that square.
   * @param turn The current turn. This should be 1 for white and -1 for black.
   * @param castling_rights The current castling rights. This is a bitmask where each bit represents the castling right for a particular side and color.
   * @param en_passant_square The square where en passant capture is currently possible. This should be -1 if there is no en passant square.
   * @param halfmove_clock The number of halfmoves since the last pawn advance or capture. This is used for the fifty-move rule.
   * @param fullmove_counter The number of full moves in the game. This is incremented after each black move.
 */
  GameState(const std::array<int, 64> &board, int turn, char castling_rights, char en_passant_square, int halfmove_clock, int fullmove_counter);

  /**
   * @brief Constructs a new GameState object with default parameters.
   * Initializes a new GameState object with a default board.
  */
  GameState();

  /**
   * @brief Make a move if it is legal.
   * 
   * First this function checks if the move is legal, by searching for it in legal_moves vector.
   * Then if it is, it makes the move and updates the GameState params like:
   * board, turn, castling rights, en passant square etc.
   *
   * @param Move Move to make.
  */
  void make_move(const Move &move);

  /**
   * @brief Make a move represented in the Long Algebraic notation, examples in Move.h
   * Rest is the same as make_move from Move object
   * 
   * @param Move Move to make in LAN notation.
  */
  void make_move(const std::string &move);

  /**
   * @brief Undo the last move stored in the moves_played
   * Pops the move from moves_played and pops GameData from game_history.
   * Then it restores the state of the game to the state before the move was made.
  */
  void undo_move();

  /**
   * @brief Check if a given move of any piece other than the King is legal.
   * Function used in legal move generation. Once all pseudolegal moves are generated, it checks
   * if after each move, the king square is attacked. If it is, the move is illegal.
   * 
   * @param move Move to check if it is legal.
   * 
   * @return true if it is legal.
   * 
  */
  bool is_figure_move_legal(const Move &move);

  /**
   * @brief Check if a given King Move is legal.
   * Once all king moves, including castles are generated, this function checks if the king is attacked
   * on the end square, if the move is a castle, it also checks if it was attacked on any of the squares during castling
   * and if it was, the move is illegal.
   * 
   * @return true if it is legal.
   * 
   */
  bool is_king_move_legal(const Move &move);

  /**
   * @brief Check if a square is attacked.
   * 
   * @param square The square to check if it is attacked.
   * @param color The color of our pieces. The color of the piece on target square.
   * 
  */
  bool is_square_attacked(int square, char color);

  std::vector<Move> generate_legal_moves(char color);
  std::vector<Move> generate_pawn_moves(int square);
  std::vector<Move> generate_knight_moves(int square, char color = 0);
  std::vector<Move> generate_diagonal_sliding_moves(int square, char color = 0);
  std::vector<Move> generate_straight_sliding_moves(int square, char color = 0);
  std::vector<Move> generate_king_moves(int square, char color = 0);

  std::vector<Move> get_legal_moves() const;

  int get_rank(int square) const;
  int get_file(int square) const;

  void print_board() const;

  std::array<int, 64> board;
  // Piece::White is white, Piece::Black is black
  int turn;
  // 0 represents full castling rights
  char castling_rights;
  // square where en passant is possible
  // index of square behind the pawn that moved two squares
  char en_passant_target;

  char white_king_square = -1;
  char black_king_square = -1;

  // number of half moves since last capture or pawn move
  int halfmove_clock = 0;
  int fullmove_counter = 0;

private:
  // current legal moves
  std::vector<Move> legal_moves;
  std::vector<Move> moves_played;
  std::vector<GameData> game_history;

  static const char WHITE_QUEENSIDE_SQUARES[2];
  static const char WHITE_KINGSIDE_SQUARES[2];
  static const char BLACK_QUEENSIDE_SQUARES[2];
  static const char BLACK_KINGSIDE_SQUARES[2];

  static const char NO_EN_PASSANT = -1;
};