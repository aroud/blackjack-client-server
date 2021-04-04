#include "dealer.h"

namespace blackjack{
	Dealer::Dealer() : Entity() {}

	void Dealer::PrintHand(std::ostream& os) const
	{
		if (!is_turn_) {
			os << hand_[0];
		}
		else {
			for (size_t i = 0; i < hand_.GetSize(); ++i) {
				os << hand_[i];
			}
		}
	}

	Turn Dealer::MakeTurn()
	{
		is_turn_ = true;
		Turn turn = Turn::stand;
		if(hand_.GetSumValues() < 17) {
			turn = Turn::hit;
		}
		return turn;
	}
}