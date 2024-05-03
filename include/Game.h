#include <vector>
#include <string>
#include "FenParser.h"
#include "ChessConstants.h"
#include "GameState.h"

class Game {
public:
    Game(const std::string &fen = STARTING_FEN) {
        game = FenParser::parse_fen(fen);
    };

    void play() {
        std::string input;

        bool is_white_turn = game.turn == Piece::White;

        while (true) {
            std::vector<Move> moves = game.get_legal_moves();
            std::cout << "Number of legal moves: " << moves.size() << std::endl;

            for(auto &move : moves){
                std::cout << move << std::endl;
            }

            game.print_board();

            std::cout << (is_white_turn ? "White " : "Black ") << "to move (or 'q' to quit): ";
            std::getline(std::cin, input);

            if (input == "q") {
                break;
            } else if(input == "undo"){
                game.undo_move();
                is_white_turn = !is_white_turn;
                continue;
            }

            try {
                game.make_move(input);
            } catch (std::exception &e) {
                std::cout << e.what() << std::endl;
                continue;
            }

            // Switch turns
            is_white_turn = !is_white_turn;
        }
    }

private:
    GameState game;
};