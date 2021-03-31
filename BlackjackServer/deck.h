#pragma once

#include <vector>

#include "card.h"

namespace blackjack {
	class Deck
	{
	public:
		//Init deck and shuffle it
		Deck();

		const size_t kDeckSize = 52;
	private:
		void InitCards();
		void Shuffle();

		std::vector<Card> cards_;
	};
}
