#pragma once

#include "control_system.h"

namespace blackjack {
    class Player : public Entity
    {
        Player(const ControlSystem& cs);
  
        virtual Turn MakeTurn() override;

        virtual void PrintHand(std::ostream& os) const override;
    private:
        const ControlSystem& control_system_;
    };
}
