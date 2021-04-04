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
	std::ostream& operator<<(std::ostream& os, const Card& card)
	{
		os << static_cast<int>(card.GetRank()) << " " << static_cast<int>(card.GetSuite()) << "\n";
		return os;
	}
}
