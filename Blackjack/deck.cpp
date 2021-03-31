#include "deck.h"

namespace blackjack {
	Deck::Deck()
	{
		InitCards();
		Shuffle();
	}
	void Deck::InitCards()
	{
		cards_.reserve(kDeckSize);

	}
	void Deck::Shuffle()
	{
	}
}
