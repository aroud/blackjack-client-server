#include "server/server.h"



int main()
{
    try {
        Server server(ENET_HOST_ANY, 6789, 4);
        server.GetGame().RegisterPlayers();
        server.PollMessagesCycle();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
   
    

    return 0;
}
