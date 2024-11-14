#include "client.hpp"

int main()
{
    std::cout << getpid() << std::endl;
    Client<NamedChannel> client(std::filesystem::current_path() / "host/host.txt");
    while(true)
    {
    }
    return 0;
}