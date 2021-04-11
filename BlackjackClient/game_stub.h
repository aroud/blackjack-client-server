#pragma once

#include <vector>
#include <limits>;

#include "player_stub.h"
#include "game.h"

class GameStub
{
public:
	GameStub() { dealer_.chips_ = blackjack::chips_constants::kDealerDefaultChipsNumber; };
	std::vector<PlayerStub> players_;
	PlayerStub dealer_;
	blackjack::ConsoleInputSystem cs_;
	size_t client_idx_ = UINT32_MAX;
	bool player_made_turn_ = false;
	size_t min_bet_;
	size_t max_bet_;
};

