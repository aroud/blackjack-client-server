#include "player.h"

namespace blackjack {

	Player::Player(const ControlSystem& cs, size_t player_id, size_t initial_chips) : Entity(initial_chips), control_system_(cs), player_id_(player_id) {}
	
	Turn Player::MakeTurn()
	{
		return control_system_.GetTurn();
	}

	void Player::PrintHand(std::ostream& os) const
	{
		for (size_t i = 0; i < hand_.GetSize(); ++i) {
			os << hand_[i];
		}
	}

	size_t Player::GetID() const
	{
		return player_id_;
	}
}