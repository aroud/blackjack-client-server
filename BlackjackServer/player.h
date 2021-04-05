#pragma once

#include "control_system.h"
#include "entity.h"

namespace blackjack {

    class Player : public Entity {
    public:
        Player(const ControlSystem& cs, size_t player_id, size_t initial_chips);
  
        virtual Turn MakeTurn() override;

        virtual void PrintHand(std::ostream& os) const override;

        size_t GetID() const;

        const ControlSystem& control_system_;

        friend std::ostream& operator<<(std::ostream& os, const Player& p);
    private:
        size_t player_id_;
    };
}
