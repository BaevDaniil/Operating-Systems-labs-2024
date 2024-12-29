#include "daemon.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstring>

Daemon& Daemon::getInstance() {
    static Daemon instance;
    return instance;
}

Daemon::Daemon() : interval(30), pid(0), pidFilePath("/tmp/daemon.pid") {
    openlog("daemon", LOG_PID | LOG_CONS, LOG_DAEMON);
}

Daemon::~Daemon() {
    closelog();
    removePidFile();
}

void Daemon::setConfigFilePath(const std::string& path) {
    configFilePath = path;
}

void Daemon::start() {
    checkAndTerminateExistingDaemon();
    daemonize();
    handleSignals();
    readConfig();
    writePidFile();

    while (true) {
        processDirectories();
        sleep(interval);
    }
}

void Daemon::stop() {
    syslog(LOG_INFO, "Daemon is stopping");
    closelog();
    removePidFile();
    exit(0);
}

void Daemon::reloadConfig() {
    syslog(LOG_INFO, "Reloading configuration file");
    readConfig();
}

void Daemon::daemonize() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        currentWorkingDir = std::string(cwd);
    }
    else {
        syslog(LOG_ERR, "Failed to get current working directory");
    }

    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Fork failed");
        exit(1);
    }
    if (pid > 0) {
        exit(0);
    }

    umask(0);
    setsid();

    if (chdir("/") < 0) {
        syslog(LOG_ERR, "Chdir failed");
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
}

void Daemon::handleSignals() {
    signal(SIGHUP, [](int) { Daemon::getInstance().reloadConfig(); });
    signal(SIGTERM, [](int) { Daemon::getInstance().stop(); });
}

void Daemon::readConfig() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        syslog(LOG_ERR, "Failed to open config file: %s", configFilePath.c_str());
        return;
    }

    directories.clear();
    std::string folder, date;
    while (configFile >> folder >> date) {
        char absolutePath[PATH_MAX];
        if (realpath(folder.c_str(), absolutePath) == nullptr) {
            std::string fullPath = currentWorkingDir + "/" + folder;
            if (realpath(fullPath.c_str(), absolutePath) == nullptr) {
                syslog(LOG_ERR, "Failed to resolve path: %s", folder.c_str());
                continue;
            }
        }
        directories.push_back(std::pair<std::string, std::string>(std::string(absolutePath), date));
    }

    configFile.close();
}

void Daemon::processDirectories() {
    for (const auto& dir : directories) {
        struct stat st;
        if (stat(dir.first.c_str(), &st) == 0) {
            std::time_t folderTime = st.st_mtime;
            std::tm folderTm = *std::localtime(&folderTime);

            std::tm dateTm = {};
            std::istringstream dateStream(dir.second);
            dateStream >> std::get_time(&dateTm, "%Y-%m-%d");

            syslog(LOG_INFO, "Folder time: %ld, Config time: %ld", std::mktime(&folderTm), std::mktime(&dateTm));
            if (std::mktime(&folderTm) < std::mktime(&dateTm)) {
                syslog(LOG_INFO, "Deleting contents of %s", dir.first.c_str());
                removeDirectoryContents(dir.first);
            }
        }
    }
}

void Daemon::removeDirectoryContents(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string filePath = path + "/" + entry->d_name;
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                if (entry->d_type == DT_DIR) {
                    removeDirectoryContents(filePath);
                    rmdir(filePath.c_str());
                }
                else {
                    unlink(filePath.c_str());
                }
            }
        }
        closedir(dir);
    }
}

void Daemon::checkAndTerminateExistingDaemon() {
    std::ifstream pidFile(pidFilePath);
    if (pidFile.is_open()) {
        pid_t existingPid;
        pidFile >> existingPid;
        pidFile.close();

        if (kill(existingPid, 0) == 0) {
            syslog(LOG_INFO, "Terminating existing daemon with PID %d", existingPid);
            kill(existingPid, SIGTERM);
            sleep(1);
        }
    }
}

void Daemon::writePidFile() {
    std::ofstream pidFile(pidFilePath);
    if (pidFile.is_open()) {
        pidFile << getpid();
        pidFile.close();
    }
    else {
        syslog(LOG_ERR, "Failed to write PID file");
    }
}

void Daemon::removePidFile() {
    std::remove(pidFilePath.c_str());
}