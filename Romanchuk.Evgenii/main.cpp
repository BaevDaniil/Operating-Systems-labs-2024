#include "daemon.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
        return EXIT_FAILURE;
    }

    openlog("daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started.");
    
    Daemon::get_instance().run(argv[1]);

    return EXIT_SUCCESS;
}
