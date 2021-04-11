#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "server/server.hpp"
#include "game.h"

int main()
{
    try
    {
        // Initialise the server.
        http::server::Server s("127.0.0.1", "6789", "");

        // Run the server until stopped.
        s.Run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

   /* blackjack::Game game(2);
    game.PlayGame();
    return 0;*/

    return 0;
}

