#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <algorithm>
#include <string>
#include <limits>
#include <thread>
#include <atomic>

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

		void PlayGameMultiThread();

		std::string ToJson();

		void ClearGame();

		void ChangeActionDone();

		bool GetActionDone() const;

		size_t current_bet_;

		Turn current_turn_;

		GameStatus GetGameStatus() const;

		bool AddPlayer(size_t id);

		bool RemovePlayer(size_t id);
	private:
		RoundResults CheckWin(std::shared_ptr<Player> player_ptr);
		
		GameStatus game_status_;

		Deck deck_;

		std::shared_ptr<ControlSystem> cs_;

		std::vector<std::shared_ptr<Player>> player_ptr_vect_;

		std::vector<std::pair<size_t, size_t>> bets_;

		Dealer dealer_;

		size_t curr_player_id_ = kMax;

		std::atomic<bool> action_done_ = false;
	};
}

