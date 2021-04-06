#include "dealer.h"

namespace blackjack{

	Dealer::Dealer(size_t initial_chips) : Entity(initial_chips) {}

	void Dealer::PrintHand(std::ostream& os) const
	{
		if (!is_turn_) {
			os << hand_[0] << "(?)\n";
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

	std::ostream& operator<<(std::ostream& os, const Dealer& dealer)
	{
		os << "Dealer, chips count: " << dealer.chips_ << "\nHand: ";
		dealer.PrintHand(os);
		os << std::endl;

		return os;
	}
}