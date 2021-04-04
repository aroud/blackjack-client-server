#pragma once

#include <iostream>

#include "hand.h"

namespace blackjack {
	enum class Turn {
		stand,
		hit,
		doubleDown,
		surrender
	};

	class Entity
	{
	public:
		Entity();

		friend std::ostream& operator<<(std::ostream& os, const Entity& entity);

		virtual Turn MakeTurn() = 0;
	protected:
		virtual void PrintHand(std::ostream& os) const = 0;

		Hand hand_;
	};
}
