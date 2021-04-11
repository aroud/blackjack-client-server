#pragma once

#include <string>
#include <vector>

#include "card.h"

class PlayerStub
{
public:
	PlayerStub() {}
	PlayerStub(size_t id, bool dealer, const std::vector<std::string>& cards, size_t chips):
		player_id_(id), cards_(cards), chips_(chips) {}
	size_t player_id_;
	std::vector<std::string> cards_;
	size_t chips_ = blackjack::chips_constants::kPlayerDefaultChipsNumber;
	bool is_dealer_;
};

