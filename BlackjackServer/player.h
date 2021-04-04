#pragma once

#include "control_system.h"

namespace blackjack {

    class Player : public Entity {
        Player(const ControlSystem& cs, size_t player_id);
  
        virtual Turn MakeTurn() override;

        virtual void PrintHand(std::ostream& os) const override;

        size_t GetID() const;
    private:
        const ControlSystem& control_system_;

        size_t player_id_;
    };
}
