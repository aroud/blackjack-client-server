#pragma once

#include "entity.h"

namespace blackjack {

    class Dealer : public Entity {
    public:
        Dealer(size_t initial_chips);

        const size_t kDealerStop = 17u;

        virtual void PrintHand(std::ostream& os) const override;

        virtual Turn MakeTurn() override;

        friend std::ostream& operator<<(std::ostream& os, const Dealer& dealer);
    private:
        bool is_turn_ = false;
    };
}

