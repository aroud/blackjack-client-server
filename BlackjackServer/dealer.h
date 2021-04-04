#pragma once

#include "entity.h"

namespace blackjack {

    class Dealer : public Entity {
    public:
        Dealer();

        const size_t kDealerStop = 17u;

        virtual void PrintHand(std::ostream& os) const override;

        virtual Turn MakeTurn() override;
    private:
        bool is_turn_ = false;
    };
}

