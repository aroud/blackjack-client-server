#include <enet/enet.h>

#include <iostream>
#include <string>
#include <sstream> 

#include "game.h"

int main(int argc, char** argv)
{
    ENetAddress address;
    ENetHost* server;
    ENetEvent event;
    int eventStatus;

    //Initialize enet
    if (enet_initialize() != 0) {
        std:: cerr << "An error occured while initializing ENet.\n";
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(
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

    // c. Connect and user service
    eventStatus = 1;

    blackjack::Game game(2);

    while (1) {
        eventStatus = enet_host_service(server, &event, 500);

        if (eventStatus > 0) {
            uint8_t* ptr = nullptr;
            std::ostringstream os;
            std::string str;
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "(Server) We got a new connection from\n" << event.peer->address.host << " host, " <<
                    event.peer->address.port << " port\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                //printf("(Server) Message from client : %s\n", event.packet->data);
                printf("A packet of length %u containing \"%s\" was received from %s on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.peer->data,
                   event.channelID);
                ptr = event.packet->data;
                
                for (int i = 0; i < event.packet->dataLength; ++i) {
                    os << ptr[i];
                }
                str = (os.str());
                std::cout << str;
        
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
        else
        {
           // std::cout << "0\n";
        }
    }

}
//int main()
//{
//    blackjack::Game game(2);
//    game.PlayGame();
//    return 0;
//
//    return 0;
//}

