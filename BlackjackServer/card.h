#pragma once

namespace blackjack {

	const size_t kRankSize = 13;

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
	private:
		Suite suite_;
		Rank rank_;
	};
}


