#pragma once

#include <enet/enet.h>
#include "json.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <thread>
#include <chrono>

#include "game.h"
#include "../game_stub.h"

class Client
{
public:
	Client(const std::string server_host, const uint16_t server_port);

	void SendENetMessage(std::string message, ENetPeer* peer);

	void MainCycle();

	GameStub& GetGameStub();
private:
	bool HandleMessage();

	void PrintSituation(bool print_cards) const;

	ENetPeer* GetPeer(const std::string& host, const uint16_t port) const;

	int InitEnet();

	ENetEvent event_;

	std::string message_;

	GameStub game_stub_;

	ENetPeer* peer_;

	ENetHost* client_host_;

	bool to_end = false;
};

