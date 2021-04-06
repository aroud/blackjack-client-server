#include "player.h"

namespace blackjack {

	Player::Player(std::shared_ptr<ControlSystem> cs, size_t player_id, size_t initial_chips) : Entity(initial_chips), control_system_(cs), player_id_(player_id) {}
	
	Turn Player::MakeTurn()
	{
		return control_system_->GetTurn(*this);
	}

	void Player::PrintHand(std::ostream& os) const
	{
		if (!hand_.GetSize()) {
			os << "(empty)\n";
		}
		else {
			for (size_t i = 0; i < hand_.GetSize(); ++i) {
				os << hand_[i];
			}
		}
	}

	size_t Player::GetID() const
	{
		return player_id_;
	}

	std::ostream& operator<<(std::ostream& os, const Player& p)
	{
		os << "Player, ID: " << p.player_id_ << ", chips count: " << p.chips_ << "\nHand: ";
		p.PrintHand(os);
		
		return os;
	}
}