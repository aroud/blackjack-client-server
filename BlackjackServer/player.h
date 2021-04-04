#pragma once

#include "entity.h"

namespace blackjack {

    class ControlSystem;

    class Player : public Entity
    {
        Player(ControlSystem cs);
  
        virtual Turn MakeTurn() override;

        virtual void PrintHand(std::ostream& os) const override;
    private:
        ControlSystem control_system_;
    };
}
