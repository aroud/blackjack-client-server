#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <algorithm>
#include <string>

#include "json.hpp"

#include "player.h"
#include "dealer.h"
#include "console_input_system.h"
#include "deck.h"


namespace blackjack {

	enum class GameStatus {
		started,
		playerRegistration,
		inProcess,
		ended
	};

	enum class RoundResults {
		playerWin32,
		playerWin11,
		draw,
		playerLose
	};

	class Game {
	public:
		Game(size_t deck_units_number);

		void RegisterPlayers();

		void BeginRound();

		void PlayRound();

		void EndRound();

		void PlayGame();

		std::string ToJson();
	//private:
		RoundResults CheckWin(std::shared_ptr<Player> player_ptr);

		size_t curr_player_id_ = std::numeric_limits<size_t>::max();
		
		GameStatus game_status_;

		Deck deck_;

		std::shared_ptr<ControlSystem> cs_;

		std::vector<std::shared_ptr<Player>> player_ptr_vect_;

		Dealer dealer_;

		std::vector<std::pair<size_t, size_t>> bets_;
	};
}

