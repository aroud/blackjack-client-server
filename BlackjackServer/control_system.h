#pragma once

#include "entity.h"

namespace blackjack {

	class ControlSystem {
	public:
		virtual Turn GetTurn() = 0;
	};
}

