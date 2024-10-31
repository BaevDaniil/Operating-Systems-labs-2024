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
	std::string path = "config.txt";
	Config config = Config(path);
	//std::cout << config.path << std::endl;
	std::vector<ConfigRule> rules = config.getRules();
	// std::cout << rules.size() << std::endl;
	for (const auto& rule : rules) {
		std::cout << rule.source << ";" << rule.destination << ";" << rule.extension << std::endl;
	}
	openlog("app_daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Daemon startup");
	Daemon::getInstance().run(std::string(cfg), std::filesystem::current_path());
	return EXIT_SUCCESS;
}