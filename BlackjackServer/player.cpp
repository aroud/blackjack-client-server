#include "player.h"

namespace blackjack {
	Player::Player(const ControlSystem& cs) : control_system_(cs) {}
	
	Turn Player::MakeTurn()
	{
		return Turn();
	}

	void Player::PrintHand(std::ostream& os) const
	{
		for (size_t i = 0; i < hand_.GetSize(); ++i) {
			os << hand_[i];
		}
	}
}