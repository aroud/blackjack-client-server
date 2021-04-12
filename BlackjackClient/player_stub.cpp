#include "player_stub.h"

PlayerStub::PlayerStub(): player_id_(UINT32_MAX) {}

PlayerStub::PlayerStub(size_t id, bool dealer, const std::vector<std::string>& cards, size_t chips) :
	player_id_(id), cards_(cards), chips_(chips) {}