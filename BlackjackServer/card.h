#pragma once

#include <iostream>

namespace blackjack {

	const size_t kRankSize = 13u;

	enum class Rank {
		ace = 1,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine,
		ten,
		jack,
		queen,
		king
	};

	enum class Suite {
		clubs,
		diamonds,
		hearts,
		spades
	};

	class Card
	{
	public:
		Card(Suite s, Rank r);

		bool IsAce() const;

		Suite GetSuite() const;

		Rank GetRank() const;

		friend std::ostream& operator<< (std::ostream& os, const Card& card);
	private:
		Suite suite_;

		Rank rank_;
	};
}


