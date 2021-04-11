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
    for (size_t i = 0; i < packet->dataLength - 1; ++i) {
        oss << packet->data[i];
    }
    return oss.str();
}


std::string ParseAndGetResponseInput(blackjack::Game& game, ENetPeer*, std::map<size_t, ENetPeer*>, std::string message) {

}

void SendENetMessage(std::string message, ENetPeer* peer) {
    if (message.length()) {
        ENetPacket* packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
}

int main(int argc, char** argv)
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
        eventStatus = enet_host_service(server, &event, 10000);

        if (eventStatus > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "(Server) We got a new connection from\n" << event.peer->address.host << " host, " <<
                    event.peer->address.port << " port\n";

                if (game.game_status_ == blackjack::GameStatus::playerRegistration ||
                    game.game_status_ == blackjack::GameStatus::ended
                    ) {
                    connections[connection_id] = event.peer;
                    message = "id " + connection_id;
                    ++connection_id;
                }
                else {
                    message = "id 0";
                }

                /* Send the packet to the peer over channel id 0. */
                SendENetMessage(message, event.peer);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                //printf("(Server) Message from client : %s\n", event.packet->data);
                printf("A packet of length %u containing \"%s\" was received from %s on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.peer->data,
                    event.channelID);
                
                message = ParseAndGetResponseInput(game, event.peer, connections, PacketToString(event.packet));
                SendENetMessage(message, event.peer);
                // Lets broadcast this message to all
                enet_host_broadcast(server, 0, event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);
                // Reset client's information
                event.peer->data = NULL;
                break;
            }
        }
    }
    return 0;
}
