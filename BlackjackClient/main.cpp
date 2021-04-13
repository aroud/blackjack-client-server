#include "client/client.h"

int main()
{
    try {
        Client client("127.0.0.1", 6789);
        client.MainCycle();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

	return 0;
}