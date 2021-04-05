#include "console_input_system.h"

namespace blackjack {

    Turn blackjack::ConsoleInputSystem::GetTurn() const
    {
        std::string input;

        bool correct_input = false;

        Turn turn = Turn::stand;

        do {
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
                        turn = Turn::doubleDown;
                        correct_input = true;
                    }
                    else {
                        if (input == "surrender") {
                            turn = Turn::surrender;
                            correct_input = true;
                        }
                    }
                }
            }
        } while (!correct_input);
      
        return turn;
    }

    size_t blackjack::ConsoleInputSystem::StartRound(size_t min_bet, size_t max_bet) const
    {
        return size_t();
    }
}
