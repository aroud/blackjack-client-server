#include "client/client.h"

using options_storage = std::vector<std::pair<std::string, std::string>>;
options_storage ParseConfig(const std::string filename);

int main()
{
    options_storage storage = ParseConfig("client.properties");

    std::string host_key = "server_hostname";
    std::string port_key = "server_port";

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

    std::string host;
    uint16_t port = 0;

    if (it_host == storage.end() || it_port == storage.end()) {
        std::cerr << "Properties in file not found.\n";
        return -1;
    }
    else {
        try {
            host = it_host->second;
            int to_cast = std::stoi(it_port->second);
            port = static_cast<uint16_t>(to_cast);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }

    try {
        Client client(host, port);
        client.MainCycle();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

	return 0;
}