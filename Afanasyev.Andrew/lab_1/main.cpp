#include <iostream>

#include <limits.h>
#include <unistd.h>

#include "demon.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
		return EXIT_FAILURE;
	}

	if (realpath(argv[1], NULL) == NULL)
	{
		std::cerr << "realpath failed\n";
		return 1;
	}

	return Daemon::get_instance().start(std::string(std::filesystem::absolute(argv[1])));
 
	return 0;
}
