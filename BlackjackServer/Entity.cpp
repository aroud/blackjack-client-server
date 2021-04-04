#include "entity.h"

namespace blackjack {
	std::ostream& operator<<(std::ostream& os, const Entity& entity) {
		entity.PrintHand(os);
		return os;
	}

	Entity::Entity() {}
}
