#pragma once

#include "entity.h"

namespace blackjack {

	class ControlSystem {
	public:
		virtual Turn GetTurn() const = 0;

		virtual size_t StartRound(size_t min_bet, size_t max_bet) const = 0;
	};
}

