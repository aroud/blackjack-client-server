#include "card.h"

namespace blackjack {

	std::string ToString(Rank rank) {
		std::string result;
		switch (rank)
		{
		case blackjack::Rank::ace:
			result = "ace";
			break;
		case blackjack::Rank::two:
			result = "2";
			break;
		case blackjack::Rank::three:
			result = "3";
			break;
		case blackjack::Rank::four:
			result = "4";
			break;
		case blackjack::Rank::five:
			result = "5";
			break;
		case blackjack::Rank::six:
			result = "6";
			break;
		case blackjack::Rank::seven:
			result = "7";
			break;
		case blackjack::Rank::eight:
			result = "8";
			break;
		case blackjack::Rank::nine:
			result = "9";
			break;
		case blackjack::Rank::ten:
			result = "10";
			break;
		case blackjack::Rank::jack:
			result = "jack";
			break;
		case blackjack::Rank::queen:
			result = "queen";
			break;
		case blackjack::Rank::king:
			result = "king";
			break;
		default:
			break;
		}
		return result;
	}

	std::string ToString(Suite suite) {
		std::string result;
		switch (suite)
		{
		case blackjack::Suite::clubs:
			result = "clubs";
			break;
		case blackjack::Suite::diamonds:
			result = "diamonds";
			break;
		case blackjack::Suite::hearts:
			result = "hearts";
			break;
		case blackjack::Suite::spades:
			result = "spades";
			break;
		default:
			break;
		}
		return result;
	}

	Card::Card(Suite s, Rank r) :suite_(s), rank_(r) {};

	bool Card::IsAce() const {
		return rank_ == Rank::ace;
	}

	Rank Card::GetRank() const {
		return rank_;
	}

	Suite Card::GetSuite() const {
		return suite_;
	}

	std::ostream& operator<<(std::ostream& os, const Card& card)
	{
		os << ToString(card.GetRank()) << " " << ToString(card.GetSuite()) << "\n";
		return os;
	}
}
