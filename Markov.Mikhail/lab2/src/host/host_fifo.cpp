#include "host.hpp"

int main()
{
    std::cout << getpid() << std::endl;
    Host<NamedChannel> host(std::filesystem::current_path() / "host/host.txt");
    while(true)
    {
        
    }
    return 0;
}