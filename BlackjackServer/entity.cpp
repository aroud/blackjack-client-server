#include "entity.h"

namespace blackjack {

	std::ostream& operator<<(std::ostream& os, const Entity& entity) {
		entity.PrintHand(os);
		return os;
	}

	Entity::Entity(size_t initial_chips): hand_(), chips_(initial_chips) {}

	void Entity::DoTurnActions(Turn turn, Deck& deck, size_t& bet)
	{
		switch (turn)
		{
		case blackjack::Turn::stand:
			break;
		case blackjack::Turn::doubleDown:
			bet *= 2;
			[[fallthrough]];
		case blackjack::Turn::hit: {
			Card card = deck.getCard();
			hand_.AddCard(card);
		}
			break;
		case blackjack::Turn::surrender:
			bet /= 2;
			has_surrended = true;
			break;
		default:
			break;
		}
	}

	size_t Entity::GetChips()
	{
		return chips_;
	}

	void Entity::SetChips(size_t chips)
	{
		chips_ = chips;
	}

	Hand& Entity::GetHand()
	{
		return hand_;
	}
}
