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
		for (auto player_ptr : player_ptr_vect_)
		{
			size_t bet = player_ptr->control_system_.StartRound(chips_constants::kMinBet, chips_constants::kMaxBet);
			size_t player_id = player_ptr->GetID();
			player_ptr->SetChips(player_ptr->GetChips() - bet);

			if (!bet) {
				auto it = std::find_if(
					begin(player_ptr_vect_),
					end(player_ptr_vect_),
					[player_id](std::shared_ptr<Player> ptr) {return ptr->GetID() == player_id; }
				);
				player_ptr_vect_.erase(it);
			} else {
				bets_.emplace_back(player_id, bet);
			}
		}
	}

	void Game::PlayRound()
	{
		for (auto player_ptr : player_ptr_vect_)
		{
			size_t player_id = player_ptr->GetID();

			auto it = std::find_if(
				begin(bets_),
				end(bets_),
				[player_id](std::pair<size_t,size_t> p) {return p.first == player_id; }
			);
			Turn turn = Turn::stand;
			do {
				 turn = player_ptr->MakeTurn();
				 player_ptr->DoTurnActions(turn, deck_, it->second);
			} while (turn == Turn::hit);
		}
		Turn dealer_turn = dealer_.MakeTurn();
		size_t i = 0;
		dealer_.DoTurnActions(dealer_turn, deck_, std::ref(i));
	}

	void Game::EndRound()
	{
		for (auto player_ptr : player_ptr_vect_)
		{
			size_t player_id = player_ptr->GetID();

			auto it = std::find_if(
				begin(bets_),
				end(bets_),
				[player_id](std::pair<size_t, size_t> p) {return p.first == player_id; }
			);
			size_t bet = it->second;

			RoundResults result = RoundResults::playerLose;
			if (!player_ptr->has_surrended) {
				result = CheckWin(player_ptr, dealer_);
			}

			player_ptr->has_surrended = false;
			
			switch (result)
			{
			case blackjack::RoundResults::playerWin32:
				player_ptr->SetChips(dealer_.GetChips() + 3 * bet / 2);
				break;
			case blackjack::RoundResults::playerWin11:
				player_ptr->SetChips(dealer_.GetChips() + 2 * bet);
				break;
			case blackjack::RoundResults::draw:
				player_ptr->SetChips(dealer_.GetChips() + bet);
				break;
			case blackjack::RoundResults::playerLose:
				dealer_.SetChips(dealer_.GetChips() + bet);
				break;
			default:
				break;
			}
			player_ptr->GetHand().ClearHand();
		}
		bets_.clear();
	}

	RoundResults Game::CheckWin(std::shared_ptr<Player> player_ptr, Dealer& dealer)
	{
		RoundResults result = RoundResults::playerLose;

		Hand& player_hand = player_ptr->GetHand();
		Hand& dealer_hand = player_ptr->GetHand();

		if (player_hand.IsBlackjack()) {
			if (dealer_hand.IsBlackjack()) {
				result = RoundResults::playerWin32;
			}
			else {
				result = RoundResults::draw;
			}
		}
		else {
			if (dealer_hand.IsBlackjack()) {
				result = RoundResults::playerLose;
			}
			else {
				if (player_hand.IsBusted()) {
					if (dealer_hand.IsBusted()) {
						result = RoundResults::draw;
					}
					else {
						result = RoundResults::playerLose;
					}
				}
				else {
					if (dealer_hand.IsBusted()) {
						result = RoundResults::playerWin11;
					}
					else {
						size_t player_sum = player_hand.GetSumValues();
						size_t dealer_sum = dealer_hand.GetSumValues();
						if (player_sum > dealer_sum) {
							result = RoundResults::playerWin11;
						}
						else {
							if (player_sum < dealer_sum) {
								result = RoundResults::playerLose;
							}
							else {
								result = RoundResults::draw;
							}
						}
					}
				}
			}
		}
		return result;
	}
}
