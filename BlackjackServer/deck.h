#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

#include "card.h"

namespace blackjack {
	class Deck
	{
	public:
		//Init deck and shuffle it
		Deck(size_t deck_units_number);

		//Blackjack deck consists of 2-6 standard decks
		const size_t kDeckUnitSize = 52;
		const size_t kDeckUnitsNumber;

		friend std::ostream& operator<< (std::ostream& os, const Deck& deck);
	private:
		void InitCards();

		void Shuffle();

		std::vector<Card> cards_;
	};
}
