#include <enet/enet.h>
#include "json.hpp"

#include <iostream>
#include <string>
#include <sstream> 
#include <map>

#include "game.h"

int InitEnet() {
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    return 0;
}


std::string PacketToString(const ENetPacket* packet) {
    std::ostringstream oss;
    for (size_t i = 0; i < packet->dataLength; ++i) {
        oss << packet->data[i];
    }
    std::string res = oss.str();
    res.pop_back();
    return res;
}

std::string ToJson(const std::string& type, const std::string& body) {
    nlohmann::json j;
    j["type"] = type;
    j["body"] = body;
    return j.dump();
}

std::string ParseAndGetResponseInput(blackjack::Game& game, ENetPeer* peer,
    std::map<size_t, ENetPeer*>& connections, std::string message, size_t& connection_id) {

    if (message == "register") {
        if (game.GetGameStatus() == blackjack::GameStatus::playerRegistration )
        {  
            connections[connection_id] = peer;
            std::ostringstream oss;
            oss << connection_id;
            message = oss.str();
            game.AddPlayer(connection_id);
            ++connection_id;
            return ToJson("id", message);
        }
        else {
            message = "0";
            return ToJson("id", message);
        }
    }
    else if (message == "end registration") {
        if (game.GetGameStatus() == blackjack::GameStatus::playerRegistration) {
            game.PlayGameMultiThread();
            return ToJson("msg", "Registration ended");
        }
    }
    else if (message == "info") {
        return game.ToJson();
    }
    else if (message == "startGame") {
        if (game.GetGameStatus() == blackjack::GameStatus::ended) {
            game.RegisterPlayers();
            return ToJson("msg", "A new game started!\n");
        }
    }
    else {
        std::istringstream iss(message);
        std::string msg_part;
        iss >> msg_part;
        if (msg_part == "turn") {
            iss >> msg_part;
            if (msg_part == "stand") {
                game.current_turn_ = blackjack::Turn::stand;
                game.ChangeActionDone();
                return ToJson("msg", "Stand turn made.\n");
            }
            else if (msg_part == "hit") {
                game.current_turn_ = blackjack::Turn::hit;
                game.ChangeActionDone();
                return ToJson("msg", "Hit turn made.\n");
            }
            else if (msg_part == "doubleDown") {
                game.current_turn_ = blackjack::Turn::doubleDown;
                game.ChangeActionDone();
                return ToJson("msg", "DoubleDown turn made.\n");
            }
            else if (msg_part == "surrender") {
                game.current_turn_ = blackjack::Turn::surrender;
                game.ChangeActionDone();
                return ToJson("msg", "Surrender turn made.\n");
            }
            else {
                return ToJson("msg", "Unknown kommand.\n");
            }
        }
        else if (msg_part == "bet") {
            size_t bet;
            iss >> bet;
            game.current_bet_ = bet;
            game.ChangeActionDone();
            std::string res = "Bet made: " +std::to_string(bet);
            return ToJson("msg", res);
        }
        else {
            return ToJson("msg", "Unknown kommand.\n");
        }
    }
    return "";
}

void SendENetMessage(std::string message, ENetPeer* peer) {
    if (message.length()) {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
}

int main()
{
    //Initialize enet
    if (InitEnet()) {
        return EXIT_FAILURE;
    }

    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 6789;

    ENetHost* server = enet_host_create(
        &address,   /* the address to bind the server host to */
        32,         /* allow up to 32 clients and/or outgoing connections */
        2,          /* allow up to 2 channels to be used, 0 and 1 */
        0,          /* assume any amount of incoming bandwidth, 0 for enet dynamic throttling algorithm */
        0           /* assume any amount of outgoing bandwidth, 0 for enet dynamic throttling algorithm */
    );
    if (server == NULL) {
        std::cerr << "An error occured while trying to create an ENet server host\n";
        exit(EXIT_FAILURE);
    }

    //create Game instance (4 52-card decks) and start registaration
    blackjack::Game game(4u);
    game.RegisterPlayers();

    ENetEvent event;
    int eventStatus;
    eventStatus = 1;

    std::map<size_t, ENetPeer*> connections;
    size_t connection_id = 1u;
    std::string message;

    while (1) {
        eventStatus = enet_host_service(server, &event, 50000);

        if (eventStatus > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "(Server) We got a new connection from\n" << event.peer->address.host << " host, " <<
                    event.peer->address.port << " port\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << "A packet containing " << event.packet->data << " was received.\n";
                
                message = ParseAndGetResponseInput(game, event.peer, connections, PacketToString(event.packet), connection_id);
                if (!message.empty()) {
                    std::cout << "sending message: " << message << "\n";
                    SendENetMessage(message, event.peer);
                }
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << event.peer->data << " disconnected.\n";
                // Reset client's information
                for (auto& p : connections) {
                    if (p.second == event.peer) {
                        connections.erase(p.first);
                    }
                }
                event.peer->data = NULL;
                break;
            }
        }
    }
    return 0;
}
