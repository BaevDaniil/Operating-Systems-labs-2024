#include "demon.h"

static void kill_existing_process(const std::string& pid_file)
{
	int pid_file_handle = open(pid_file.c_str(), O_RDWR | O_CREAT, 0600);
	if (pid_file_handle == -1)
	{
		syslog(LOG_ERR, "PID file %s cannot be opened", pid_file.c_str());
		exit(EXIT_FAILURE);
	}

	if (lockf(pid_file_handle, F_TLOCK, 0) == -1)
	{
		syslog(LOG_ERR, "Daemon is already running (PID file is locked)");
		exit(EXIT_FAILURE);
	}

	char old_pid_str[10];
	if (read(pid_file_handle, old_pid_str, sizeof(old_pid_str) - 1) > 0)
	{
		int old_pid = atoi(old_pid_str);

		if (old_pid > 0 && kill(old_pid, 0) == 0)
		{
			syslog(LOG_INFO, "Process with PID %d is already running, sending SIGTERM", old_pid);
			kill(old_pid, SIGTERM);
			sleep(1);
		}
		else
		{
			syslog(LOG_INFO, "No process found with PID %d, continuing...", old_pid);
		}
	}

	ftruncate(pid_file_handle, 0);
	lseek(pid_file_handle, 0, SEEK_SET);

	char str[10];
	snprintf(str, sizeof(str), "%d\n", getpid());
	write(pid_file_handle, str, strlen(str));

	syslog(LOG_INFO, "PID file %s created successfully with PID %d", pid_file.c_str(), getpid());

	close(pid_file_handle);
}

static bool read_config(std::vector<config_line> &config, const std::string &CONFIG_FILE, const fs::path current_path)
{
	std::ifstream fin(CONFIG_FILE);

	if (!fin.is_open())
	{
		return true;
	}

	std::string line;

	while (std::getline(fin, line))
	{
		config_line config_line;
		static const std::regex re(R"(^\s*("?)([^"]+)\1\s+("?)([^"]+)\3\s+("?)([^"]+)\5\s*$)");
		std::smatch match;

		if (std::regex_match(line, match, re) && match.size() == 7)
		{
			auto from_path = fs::path(match[2].str());
			config_line.from = from_path.is_absolute() ? from_path : current_path / from_path;
			auto to_path = fs::path(match[4].str());
			config_line.to = to_path.is_absolute() ? to_path : current_path / to_path;
			config_line.ext = match[6].str();
		}
		else
		{
			syslog(LOG_ERR, "Line %s is not correct", line.c_str());
			continue;
		}

		syslog(LOG_DEBUG, "Read config line: '%s' -> '%s' (%s)",
			   config_line.from.c_str(), config_line.to.c_str(), config_line.ext.c_str());
		config.push_back(std::move(config_line));
	}

	fin.close();

	return true;
}

inline bool ends_with(const std::string& value, const std::string& ending)
{
    return value.size() > ending.size() &&
		   std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

static void work(const std::vector<config_line>& config)
{
	std::unordered_set<fs::path> cleaned_dirs;

	for (const auto& config_line : config)
	{
		try
		{
			if (cleaned_dirs.find(config_line.to) == cleaned_dirs.end())
			{
				for (const auto& entry : fs::directory_iterator(config_line.to))
				{
					fs::remove(entry.path());
				}

				cleaned_dirs.insert(config_line.to);
			}

			for (const auto& entry : fs::directory_iterator(config_line.from))
			{
				if (!entry.is_regular_file() ||
					!ends_with(entry.path(), config_line.ext))
				{
					continue;
				}

				syslog(LOG_DEBUG, "Copy '%s' -> '%s'", entry.path().c_str(), config_line.to.c_str());

				try
				{
					if (!fs::copy_file(entry.path(), config_line.to / entry.path().filename()))
					{
						syslog(LOG_ERR, "Failed copying '%s'", entry.path().c_str());
					}
				}
				catch(const std::exception& e)
				{
					syslog(LOG_ERR, "%s", e.what());
				}
			}
		}
		catch (const std::exception& e)
		{
			syslog(LOG_ERR, "%s", e.what());
		}
	}
}

void sighup_fun(int sig)
{
	syslog(LOG_NOTICE, "Reload config");

	if (!read_config(Daemon::get_instance().config, Daemon::get_instance().CONFIG_FILE, Daemon::get_instance().current_path))
	{
		syslog(LOG_ERR, "Failed reloading config");
		closelog();
		exit(1);
	}
	syslog(LOG_DEBUG, "File was reloaded");
}

void sigterm_fun(int sig)
{
	syslog(LOG_NOTICE, "Terminate");
	closelog();
	exit(0);
}

bool Daemon::start(const std::string& config_file)
{
	current_path = fs::current_path();
	openlog("demon", LOG_PID | LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Daemon starts"); 

	pid_t pid, sid;
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	if ((chdir("/")) < 0)
		exit(EXIT_FAILURE);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	CONFIG_FILE = config_file;

	kill_existing_process(PID_FILE);

	struct sigaction sighup_action;
	memset(&sighup_action, 0, sizeof(sighup_action));
	sighup_action.sa_handler = &sighup_fun;

	struct sigaction sigterm_action;
	memset(&sigterm_action, 0, sizeof(sigterm_action));
	sigterm_action.sa_handler = &sigterm_fun;

	sigaction(SIGHUP, &sighup_action, NULL);
	sigaction(SIGTERM, &sigterm_action, NULL);

	if (!read_config(config, CONFIG_FILE, current_path))
	{
		syslog(LOG_ERR, "Error while reading config");
		return false;
	}

	while(true)
	{
		work(config);
		sleep(30);
		syslog(LOG_INFO, "awake after 30 seconds");
	}

	return true;
}

