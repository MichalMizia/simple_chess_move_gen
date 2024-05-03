#include <iostream>
#include "../src/Piece.cpp"
#include <vector>
#include <array>
#include <string>

class Move {
public:
    // Constructor
    Move(int start, int end, int piece, int flags = 0);

    int start;
    int end;
    int piece;
    int flags;

    // overloaded operators
    friend std::ostream& operator<<(std::ostream &os, const Move &move);
    bool operator==(const Move &other) const;
    bool operator!=(const Move &other) const;
    // used only in tests: 
    bool operator<(const Move &other) const;
    bool operator>(const Move &other) const;

    /**
     * @brief Parses a string in long algebraic notation (LAN) into a move.
     * 
     * Examples of lan strings:
     * Regular move: \b e2-e4, 
     * Knight capture: \b Nc3xd5, 
     * Promotion: \b e7-e8=Q,
     * Castle: O-O(kingside) or O-O-O(queenside)
     * When the move is castle, the start and end square are returned as -1 and its the callers responsibility to change them to king squares based on current turn.
     * En passant: e5xd6 e.p
     * 
     * @param lan_str The string in long algebraic notation.
     * @return A std::array<int, 4> representing the move. For example, the LAN string "e2-e4" would be converted into the array {4, 1, 4, 3}.
     */
    static std::array<int, 4> parse_lan(const std::string &lan_str);

    /**
     * @brief Returns a lan string of the current move object.
     *
     * Examples of lan strings: 
     * Regular move: e2-e4
     * Knight capture: Nc3xd5
     * Promotion: e7-e8=Q
     * Castle: O-O(kingside) or O-O-O(queenside)
     * When the move is castle, the start and end square are returned as -1 and its the callers responsibility to change them to king squares based on current turn.
     * En passant: e5xd6 e.p
     * 
     * @return lan_str in Long Algebraic Notation.
     */
    std::string lan_str() const;
    std::string perft_str() const;
    
    static bool is_normal(int flags);
    static bool is_capture(int flags);
    static bool is_en_passant(int flags);
    static bool is_double_push(int flags);
    static bool is_castle_kingside(int flags);
    static bool is_castle_queenside(int flags);
    static bool is_castle(int flags);
    static bool is_promotion_queen(int flags);
    static bool is_promotion_rook(int flags);
    static bool is_promotion_bishop(int flags);
    static bool is_promotion_knight(int flags);
    static bool is_promotion(int flags);

    static const int NORMAL = 1;
    static const int CAPTURE = 2;
    static const int EN_PASSANT = 4;
    static const int DOUBLE_PUSH = 8;
    static const int CASTLE_KINGSIDE = 16;
    static const int CASTLE_QUEENSIDE = 32;
    static const int PROMOTION_QUEEN = 64;
    static const int PROMOTION_ROOK = 128;
    static const int PROMOTION_BISHOP = 256;
    static const int PROMOTION_KNIGHT = 512;
};