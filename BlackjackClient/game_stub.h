#pragma once

#include <vector>
#include <limits>

#include "player_stub.h"
#include "console_input_system_stub.h"

class GameStub
{
public:
	GameStub();

	std::vector<PlayerStub> players_;

	PlayerStub dealer_;

	blackjack::ConsoleInputSystemStub cs_;

	size_t client_idx_ = UINT32_MAX;

	bool player_made_turn_ = false;

	bool try_to_get_id_ = false;

	/*to print turn results only once*/
	bool printed_once = false;

	size_t min_bet_ = client_chips_constants::kMinBet;

	size_t max_bet_ = client_chips_constants::kMaxBet;
};

