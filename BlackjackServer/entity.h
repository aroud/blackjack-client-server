#pragma once

#include <iostream>

#include "hand.h"
#include "deck.h"

namespace blackjack {

	enum class Turn {
		stand,
		hit,
		doubleDown,
		surrender
	};

	namespace chips_constants {
		const size_t kPlayerDefaultChipsNumber = 200;
		const size_t kDealerDefaultChipsNumber = 2000;
		const size_t kMinBet = 5;
		const size_t kMaxBet = 15;
	}

	class Entity {
	public:
		Entity(size_t initial_chips);

		friend std::ostream& operator<<(std::ostream& os, const Entity& entity);

		virtual Turn MakeTurn() = 0;

		void DoTurnActions(Turn turn, Deck& deck, size_t& bet);

		size_t GetChips();

		void SetChips(size_t chips);

		Hand& GetHand();

		bool has_surrended = false;
	protected:
		virtual void PrintHand(std::ostream& os) const = 0;

		Hand hand_;

		size_t chips_;
	};
}
