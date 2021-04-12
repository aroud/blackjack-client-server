#include "game.h"

namespace blackjack {

	std::string ToString(Turn turn) {
		std::string result;
		switch (turn)
		{
		case blackjack::Turn::stand:
			result = "stand";
			break;
		case blackjack::Turn::hit:
			result = "hit";
			break;
		case blackjack::Turn::doubleDown:
			result = "double down";
			break;
		case blackjack::Turn::surrender:
			result = "surrender";
			break;
		default:
			break;
		}
		return result;
	}

	std::string ToString(GameStatus status) {
		std::string result;
		switch (status)
		{
		case blackjack::GameStatus::started:
			result = "started";
			break;
		case blackjack::GameStatus::playerRegistration:
			result = "playerRegistration";
			break;
		case blackjack::GameStatus::inRound:
			result = "inRound";
			break;
		case blackjack::GameStatus::ended:
			result = "ended";
			break;
		case blackjack::GameStatus::checkingResults:
			result = "checkingResults";
			break;
		case blackjack::GameStatus::makingBets:
			result = "makingBets";
			break;
		default:
			break;
		}
		return result;
	}

	std::string ToString(Rank rank);
	std::string ToString(Suite suite);

	Game::Game(size_t deck_units_number):
		deck_(deck_units_number),
		cs_(new ConsoleInputSystem()),
		game_status_(GameStatus::started),
		dealer_(chips_constants::kDealerDefaultChipsNumber)
	{}

	void Game::RegisterPlayers()
	{
		game_status_ = GameStatus::playerRegistration;

		//for not client-server version
		/*player_ptr_vect_.push_back(std::make_shared<Player>(cs_, 1, chips_constants::kPlayerDefaultChipsNumber));
		player_ptr_vect_.push_back(std::make_shared<Player>(cs_, 2, chips_constants::kPlayerDefaultChipsNumber));*/
	}

	void Game::BeginRound()
	{
		game_status_ = GameStatus::makingBets;

		std::cout << "Making bets:\n";
		for (auto it = player_ptr_vect_.begin(); it != player_ptr_vect_.end(); ++it)
		{
			auto player_ptr = *it;
			std::cout << *player_ptr;

			//spinlock
			while (!GetActionDone()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			ChangeActionDone();


			size_t player_id = player_ptr->GetID();
			if (current_bet_ && current_bet_ > player_ptr->GetChips()) {
				std::cout << "not enough chips for bet, bet set to 0\n";
				current_bet_ = 0;
			}
			if (current_bet_) {
				player_ptr->SetChips(player_ptr->GetChips() - current_bet_);
				bets_.emplace_back(player_id, current_bet_);
			}
			else {
				std::cout << "Player " << player_ptr->GetID() << " ends up with " << player_ptr->GetChips() << " chips.\n" << std::endl;
			}
		}

		player_ptr_vect_.erase(
			std::remove_if(
				begin(player_ptr_vect_),
				end(player_ptr_vect_),
				[this](std::shared_ptr<Player> player_ptr) {
					size_t player_id = player_ptr->GetID();
					auto it = std::find_if(
						begin(bets_),
						end(bets_),
						[player_id](std::pair<size_t, size_t> p) {return p.first == player_id; }
					);
					return it == end(bets_);
				}),
			end(player_ptr_vect_)
		);

		if (player_ptr_vect_.empty()) {
			ClearGame();
		}
	}

	void Game::PlayRound()
	{
		game_status_ = GameStatus::inRound;

		Card d_card1 = deck_.getCard();
		Card d_card2 = deck_.getCard();
		dealer_.GetHand().AddCard(d_card1);
		dealer_.GetHand().AddCard(d_card2);

		std::cout << "Round:\n" << dealer_ << "\n";
		for (auto player_ptr : player_ptr_vect_)
		{
			Card card1 = deck_.getCard();
			Card card2 = deck_.getCard();
			
			player_ptr->GetHand().AddCard(card1);
			player_ptr->GetHand().AddCard(card2);

			size_t player_id = player_ptr->GetID();
			curr_player_id_ = player_id;

			auto it = std::find_if(
				begin(bets_),
				end(bets_),
				[player_id](std::pair<size_t,size_t> p) {return p.first == player_id; }
			);
			
			do {
				 std::cout << *player_ptr << "makes turn:\n";

				 //spinlock
				 while (!GetActionDone()) {
					 std::this_thread::sleep_for(std::chrono::milliseconds(500));
				 }
				 ChangeActionDone();

				 std::cout << "turn: " << ToString(current_turn_) << std::endl;

				 player_ptr->DoTurnActions(current_turn_, deck_, it->second);

				 if (current_turn_ == Turn::doubleDown) {
					 player_ptr->SetChips(player_ptr->GetChips() - it->second / 2);
				 }
			} while (current_turn_ == Turn::hit);

			std::cout << "After turn actions: " << *player_ptr << "bet:" << it->second << "\n\n";
		}

		curr_player_id_ = std::numeric_limits<size_t>::max();

		Turn dealer_turn = Turn::stand;
		do {
			dealer_turn = dealer_.MakeTurn();
			size_t i = 0;
			dealer_.DoTurnActions(dealer_turn, deck_, std::ref(i));
		} while (dealer_turn == Turn::hit);
	}

	void Game::EndRound()
	{
		game_status_ = GameStatus::checkingResults;

		std::cout << "Checking results:\n";
		for (auto player_ptr : player_ptr_vect_)
		{
			size_t player_id = player_ptr->GetID();

			auto it = std::find_if(
				begin(bets_),
				end(bets_),
				[player_id](std::pair<size_t, size_t> p) {return p.first == player_id; }
			);
			size_t bet = it->second;
			std::cout << "Player: " << player_id << ", bet: " << bet << "\n";

			RoundResults result = RoundResults::playerLose;
			if (!player_ptr->has_surrended) {
				result = CheckWin(player_ptr);
			}
			else {
				result = RoundResults::draw;
				dealer_.SetChips(dealer_.GetChips() + bet);
			}

			player_ptr->has_surrended = false;
			player_ptr->made_turn = false;
			
			switch (result)
			{
			case blackjack::RoundResults::playerWin32:
				player_ptr->SetChips(player_ptr->GetChips() + 5 * bet / 2);
				dealer_.SetChips(dealer_.GetChips() - 3 * bet / 2);
				break;
			case blackjack::RoundResults::playerWin11:
				player_ptr->SetChips(player_ptr->GetChips() + 2 * bet);
				dealer_.SetChips(dealer_.GetChips() - bet);
				break;
			case blackjack::RoundResults::draw:
				player_ptr->SetChips(player_ptr->GetChips() + bet);
				break;
			case blackjack::RoundResults::playerLose:
				dealer_.SetChips(dealer_.GetChips() + bet);
				break;
			default:
				break;
			}
			player_ptr->GetHand().ClearHand();
			std::cout << "After round end: " << *player_ptr << "\n";
		}
		bets_.clear();
		std::cout << "After round end, dealer: " << dealer_ << "\n";
		dealer_.GetHand().ClearHand();
		dealer_.made_turn = false;
	}

	void Game::PlayGame()
	{
		std::cout << "Initial players:\n";
		for (auto ptr : player_ptr_vect_) {
			std::cout << *ptr;
		}
		std::cout << std::endl;
		while (true) {
			BeginRound();

			std::cout << "After bets:\n";
			if (player_ptr_vect_.empty()) {
				std::cout << "No players left\n";
				break;
			}
			else {
				for (auto ptr : player_ptr_vect_) {
					std::cout << *ptr;
				}
				std::cout << "Bets:\n";
				for (auto& p : bets_) {
					std::cout << "Player: " << p.first << ", bet: " << p.second << "\n";
				}
				std::cout << std::endl;

				PlayRound();

				EndRound();
			}
		}
		ClearGame();
	}

	void Game::PlayGameMultiThread()
	{
		std::thread t(&Game::PlayGame, this);
		t.join();
	}

	std::string Game::ToJson()
	{
		using json = nlohmann::json;
		json j;

		j["type"] = "game_info";

		j["min_bet"] = chips_constants::kMinBet;
		j["max_bet"] = chips_constants::kMaxBet;

		j["game_status"] = ToString(game_status_);

		j["curr_player_id"] = curr_player_id_;

		std::vector < std::pair<size_t, std::vector<std::string>>> player_cards;
		std::vector < std::pair<size_t, size_t>> player_chips;

		bool player_add_turn_field_added = false;
		for (auto ptr : player_ptr_vect_) {
			auto& hand = ptr->GetHand();
			std::vector<std::string> cards;
			for (size_t i = 0; i < hand.GetSize(); ++i) {
				cards.push_back(ToString(hand[i].GetRank()) + " " + ToString(hand[i].GetSuite()));
			}
			player_cards.emplace_back(ptr->GetID(), cards);
			player_chips.emplace_back(ptr->GetID(), ptr->GetChips());
			if (ptr->GetID() == curr_player_id_) {
				j["player_made_turn"] = ptr->made_turn;
				player_add_turn_field_added = true;
			}
		}

		if (!player_add_turn_field_added) {
			j["player_made_turn"] = false;
		}
		
		j["player_cards"] = player_cards;
		j["player_chips"] = player_chips;
		j["dealer_chips"] = dealer_.GetChips();

		std::vector<std::string> dealer_cards;
		auto& dealer_hand = dealer_.GetHand();
		if (dealer_hand.GetSize() == 2u) {
			dealer_cards.push_back(ToString(dealer_hand[0].GetRank()) + " " + ToString(dealer_hand[0].GetSuite()));
			dealer_cards.push_back("(?)");
		}
		else {
			for (size_t i = 0; i < dealer_hand.GetSize(); ++i) {
				dealer_cards.push_back(ToString(dealer_hand[i].GetRank()) + " " + ToString(dealer_hand[i].GetSuite()));
			}
		}
		j["dealer_cards"] = dealer_cards;

		return j.dump();
	}

	void Game::ClearGame()
	{
		std::cout << "Game ended. Clearing game resources.";
		game_status_ = GameStatus::ended;
		player_ptr_vect_.clear();
		bets_.clear();
		dealer_.SetChips(chips_constants::kDealerDefaultChipsNumber);
		dealer_.GetHand().ClearHand();
	}

	void Game::ChangeActionDone()
	{
		bool b = action_done_.load();
		action_done_.store(!b);
	}

	bool Game::GetActionDone() const
	{
		return action_done_.load();
	}


	GameStatus Game::GetGameStatus() const
	{
		return game_status_;
	}

	void Game::AddPlayer(size_t id)
	{
		player_ptr_vect_.push_back(std::make_shared<Player>(cs_, id, chips_constants::kPlayerDefaultChipsNumber));
	}

	RoundResults Game::CheckWin(std::shared_ptr<Player> player_ptr)
	{
		RoundResults result = RoundResults::playerLose;

		Hand& player_hand = player_ptr->GetHand();
		Hand& dealer_hand = dealer_.GetHand();

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
