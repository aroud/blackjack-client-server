#pragma once

#include "entity.h"

namespace blackjack {

	class ControlSystem {
	public:
		virtual Turn GetTurn() const = 0;
		virtual size_t StartRound() const = 0;
	};
}

