#pragma once

#include <iostream>
#include <string>

#include "control_system.h"

namespace blackjack {

    class ConsoleInputSystem : public ControlSystem
    {
    public:
        virtual Turn GetTurn() const override;

        virtual size_t StartRound(size_t min_bet, size_t max_bet) const override;
    };
}
