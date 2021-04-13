#include "client.h"

std::string PacketToString(const ENetPacket* packet);

Client::Client(const std::string server_host, const uint16_t server_port)
{
    /* Initialize enet enviroment */
    if (InitEnet()) {
        throw std::runtime_error("Enet initialization failed");
    }

    client_host_ = enet_host_create(
        NULL,   /* create a client host */
        1,      /* only allow 1 outgoing connection */
        2,      /* allow up 2 channels to be used, 0 and 1 */
        0,      /* assume any amount of outgoing bandwidth */
        0       /* assume any amount of outgoing bandwidth */
    );

    if (client_host_ == NULL) {
        throw std::runtime_error("Host creation failed");
    }

    peer_ = GetPeer(server_host, server_port);

}

void Client::SendENetMessage(std::string message, ENetPeer* peer)
{
    if (message.length()) {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
}

void Client::MainCycle()
{
    int eventStatus = 1;

    std::string got_message;

    bool skip = false;

    bool disconnected = false;

    while (true) {
        eventStatus = enet_host_service(client_host_, &event_, 500);

        if (eventStatus > 0) {
            switch (event_.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Got connected to the server.\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                message_ = PacketToString(event_.packet);
                skip = HandleMessage();
                enet_packet_destroy(event_.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected.\n";
                disconnected = true;
                event_.peer->data = NULL;
                break;
            }
        }

        if (disconnected) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        if (!skip) {
            SendENetMessage("info", peer_);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        else {
            skip = false;
        }
    }
}

bool Client::HandleMessage()
{
    nlohmann::json j = nlohmann::json::parse(message_);

    std::string type = j["type"].get<std::string>();

    if (type == "game_info") {
        std::string game_status = j["game_status"].get<std::string>();
        size_t curr_player_id = j["curr_player_id"].get<size_t>();

        game_stub_.min_bet_ = j["min_bet"];
        game_stub_.max_bet_ = j["max_bet"];

        std::vector < std::pair<size_t, std::vector<std::string>>> player_cards =
            j["player_cards"].get<std::vector < std::pair<size_t, std::vector<std::string>>>>();

        std::vector < std::pair<size_t, size_t>> player_chips =
            j["player_chips"].get<std::vector < std::pair<size_t, size_t>>>();

        bool player_made_turn =
            j["player_made_turn"].get<bool>();
        game_stub_.player_made_turn_ = player_made_turn;

        size_t num = player_cards.size();

        game_stub_.players_.clear();
        for (size_t i = 0; i < num; ++i) {
            game_stub_.players_.emplace_back(player_cards[i].first, false, player_cards[i].second, player_chips[i].second);
            game_stub_.players_[i].cards_ = player_cards[i].second;
            game_stub_.players_[i].chips_ = player_chips[i].second;
            game_stub_.players_[i].is_dealer_ = false;
        }

        game_stub_.dealer_.cards_ = j["dealer_cards"].get<std::vector<std::string>>();
        game_stub_.dealer_.chips_ = j["dealer_chips"].get<size_t>();

        if (game_stub_.printed_once)
        {
            PrintSituation(false);
            game_stub_.printed_once = !game_stub_.printed_once;
        }

        if (game_status == "ended") {
            std::cout << "Starting a new game.\n";
            SendENetMessage("startGame", peer_);
            return true;
        }
        if (game_status == "playerRegistration") {
            if (game_stub_.client_idx_ == UINT32_MAX && !game_stub_.try_to_get_id_) {
                std::cout << "Register to the game? (y/n)\n";
                std::string temp;
                std::cin >> temp;
                if (temp == "y") {
                    std::cout << "Registering to the game...\n";
                    SendENetMessage("register", peer_);
                    game_stub_.try_to_get_id_ = true;

                }
                else {
                    std::cout << "Not registering to the game.\n";
                    enet_peer_disconnect(peer_, 0);
                }
                return true;
            }
            else if (game_stub_.client_idx_) {
                std::string temp;
                std::cout << "End registration? (y/n)\n";
                std::cout << "Currently ";
                size_t players_num = game_stub_.players_.size();
                if (players_num == 1) {
                    std::cout << "1 player in the game.\n";
                }
                else {
                    std::cout << players_num << " players in the game\n";
                }

                std::cin >> temp;
                if (temp == "y") {
                    std::cout << "Ending registration.\n";
                    SendENetMessage("end registration", peer_);
                    return true;
                }
                else {
                    return false;
                }
            }
        }
        else if (game_status == "inRound") {
            if (curr_player_id == game_stub_.client_idx_) {
                PrintSituation(true);

                blackjack::Turn turn = game_stub_.cs_.GetTurn(game_stub_.player_made_turn_);
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
                game_stub_.printed_once = true;
                SendENetMessage(result, peer_);
                return true;
            }
        }
        else if (game_status == "makingBets") {
            if (curr_player_id == game_stub_.client_idx_) {
                PrintSituation(false);
                size_t bet = game_stub_.cs_.StartRound(game_stub_.min_bet_, game_stub_.max_bet_);
                std::string bet_str = "bet " + std::to_string(bet);
                if (bet) {
                    SendENetMessage(bet_str, peer_);
                }
                else {
                    enet_peer_disconnect(peer_, 0);
                }
                return true;
            }
        }

        if (game_stub_.client_idx_ == UINT32_MAX && !game_stub_.try_to_get_id_)
        {
            std::cout << "Disconnecting, not entering the game\n";
            enet_peer_disconnect(peer_, 0);
            return true;
        }
        return false;
    }
    else if (type == "id") {
        std::string body = j["body"].get<std::string>();
        std::istringstream iss(body);
        size_t id = 0;
        iss >> id;
        std::cout << "Got message with id: " << id << "\n";

        if (game_stub_.client_idx_ == 0) {
            //server refused this connection by giving 0 id
            std::cout << "Got 0 player idx, disconnecting";
            enet_peer_disconnect(peer_, 0);
            return true;
        }
        else {
            game_stub_.client_idx_ = id;
            std::cout << "player id: " << game_stub_.client_idx_ << "\n";
        }
        return false;
    }
    else {
        std::cout << j["body"].get<std::string>() << "\n";
        return false;
    }
}

void Client::PrintSituation(bool print_cards) const
{
    std::cout << "\nCurrent situation:\n";
    for (auto& p : game_stub_.players_)
    {
        std::cout << "Player " << p.player_id_;
        if (print_cards) {
            std::cout<< ", cards: ";
            if (p.cards_.empty()) {
                std::cout << "<currently no cards>";
            }
            else {
                for (auto& c : p.cards_) {
                    std::cout <<"("<< c << ") ";
                }
            }
        }
        std::cout << "\nChips: " << p.chips_ << "\n";
    }
    std::cout << "Dealer: ";
    if (print_cards) {
        for (auto& c : game_stub_.dealer_.cards_) {
            std::cout << c << " ";
        }
    }
    std::cout << "\nChips: " << game_stub_.dealer_.chips_ << "\n\n";
}

ENetPeer* Client::GetPeer(const std::string& host, const uint16_t port) const
{
    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;
    return enet_host_connect(client_host_, &address, 2, 0);
}


int Client::InitEnet()
{
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    return 0;
}