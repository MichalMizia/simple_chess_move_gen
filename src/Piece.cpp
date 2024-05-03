#include <iostream>

#pragma once

class Piece {
public:
    // The chess board is a array of 64 integers.
    // we represent pieces as integers, with the following bitmasks:
    static const int NonePiece = 0;
    static const int King = 1;
    static const int Pawn = 2;
    static const int Knight = 3;
    static const int Bishop = 5;
    static const int Rook = 6;
    static const int Queen = 7;

    static const int White = 8;
    static const int Black = 16;

    static const int typeMask = 0b00111;
    static const int blackMask = 0b10000;
    static const int whiteMask = 0b01000;
    static const int colourMask = whiteMask | blackMask;

    static bool is_colour(int piece, int colour) {
        return (piece & colourMask) == colour;
    }

    static int colour(int piece) {
        return piece & colourMask;
    }

    static int piece_type(int piece) {
        return piece & typeMask;
    }

    static bool is_rook_or_queen(int piece) {
        return (piece & 0b110) == 0b110;
    }

    static bool is_bishop_or_queen(int piece) {
        return (piece & 0b101) == 0b101;
    }

    static bool is_sliding_piece(int piece) {
        return (piece & 0b100) != 0;
    }

    static std::string get_piece_name(int piece) {
        switch (piece) {
            case King | White:
            case King | Black:
                return "King";
            case Pawn | White:
            case Pawn | Black:
                return "Pawn";
            case Knight | White:
            case Knight | Black:
                return "Knight";
            case Bishop | White:
            case Bishop | Black:
                return "Bishop";
            case Rook | White:
            case Rook | Black:
                return "Rook";
            case Queen | White:
            case Queen | Black:
                return "Queen";
            default:
                return "None";
        }
    }

    static char get_piece_short(int piece) {
        switch(piece) {
            case King | White:
            case King | Black:
                return 'K';
            case Pawn | White:
            case Pawn | Black:
                return 'P';
            case Knight | White:
            case Knight | Black:
                return 'N';
            case Bishop | White:
            case Bishop | Black:
                return 'B';
            case Rook | White:
            case Rook | Black:
                return 'R';
            case Queen | White:
            case Queen | Black:
                return 'Q';
            default:
                return ' ';
        }
    }
};