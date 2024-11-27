// 
#include <iostream>
#include <syslog.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <filesystem>
#include <iostream>
#include <csignal>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <exception>
#include "Daemon.h"
#include "Config.h"
#include "Helper.h"


int main(int argc, char* argv[]) {
	checkArgCount(argc);
	const char* cfg = argv[1];
	checkConfigAccessible(cfg);
	std::string path(cfg);
	std::cout << "Suggested config path: " << path << std::endl;
	Config config = Config(path);

	
	openlog("app_daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Daemon startup");
	Daemon::getInstance().run(path, std::filesystem::current_path(), 5);
	return EXIT_SUCCESS;
}