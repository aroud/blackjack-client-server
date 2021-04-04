#pragma once

#include "control_system.h"
#include "entity.h"

namespace blackjack {

    class Player : public Entity {
        Player(const ControlSystem& cs, size_t player_id, size_t initial_chips);
  
        virtual Turn MakeTurn() override;

        virtual void PrintHand(std::ostream& os) const override;

        size_t GetID() const;
    private:
        const ControlSystem& control_system_;

        size_t player_id_;
    };
}
