#include "host/host.hpp"

int main()
{
    std::cout << getpid() << std::endl;
    Host<NamedChannel> host("host/host.txt");
    while (true)
    {
        sleep(0.1);
    }
    return 0;
}