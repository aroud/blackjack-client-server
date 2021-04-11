#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "server/server.hpp"
#include "game.h"

int main()
{
    //try
    //{
    //    // Check command line arguments.
    //    if (argc != 4)
    //    {
    //        std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
    //        std::cerr << "  For IPv4, try:\n";
    //        std::cerr << "    receiver 0.0.0.0 80 .\n";
    //        std::cerr << "  For IPv6, try:\n";
    //        std::cerr << "    receiver 0::0 80 .\n";
    //        return 1;
    //    }

    //    // Initialise the server.
    //    http::server::Server s(argv[1], argv[2], argv[3]);

    //    // Run the server until stopped.
    //    s.Run();
    //}
    //catch (std::exception& e)
    //{
    //    std::cerr << "exception: " << e.what() << "\n";
    //}

    blackjack::Game game(2);
    game.PlayGame();
    return 0;

    return 0;
}

