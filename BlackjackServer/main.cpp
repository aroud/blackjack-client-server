#include "server/server.h"
using options_storage = std::vector<std::pair<std::string, std::string>>;
options_storage ParseConfig(const std::string filename);

int main()
{
    options_storage storage = ParseConfig("server.properties");

    std::string host_key = "hostname";
    std::string port_key = "port";
    std::string decks_num_key = "decks_number";

    auto it_host = std::find_if(begin(storage), end(storage),
        [&host_key](const std::pair<std::string, std::string>& p) {
            return p.first == host_key;
        }
    );
    auto it_port = std::find_if(begin(storage), end(storage),
        [&port_key](const std::pair<std::string, std::string>& p) {
            return p.first == port_key;
        }
    );
    auto it_decks_num = std::find_if(begin(storage), end(storage),
        [&decks_num_key](const std::pair<std::string, std::string>& p) {
            return p.first == decks_num_key;
        }
    );

    std::string host;
    uint16_t port = 0;
    size_t decks_num = 0;

    if (it_host == storage.end() || it_port == storage.end() || it_decks_num==storage.end()) {
        std::cerr << "Properties in file not found.\n";
        return -1;
    }
    else {
        try {
            host = it_host->second;
            unsigned long to_cast_port = std::stoul(it_port->second);
            unsigned long long to_cast_decks_num = std::stoull(it_decks_num->second);
            port = static_cast<uint16_t>(to_cast_port);
            decks_num = static_cast<size_t>(to_cast_decks_num);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }

    try {
        Server server(host, port, decks_num);
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
