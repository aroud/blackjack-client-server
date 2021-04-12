#include "game_stub.h"

GameStub::GameStub():	min_bet_(client_chips_constants::kMinBet),
						max_bet_(client_chips_constants::kMaxBet)
{ dealer_.chips_ = blackjack::chips_constants::kDealerDefaultChipsNumber; };