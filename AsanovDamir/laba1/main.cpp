#include "Deamon.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Wrong count of argument. Expected path to config" << std::endl;
        return 1;
    }
    const char* config_path = argv[1];
    Deamon::get_instance().start(config_path);
    return 0;
}