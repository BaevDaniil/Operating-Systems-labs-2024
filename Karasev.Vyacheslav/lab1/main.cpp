#include "daemon.hpp"
#include <iostream>
#include <limits.h>
#include <unistd.h> 



int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    char configFilePath[PATH_MAX];
    if (realpath(argv[1], configFilePath) == nullptr) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::string fullPath = std::string(cwd) + "/" + argv[1];
            if (realpath(fullPath.c_str(), configFilePath) == nullptr) {
                std::cerr << "Error: Cannot resolve config file path: " << argv[1] << std::endl;
                return 1;
            }
        }
        else {
            std::cerr << "Error: Cannot get current working directory" << std::endl;
            return 1;
        }
    }

    Daemon& daemon = Daemon::getInstance();
    daemon.setConfigFilePath(configFilePath);
    daemon.start();

    return 0;
}