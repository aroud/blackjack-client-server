#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <algorithm>
#include <string>
#include <limits>

#include "json.hpp"

#include "player.h"
#include "dealer.h"
#include "console_input_system.h"
#include "deck.h"


namespace blackjack {

	enum class GameStatus {
		started,
		playerRegistration,
		makingBets,
		inRound,
		checkingResults,
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
		const size_t kMax = UINT32_MAX;

		Game(size_t deck_units_number);

		void RegisterPlayers();

		void BeginRound();

		void PlayRound();

		void EndRound();

		void PlayGame();

		std::string ToJson();

		void ClearGame();
	//private:
		RoundResults CheckWin(std::shared_ptr<Player> player_ptr);

		size_t curr_player_id_ = kMax;
		
		GameStatus game_status_;

		Deck deck_;

		std::shared_ptr<ControlSystem> cs_;

		std::vector<std::shared_ptr<Player>> player_ptr_vect_;

		Dealer dealer_;

		std::vector<std::pair<size_t, size_t>> bets_;
	};
}

