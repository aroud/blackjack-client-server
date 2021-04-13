#pragma once

#include <enet/enet.h>
#include "json.hpp"

#include <iostream>
#include <string>
#include <sstream> 
#include <map>

#include "../game.h"
class Server
{
public:
	Server(const std::string host, const uint16_t port, const size_t decks_number);

	void SendENetMessage(std::string message, ENetPeer* peer);

	void MainCycle();

	blackjack::Game& GetGame();
private:
	int InitEnet();

	std::string ParseAndGetResponseInput(ENetPeer* peer, std::string message);

	void ShutdownServer();

	ENetEvent event_;

	std::map<size_t, ENetPeer*> connections_;

	ENetHost* server_host_;

	size_t connection_id_ = 1u;

	std::string message_;

	blackjack::Game game_;
};

