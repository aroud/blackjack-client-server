#include "console_input_system.h"

namespace blackjack {

    ConsoleInputSystem::ConsoleInputSystem() {}

    Turn blackjack::ConsoleInputSystem::GetTurn(const bool made_turn) const
    {
        std::string input;

        bool correct_input = false;

        Turn turn = Turn::stand;

        
        do {
            std::cout << "Input turn {stand, hit, doubleDown, surrender} in you have 2 cards only or\n" <<
                "{hit, stand} on other cases:\n";
            std::cin >> input;
            if (input == "stand") {
                correct_input = true;
            }
            else {
                if (input == "hit") {
                    turn = Turn::hit;
                    correct_input = true;
                }
                else {
                    if (input == "doubleDown") {
                        if (!made_turn) {
                            turn = Turn::doubleDown;
                            correct_input = true;
                        }
                    }
                    else {
                        if (input == "surrender") {
                            if (!made_turn) {
                                turn = Turn::surrender;
                                correct_input = true;
                            }
                        }
                    }
                }
            }
        } while (!correct_input);
      
        return turn;
    }

    size_t blackjack::ConsoleInputSystem::StartRound(size_t min_bet, size_t max_bet) const
    {
        size_t result = 0;
        
        do {
            std::cout << "Input your bet (between " << min_bet << " and " << max_bet << ") or 0 to exit game:\n";
            std::cin >> result;
        } while (!((result == 0) || (result >= min_bet && result <= max_bet)));
        return result;
    }
}
