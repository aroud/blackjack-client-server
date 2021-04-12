#pragma once

#include <iostream>
#include <string>

#include "entity.h"

namespace blackjack {

    class ConsoleInputSystemStub
    {
    public:
        ConsoleInputSystemStub();

        blackjack::Turn GetTurn(const bool made_turn) const;

        size_t StartRound(size_t min_bet, size_t max_bet) const;
    };
}
