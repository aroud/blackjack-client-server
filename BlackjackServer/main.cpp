#include "game.h"

int main() {
	blackjack::Game game(2);
	game.BeginRound();
	game.PlayRound();
	game.EndRound();
	return 0;
}