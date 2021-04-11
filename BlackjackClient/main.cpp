#include <enet/enet.h>
#include "json.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <thread>
#include <chrono>

#include "game.h"
#include "game_stub.h"

int InitEnet() {
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    return 0;
}

void SendENetMessage(std::string message, ENetPeer* peer) {
    if (message.length()) {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
}

void PrintSituation(GameStub& game_stub) {
    std::cout << "Current situation:\n";
    for (auto& p : game_stub.players_)
    {
        std::cout << "Player " << p.player_id_ << ", cards: ";
        for (auto& c : p.cards_) {
            std::cout << c << "\t";
        }
        std::cout << "\nChips: " << p.chips_ << "\n";
    }
}

void HandleMessage(std::string& message, GameStub& game_stub, ENetPeer* peer) {
    nlohmann::json j = message;
    std::string type = j["type"].get<std::string>();

    if (type == "game_info") {

        std::string game_status = j["game_status"].get<std::string>();

        size_t curr_player_id = j["curr_player_id"].get<size_t>();

        game_stub.min_bet_ = j["min_bet"];
        game_stub.max_bet_ = j["max_bet"];
   
        std::vector < std::pair<size_t, std::vector<std::string>>> player_cards = 
            j["player_cards"].get<std::vector < std::pair<size_t, std::vector<std::string>>>>();
        std::vector < std::pair<size_t, size_t>> player_chips =
            j["player_chips"].get<std::vector < std::pair<size_t, size_t>>>();
        bool player_made_turn =
            j["player_made_turn"].get<bool>();

        size_t num = player_cards.size();

        game_stub.players_.clear();
        for (size_t i = 0; i < num; ++i) {
            game_stub.players_.emplace_back(player_cards[i].first, false, player_cards[i].second, player_chips[i].second);
            game_stub.players_[i].cards_ = player_cards[i].second;
            game_stub.players_[i].chips_ = player_chips[i].second;
            game_stub.players_[i].is_dealer_ = false;
        }

        game_stub.dealer_.cards_ = j["dealer_cards"].get<std::vector<std::string>>();
        game_stub.dealer_.chips_ = j["dealer_chips"].get<size_t>();

        if (game_status == "ended") {
            std::cout << "Sent message to start the game.\n";
            SendENetMessage("startGame", peer);
        }
        if (game_status == "playerRegistration") {
            std::cout << "Register the game? (y/n)\n";
            std::string temp;
            std::cin >> temp;
            if (temp == "y") {
                std::cout << "Registering to the game...\n";
                SendENetMessage("register", peer);

                std::cout << "End registration? (y/n)\n";
                std::cin >> temp;
                if (temp == "y") {
                    SendENetMessage("end registration", peer);
                }
            }
            else {
                std::cout << "Not registering to the game.\n";
            }
        }
        else if (game_status == "inRound") {
            
            if (curr_player_id == game_stub.client_idx_) {
                PrintSituation(game_stub);
                
                blackjack::Turn turn = game_stub.cs_.GetTurn(game_stub.player_made_turn_);
                std::string result;
                switch (turn)
                {
                case blackjack::Turn::stand:
                    result = "turn stand";
                    break;
                case blackjack::Turn::hit:
                    result = "turn hit";
                    break;
                case blackjack::Turn::doubleDown:
                    result = "turn doubleDown";
                    break;
                case blackjack::Turn::surrender:
                    result = "turn surrender";
                    break;
                default:
                    break;
                }
                SendENetMessage(result, peer);
            }
        }
        else if (game_status == "makingBets") {
            PrintSituation(game_stub);
            if (curr_player_id == game_stub.client_idx_) {
                size_t bet = game_stub.cs_.StartRound(game_stub.min_bet_, game_stub.max_bet_);
                std::string bet_str = "bet " + bet;
                SendENetMessage(bet_str, peer);
            }
        }
    }
    else if (type == "id") {
        std::string b = j["body"].get<std::string>();
        std::cout << "Got message with id: " << b << "\n";
        if (1 == scanf("%zu", &game_stub.client_idx_)) {
            if (game_stub.client_idx_ == 0) {
                //server refused this connection by giving 0 id
                enet_peer_disconnect(peer, 0);
            }
        }
        else {
            std::cerr << "Id parsing error" << std::endl;
        }
    }
    else {
        std::cout << j["body"].get<std::string>() << "\n";
    }
}

ENetPeer* GetPeer(ENetHost* client, std::string host, size_t port) {
    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;
    return enet_host_connect(client, &address, 2, 0);
}


std::string PacketToString(const ENetPacket* packet) {
    std::ostringstream oss;
    for (size_t i = 0; i < packet->dataLength - 1; ++i) {
        oss << packet->data[i];
    }
    return oss.str();
}


int main(int argc, char** argv)
{
    if (InitEnet()) {
        return EXIT_FAILURE;
    }
   
    ENetHost* client = enet_host_create(
        NULL,   /* create a client host */
        1,      /* only allow 1 outgoing connection */
        2,      /* allow up 2 channels to be used, 0 and 1 */
        0,      /* assume any amount of outgoing bandwidth */
        0       /* assume any amount of outgoing bandwidth */
    );

    if (client == NULL) {
        std::cerr << "An error occured while trying to create an ENet server host\n";
        return EXIT_FAILURE;
    }

    ENetPeer* peer = GetPeer(client, "localhost", 6789);

    ENetEvent event;
    int eventStatus = 1;
    std::string got_message;
    GameStub game_stab;

    while (true) {
        eventStatus = enet_host_service(client, &event, 5000);

        if (eventStatus > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Got connection from: " << event.peer->address.host << std::endl;
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                got_message = PacketToString(event.packet);
                std::cout << got_message;
                HandleMessage(got_message, game_stab, peer);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client " << event.peer->data << " disconnected.\n";
                event.peer->data = NULL;
                break;
            }
        }
        SendENetMessage("info", peer);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}