#pragma once

#include <vector>
#include <algorithm>
#include <memory>

#include "card.h"

namespace blackjack {

	class Deck {
	public:
		//Init deck and shuffle it
		Deck(size_t deck_units_number);

		//Blackjack deck consists of 2-6 standard 52-cards decks
		const size_t kDeckUnitSize = 52u;
		const size_t kDeckUnitsNumber;

		friend std::ostream& operator<< (std::ostream& os, const Deck& deck);

		void Reset();

		Card getCard();
	private:
		void InitCards();

		void Shuffle();

		std::vector<Card> cards_;
	};
}
