#include <iostream>
#include <unistd.h> 
#include "Helper.h"

void checkArgCount(int argc) {
	if (argc != 2) {
		std::cerr << "Incorrect usage, provide a path to the config.txt file as "
			<< "an argument for a program" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void checkConfigAccessible(const char* config) {
	if (access(config, R_OK) != 0) {
		std::cerr << "Config file " << config << " is unaccessible." << std::endl;
		exit(EXIT_FAILURE);
	}
}


