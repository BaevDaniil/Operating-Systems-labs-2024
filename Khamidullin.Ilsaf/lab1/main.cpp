#include "daemon.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <configuration_file>" << std::endl;
        return EXIT_FAILURE;
    }

    DeepCleanerDaemon::Get().Initialize(argv[1]);
    DeepCleanerDaemon::Get().Run();

    return EXIT_SUCCESS;
}