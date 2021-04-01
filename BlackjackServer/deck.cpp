#include "deck.h"

namespace blackjack {
	
	Deck::Deck(size_t deck_units_number): kDeckUnitsNumber(deck_units_number) {
		InitCards();
		Shuffle();
	}

	void Deck::InitCards() {
		cards_.reserve(kDeckUnitSize * kDeckUnitsNumber);

		std::vector<Suite> all_suites = { Suite::clubs, Suite::diamonds, Suite::hearts, Suite::spades };
		for (size_t j = 0; j < kDeckUnitsNumber; ++j) {
			for (Suite suite : all_suites)
			{
				for (size_t i = 1; i <= kRankSize; ++i) {
					Rank rank = static_cast<Rank>(i);
					cards_.push_back(Card(suite, rank));
				}
			}
		}
	}

	void Deck::Shuffle() {
		std::random_shuffle(begin(cards_), end(cards_));
	}

	std::ostream& operator<< (std::ostream& os, const Deck& deck) {
		for (const auto& card : deck.cards_) {
			
			std::cout << static_cast<int>(card.GetRank()) << " " << static_cast<int>(card.GetSuite()) << "\n";
		}
		return os;
	}
}
