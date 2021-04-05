#include "hand.h"

namespace blackjack {

	Hand::Hand() {}

	void Hand::AddCard(Card& card) {
		cards_at_hand_.push_back(card);
	}

	int Hand::GetSumValues() const {
		int result_sum = 0;
		bool ace_in_hand = false;

		for (const Card& card: cards_at_hand_) {
			Rank card_rank = card.GetRank();

			switch (card_rank)
			{
			case Rank::jack: case Rank::queen: case Rank::king:
				result_sum += 10;
				break;
			case Rank::ace:
				result_sum += 1;
				ace_in_hand = true;
				break;
			default: {
				int card_value = static_cast<int>(card_rank);
				result_sum += card_value;
			}
				break;
			}
		}
		
		if (ace_in_hand && (result_sum + 10 < kBustSum)) {
			result_sum += 10;
		}

		return result_sum;
	}

	size_t Hand::GetSize() const {
		return cards_at_hand_.size();
	}

	bool Hand::IsBlackjack() const {
		return (cards_at_hand_.size() == 2 && GetSumValues() == 21);
	}

	bool Hand::IsBusted() const
	{
		return GetSumValues() >= kBustSum;
	}

	void Hand::ClearHand() {
		cards_at_hand_.clear();
	}

	Card Hand::operator[](size_t index) const
	{
		return cards_at_hand_[index];
	}
}
