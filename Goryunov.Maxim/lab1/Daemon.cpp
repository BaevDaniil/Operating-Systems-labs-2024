#include "Daemon.h"
#include <sys/types.h>
#include <syslog.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
namespace fs = std::filesystem;

void Daemon::preparePidFile() {
	int pidFile = this->safeOpenPidFile();
	this->checkRunningInstance(pidFile);
	this->terminateExistingProcess(pidFile);

	ftruncate(pidFile, 0);
	lseek(pidFile, 0, SEEK_SET);

	char pidVal[10];
	snprintf(pidVal, sizeof(pidVal), "%d\n", getpid());
	write(pidFile, pidVal, strlen(pidVal));
	cout << "before file creation syslog" << endl;
	syslog(LOG_INFO, "Created pid file %s with pid %i",
		this->pidFilePath.c_str(), getpid());
	close(pidFile);
}

void logErrorFork(pid_t pid) {
	if (pid < 0) {
		syslog(LOG_ERR, "Could not fork daemon from parent: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void stopParent(pid_t pid) {
	if (pid > 0) {
		syslog(LOG_INFO, "This is a parent process. Stopping it.");
		exit(EXIT_SUCCESS);
	}
}

void chdirLogError() {
	if (chdir("/") < 0) {
		syslog(LOG_ERR, "Could not chdir to /: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void logErrorSetsid(pid_t sid) {
	if (sid < 0) {
		syslog(LOG_ERR, "Could not setsid: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void Daemon::forkFromParent() {
	pid_t pid, sid;
	pid = fork();
	cout << "Console message" << endl;
	logErrorFork(pid);
	stopParent(pid);
	syslog(LOG_INFO, "Pid info: %i", pid);
	cout << "Pid info: " << pid << endl;

	// Allow all
	umask(0);

	sid = setsid();
	logErrorSetsid(sid);
	syslog(LOG_INFO, "Sid info: %i", sid);
	cout << "Sid info: " << sid << endl;
	chdirLogError();
	cout << "chdir to /" << endl;
	
	// std::cout no longer works, write to syslog!!!
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	this->preparePidFile();
}

void Daemon::checkRunningInstance(int pidFile) {
	if (lockf(pidFile, F_TLOCK, 0) == -1) {
		string msg = "PidFile is locked because other demong istance is running";
		syslog(LOG_ERR, "%s", msg.c_str());
		exit(EXIT_FAILURE);
	}
}

int Daemon::safeOpenPidFile() {
	int pidFile = open(this->pidFilePath.c_str(), O_RDWR | O_CREAT, 0600);
	if (pidFile == -1) {
		syslog(LOG_ERR, "Could not open PID file %s.", this->pidFilePath.c_str());
		exit(EXIT_FAILURE);
	}
	return pidFile;
}

void Daemon::terminateExistingProcess(int pidFile) {
	char writtenPid[10];
	if (read(pidFile, writtenPid, sizeof(writtenPid) - 1) > 0) {
		int oldPid = atoi(writtenPid);
		syslog(LOG_INFO, "Old pid: %i", oldPid);
		if (oldPid > 0 && kill(oldPid, 0) == 0) {
			syslog(LOG_INFO, "Send SIGTERM to process occupying pid %d.", oldPid);
			kill(oldPid, SIGTERM);
			syslog(LOG_INFO, "Occupying process killed successfully.");
			sleep(1);
		}
	}
}

void Daemon::saveConfigRules() {
	this->rules = this->config.getRules();
}

void signalHandler(int signal) {
	switch (signal) {
		case SIGHUP:
			syslog(LOG_INFO, "Response to SIGHUP, re-reading config file");
			Daemon::getInstance().handleSighup();
			break;
		case SIGTERM:
			syslog(LOG_INFO, "Response to SIGTERM, stop everything.");
			Daemon::getInstance().handleSigterm();
			break;
		default:
			syslog(LOG_NOTICE, "Receive unknown signal %i, no action taken", signal);
			break;
	}
}

void Daemon::handleSighup() {
	this->saveConfigRules();
}

void Daemon::handleSigterm() {
	closelog();
	exit(EXIT_SUCCESS);
}

fs::path makeAbsolutePath(string const& pathStr, fs::path const& initDir) {
	fs::path path = fs::path(pathStr);
	if (!path.is_absolute()) {
		path = initDir / pathStr;
	}
	return path;
}

bool Daemon::copyFiles(PathedConfigRule const& rule) {
	fs::path source = rule.source;
	fs::path destination = rule.destination;
	string extension = rule.extension;
	if (extension.empty()) {
		syslog(LOG_ERR, "Emtpy extension, can't copy files.");
		return false;
	}
	//for path::extension comparison
	if (extension[0] != '.') {
		extension = '.' + extension;
	}
	syslog(LOG_INFO, "Extension for copying: %s", extension.c_str());

	for (const auto& file : fs::recursive_directory_iterator(source)) {
		if (file.is_regular_file() && file.path().extension() == extension) {
			string sourcePath = file.path().string();
			string fileTail = sourcePath.substr(source.string().size() + 1);
			fs::path pathFileTail = fs::path(fileTail);

			syslog(LOG_INFO, "DESTINATION: %s, FILETAIL: %s", destination.string().c_str(), pathFileTail.string().c_str());
			fs::path copied = destination / pathFileTail;
			fs::path copiedCopy = copied;
			fs::create_directories(copiedCopy.remove_filename());
			syslog(LOG_INFO, "Copying FILES %s to %s", file.path().string().c_str(), copied.string().c_str());
			fs::copy_file(file.path(), copied, fs::copy_options::overwrite_existing);
		}
	}
	syslog(
		LOG_INFO, "Finished copying from %s to %s.",
		source.string().c_str(), destination.string().c_str()
	);
	return true;
}

PathedConfigRule Daemon::ruleToPathed(ConfigRule const& rule, std::filesystem::path const& initDir) {
	fs::path source = makeAbsolutePath(rule.source, initDir);
	syslog(LOG_INFO, "Source dir: %s", source.string().c_str());
	fs::path destination = makeAbsolutePath(rule.destination, initDir);
	syslog(LOG_INFO, "Destination dir: %s", destination.string().c_str());
	PathedConfigRule pathed;
	pathed.source = source;
	pathed.destination = destination;
	pathed.extension = rule.extension;
	return pathed;
}

bool Daemon::setupFolders(PathedConfigRule const& rule) {
	// Create existence
	if (!fs::exists(rule.source)) {
		syslog(LOG_ERR, "Source dir %s does not exist, cannot copy.", rule.source.string().c_str());
		return false;
	}
	if (!fs::exists(rule.destination)) {
		syslog(LOG_INFO, "Destination dir %s does not exist, creating it.", rule.destination.string().c_str());
		fs::create_directory(rule.destination);
	}
	if (!fs::is_directory(rule.source) || !fs::is_directory(rule.destination)) {
		syslog(LOG_ERR, "Either %s or %s is not a folder.",
			rule.source.string().c_str(), rule.destination.string().c_str()
		);
		return false;
	}
	for (const auto& entry : fs::directory_iterator(rule.destination)) {
		fs::remove_all(entry);
	}
	syslog(LOG_INFO, "Cleaned files in %s.", rule.destination.string().c_str());
	return true;
}

void Daemon::run(
	string const& path,
	std::filesystem::path const& initDir,
	int repeat
) {
	this->forkFromParent(); // own process now

	this->config = Config(path);
	this->saveConfigRules();

	signal(SIGHUP, signalHandler);
	signal(SIGTERM, signalHandler);

	while (true) {
		syslog(LOG_INFO, "Begin copying files from source folders to destinations.");
		for (auto const& rule : this->rules) {
			PathedConfigRule pathed = this->ruleToPathed(rule, initDir);
			bool checked = this->setupFolders(pathed);
			if (!checked) {
				syslog(
					LOG_ERR,
					"Some error occured, incorrect foldeer format."
				);
				continue;
			}
		}
		for (auto const& rule : this->rules) {
			try {
				PathedConfigRule pathed = this->ruleToPathed(rule, initDir);
				bool success = this->copyFiles(pathed);
				if (!success) {
					syslog(
						LOG_ERR,
						"Could not copy files from %s to %s for some reason.",
						rule.source.c_str(),
						rule.destination.c_str()
					);
				}
			}
			catch (const std::exception& e) {
				syslog(
					LOG_ERR,
					"During copying from %s to %s an exception occured: %s.",
					rule.source.c_str(),
					rule.destination.c_str(),
					e.what()
				);
			}
		}
		syslog(LOG_INFO, "Finished copying files.");
		sleep(repeat);
	}
}
