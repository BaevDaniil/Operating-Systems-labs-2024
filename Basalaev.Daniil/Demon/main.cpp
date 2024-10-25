#include "Demon.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Failed. No path to config was recieved" << std::endl;
        return 1;
    }
    const char* configPath = argv[1];
    Demon::getInstance().start(configPath);
    return 0;
}
