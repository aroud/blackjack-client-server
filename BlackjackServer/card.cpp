#include "card.h"

namespace blackjack {

	Card::Card(Suite s, Rank r) :suite_(s), rank_(r) {};

	bool Card::IsAce() const {
		return rank_ == Rank::ace;
	}

	Rank Card::GetRank() const {
		return rank_;
	}

	Suite Card::GetSuite() const {
		return suite_;
	}
}
