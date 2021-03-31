#include "card.h"

namespace blackjack {

	Card::Card(Suite s, Rank r) :suite_(s), rank_(r) {};

	bool Card::IsAce() {
		return rank_ == Rank::ace;
	}

	Rank Card::GetRank()
	{
		return rank_;
	}
}
