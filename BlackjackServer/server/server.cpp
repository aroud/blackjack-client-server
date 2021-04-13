#include "server.h"

std::string PacketToString(const ENetPacket* packet);
std::string ToJson(const std::string& type, const std::string& body);

Server::Server(const std::string host, const uint16_t port, const size_t decks_number):
    game_(decks_number)
{
    /* Initialize enet enviroment */
    if (InitEnet()) {
        throw std::runtime_error("Enet initialization failed");
    }

    /* Bind the server to the adress */
    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    server_host_ = enet_host_create(
        &address,   /* the address to bind the server host to */
        32,         /* allow up to 32 clients and/or outgoing connections */
        2,          /* allow up to 2 channels to be used, 0 and 1 */
        0,          /* assume any amount of incoming bandwidth, 0 for enet dynamic throttling algorithm */
        0           /* assume any amount of outgoing bandwidth, 0 for enet dynamic throttling algorithm */
    );

    if (server_host_ == NULL) {
        throw std::runtime_error("Host creation failed");
    }
}


void Server::SendENetMessage(std::string message, ENetPeer* peer)
{
    if (message.length()) {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
}

void Server::MainCycle()
{
    int eventStatus = 1;
    while (1) {
        eventStatus = enet_host_service(server_host_, &event_, 0);

        if (eventStatus > 0) {
            switch (event_.type) {
            case ENET_EVENT_TYPE_CONNECT:

                std::cout << "Server got a new connection from\n" << event_.peer->address.host << " host, " <<
                    event_.peer->address.port << " port\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE:

                std::cout << "A packet containing " << event_.packet->data << " was received.\n";

                message_ = ParseAndGetResponseInput(event_.peer, PacketToString(event_.packet));
                if (!message_.empty()) {
                    std::cout << "sending message: " << message_ << "\n";
                    SendENetMessage(message_, event_.peer);
                }
                break;

            case ENET_EVENT_TYPE_DISCONNECT:

                // Reset client's information
                auto iter = std::find_if(
                    begin(connections_),
                    end(connections_),
                    [this](std::pair<size_t, ENetPeer*> p) {
                        return p.second->address.host == event_.peer->address.host &&
                            p.second->address.port == event_.peer->address.port;
                    }
                );

                if (game_.GetGameStatus() == blackjack::GameStatus::started ||
                    game_.GetGameStatus() == blackjack::GameStatus::playerRegistration ||
                    game_.GetGameStatus() == blackjack::GameStatus::ended)
                {
                    size_t id = iter->first;
                    game_.RemovePlayer(id);
                    iter = connections_.erase(iter);
                }
                else {
                    std::cout << "Ending game by removing clients.\n";
                    ShutdownServer();
                }

                event_.peer->data = NULL;
                break;
            }
        }
    }
}

blackjack::Game& Server::GetGame()
{
    return game_;
}

int Server::InitEnet()
{
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return 1;
    }
    atexit(enet_deinitialize);
	return 0;
}

std::string Server::ParseAndGetResponseInput(ENetPeer* peer, std::string message)
{
    if (message == "register") {
        if (game_.GetGameStatus() == blackjack::GameStatus::playerRegistration)
        {
            connections_[connection_id_] = peer;
            std::ostringstream oss;
            oss << connection_id_;
            message = oss.str();
            game_.AddPlayer(connection_id_);
            ++connection_id_;
            return ToJson("id", message);
        }
        else {
            message = "0";
            return ToJson("id", message);
        }
    }
    else if (message == "end registration") {
        if (game_.GetGameStatus() == blackjack::GameStatus::playerRegistration) {
            game_.PlayGameMultiThread();
            return ToJson("msg", "Registration ended");
        }
        else {
            return "";
        }
    }
    else if (message == "info") {
        return game_.ToJson();
    }
    else if (message == "startGame") {
        if (game_.GetGameStatus() == blackjack::GameStatus::ended) {
            connections_.clear();
            game_.RegisterPlayers();
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
                game_.current_turn_ = blackjack::Turn::stand;
                game_.ChangeActionDone();
                return ToJson("msg", "Stand turn made.\n");
            }
            else if (msg_part == "hit") {
                game_.current_turn_ = blackjack::Turn::hit;
                game_.ChangeActionDone();
                return ToJson("msg", "Hit turn made.\n");
            }
            else if (msg_part == "doubleDown") {
                game_.current_turn_ = blackjack::Turn::doubleDown;
                game_.ChangeActionDone();
                return ToJson("msg", "DoubleDown turn made.\n");
            }
            else if (msg_part == "surrender") {
                game_.current_turn_ = blackjack::Turn::surrender;
                game_.ChangeActionDone();
                return ToJson("msg", "Surrender turn made.\n");
            }
            else {
                return ToJson("msg", "Unknown kommand.\n");
            }
        }
        else if (msg_part == "bet") {
            size_t bet;
            iss >> bet;
            game_.current_bet_ = bet;
            game_.ChangeActionDone();
            std::string res = "Bet made: " + std::to_string(bet);
            return ToJson("msg", res);
        }
        else {
            return ToJson("msg", "Unknown kommand.\n");
        }
    }
    return "";
}

void Server::ShutdownServer()
{
    game_.ClearGame();
}
