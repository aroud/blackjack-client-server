#include "deck.h"
#include "hand.h"

int main() {
	blackjack::Hand hand;
	blackjack::Card c1(blackjack::Suite::clubs, blackjack::Rank::king);
	blackjack::Card c2(blackjack::Suite::clubs, blackjack::Rank::king);
	blackjack::Card c3(blackjack::Suite::clubs, blackjack::Rank::queen);
	hand.AddCard(c1);
	hand.AddCard(c2);
	hand.AddCard(c3);
	std::cout << hand.IsBlackjack() << std::endl << hand.GetSumValues() << std::endl;
	return 0;
}