#include "server/server.h"



int main()
{
    try {
        Server server("127.0.0.1", 6789, 4);
        server.GetGame().RegisterPlayers();
        server.MainCycle();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }  

    return 0;
}
