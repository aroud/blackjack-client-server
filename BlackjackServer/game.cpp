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

	nlohmann::json ToJson(std::string& type, std::string& body) {
		nlohmann::json j;
		j["type"] = type;
		j[]
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

		player_ptr_vect_.push_back(std::make_shared<Player>(cs_, 1, chips_constants::kPlayerDefaultChipsNumber));
		player_ptr_vect_.push_back(std::make_shared<Player>(cs_, 2, chips_constants::kPlayerDefaultChipsNumber));
	}

	void Game::BeginRound()
	{
		game_status_ = GameStatus::makingBets;

		std::cout << "Making bets:\n";
		for (auto it = player_ptr_vect_.begin(); it != player_ptr_vect_.end(); ++it)
		{
			auto player_ptr = *it;
			std::cout << *player_ptr;
			size_t bet = player_ptr->control_system_->StartRound(chips_constants::kMinBet, chips_constants::kMaxBet);
			std::cout << "\n";
			size_t player_id = player_ptr->GetID();
			if (bet && bet > player_ptr->GetChips()) {
				std::cout << "not enough chips for bet, bet set to 0\n";
				bet = 0;
			}
			if (bet) {
				player_ptr->SetChips(player_ptr->GetChips() - bet);
				bets_.emplace_back(player_id, bet);
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
			std::cout << ToJson() << "\n";
			Turn turn = Turn::stand;
			do {
				std::cout << *player_ptr << "makes turn:\n";
				 turn = player_ptr->MakeTurn();

				 std::cout << "turn: " << ToString(turn) << std::endl;

				 player_ptr->DoTurnActions(turn, deck_, it->second);

				 if (turn == Turn::doubleDown) {
					 player_ptr->SetChips(player_ptr->GetChips() - it->second / 2);
				 }
			} while (turn == Turn::hit);

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
		std::cout << ToJson() << "\n";
		RegisterPlayers();
		std::cout << ToJson() << "\n";
		std::cout << "Initial players:\n";
		for (auto ptr : player_ptr_vect_) {
			std::cout << *ptr;
		}
		std::cout << std::endl;
		while (true) {
			BeginRound();
			std::cout << ToJson() << "\n";
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
				std::cout << ToJson() << "\n";
				PlayRound();

				EndRound();
			}
		}
		game_status_ = GameStatus::ended;

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
			}
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
