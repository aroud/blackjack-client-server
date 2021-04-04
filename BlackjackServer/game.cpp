#include "game.h"

namespace blackjack {

	Game::Game(size_t deck_units_number):
		deck_(deck_units_number),
		cs_(ConsoleInputSystem()),
		game_status_(GameStatus::started),
		dealer_(chips_constants::kDealerDefaultChipsNumber)
	{}

	void Game::RegisterPlayers()
	{
		game_status_ = GameStatus::playerRegistration;

		player_ptr_vect_.push_back(std::make_shared<Player>(cs_, 1, chips_constants::kPlayerDefaultChipsNumber));
		player_ptr_vect_.push_back(std::make_shared<Player>(cs_, 2, chips_constants::kPlayerDefaultChipsNumber));
	}

	void Game::BeginRound()
	{

	}

	void Game::PlayRound()
	{

	}
	void Game::EndRound()
	{

	}
}
