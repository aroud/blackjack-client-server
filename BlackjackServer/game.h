#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>

#include "player.h"
#include "dealer.h"
#include "console_input_system.h"
#include "deck.h"


namespace blackjack {

	enum class GameStatus {
		playerRegistration,
		started,
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
	private:
		RoundResults CheckWin(std::shared_ptr<Player> player_ptr, Dealer& dealer);
		
		GameStatus game_status_;

		Deck deck_;

		const ControlSystem& cs_;

		std::vector<std::shared_ptr<Player>> player_ptr_vect_;

		Dealer dealer_;

		std::vector<std::pair<size_t, size_t>> bets_;
	};
}

