#include <enet/enet.h>
#include <string>
#include <iostream>
#include <string.h>

int main()
{
    ENetAddress address;
    ENetHost* client;
    ENetPeer* peer;
    char message[1024];
    ENetEvent event;
    int eventStatus;

    // Initialize enet
    if (enet_initialize() != 0) {
        std::cerr << "An error occured while initializing ENet.\n";
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);

    // Create a host using enet_host_create
    client = enet_host_create(
        NULL,   /* create a client host */
        1,      /* only allow 1 outgoing connection */
        2,      /* allow up 2 channels to be used, 0 and 1 */
        0,      /* assume any amount of incoming bandwidth */
        0       /* assume any amount of outgoing bandwidth */
    );

    if (client == NULL) {
        std::cerr << "An error occured while trying to create an ENet server host\n";
        exit(EXIT_FAILURE);
    }

    enet_address_set_host(&address, "localhost");
    address.port = 1234;

    // c. Connect and user service
    peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL) {
        std::cerr << "No available peers for initializing an ENet connection";
        exit(EXIT_FAILURE);
    }

    eventStatus = 1;
   
    while (1) {
        eventStatus = enet_host_service(client, &event, 5000);

        // If we had some event that interested us
        if (eventStatus > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("(Client) We got a new connection from %x\n",
                    event.peer->address.host);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %u containing \"%s\" was received from %s on channel %u.\n",
                    event.packet->dataLength,
                    event.packet->data,
                    event.peer->data,
                    event.channelID);
                // Lets broadcast this message to all
                // enet_host_broadcast(client, 0, event.packet);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("(Client) %s disconnected.\n", event.peer->data);

                // Reset client's information
                event.peer->data = NULL;
                break;
            }
        }
        
        printf("Say > ");
        std::cin >> message;
        
        if (strlen(message) > 0) {
            ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);
            /* Send the packet to the peer over channel id 0. */
            enet_peer_send(peer, 0, packet);
        }
    }
}