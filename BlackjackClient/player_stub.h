#pragma once

#include <string>
#include <vector>

namespace client_chips_constants {
	const size_t kPlayerDefaultChipsNumber = 200;
	const size_t kDealerDefaultChipsNumber = 2000;
	const size_t kMinBet = 5;
	const size_t kMaxBet = 15;
}

class PlayerStub
{
public:
	PlayerStub();
	PlayerStub(size_t id, bool dealer, const std::vector<std::string>& cards, size_t chips);
	size_t player_id_;
	std::vector<std::string> cards_;
	size_t chips_ = client_chips_constants::kPlayerDefaultChipsNumber;
	bool is_dealer_ = false;
};

