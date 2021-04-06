#pragma once

#include <vector>

#include "card.h"

namespace blackjack {

	class Hand {
	public:
		Hand();

		void AddCard(Card& card);

		int GetSumValues() const;

		size_t GetSize() const;

		bool IsBlackjack() const;

		bool IsBusted() const;

		void ClearHand();

		const int kBustSum = 22;

		Card operator[] (size_t index) const;
	private:
		std::vector<Card> cards_at_hand_;
	};
}

